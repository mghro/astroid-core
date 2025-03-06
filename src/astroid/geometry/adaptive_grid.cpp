#include <astroid/geometry/adaptive_grid.hpp>

#include <cstdint>
#include <iostream>
#include <queue>
#include <stack>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/scoped_array.hpp>
#include <boost/tuple/tuple.hpp>

#include <astroid/geometry/common.hpp>

#define DIM 3
#define QO 8
#define QL 6

namespace astroid {

// This wraps another check_in_interface and reduces the frequency at which
// the wrapped interface is called by the given factor.
struct check_in_frequency_reducer : check_in_interface
{
    check_in_frequency_reducer(check_in_interface* wrapped, int factor)
        : wrapped_(wrapped), factor_(factor), count_(0)
    {
    }

    void
    operator()()
    {
        ++count_;
        if (count_ >= factor_)
        {
            (*wrapped_)();
            count_ = 0;
        }
    }

 private:
    check_in_interface* wrapped_;
    int factor_;
    int count_;
};

box3d
get_octree_box(box3d const& extents, uint64_t index)
{
    vector3d offset = make_vector(0.0, 0.0, 0.0);
    double del = 1.0;
    uint64_t k = index;
    uint64_t kb;
    while (k > 1)
    {
        kb = (k + QL) % QO;
        for (int j = 0; j < DIM; ++j)
        {
            if (kb & (uint64_t(1) << j))
            {
                offset[j] += del;
            }
        }
        k = (k + QL) >> DIM;
        del *= 2.0;
    }
    vector3d plo, phi;
    for (int j = 0; j < DIM; ++j)
    {
        plo[j] = extents.corner[j] + extents.size[j] * offset[j] / del;
        phi[j] = extents.corner[j] + extents.size[j] * (offset[j] + 1) / del;
    }
    return make_box(plo, phi - plo);
}

int
get_octree_depth(box3d const& extents, uint64_t index)
{
    int depth = 0;
    uint64_t k = index;
    while (k > 1)
    {
        k = (k + QL) >> DIM;
        depth++;
    }
    return depth;
}

regular_grid<3, double>
regularize(adaptive_grid const& grid)
{
    // Determine minimum box
    uint64_t max_index = 0;
    for (auto const& v : grid.voxels)
    {
        if (v.index > max_index)
        {
            max_index = v.index;
        }
    }
    box3d min_box = get_octree_box(grid.extents, max_index);

    return make_grid_for_box(grid.bounds, min_box.size);
}

template<class Pixel>
image3
to_image(
    // Adaptive grid to translate into an image
    adaptive_grid const& grid,
    // Array of numeric values that correspond to the value at the grid point
    Pixel const* field,
    // Units for the returned image
    units const& img_unit)
{
    // Create image
    regular_grid3d regular_grid = regularize(grid);
    image<3, Pixel, unique> img;
    create_image_on_grid(img, regular_grid);

    // Transfer data to image
    auto voxel_count = grid.voxels.size();
    unsigned n = product(img.size);

    // Make sure pixel values are set to 0
    for (unsigned i = 0; i < n; i++)
    {
        img.pixels.ptr[i] = (Pixel) 0;
    }

    for (unsigned i = 0; i < voxel_count; ++i)
    {

        // Determine start and end vertices of voxel in image
        auto box = get_octree_box(grid.extents, grid.voxels[i].index);
        vector3u start, end;
        for (int k = 0; k < 3; ++k)
        {
            auto s = unsigned(std::max(
                regular_grid.n_points[k]
                    * (box.corner[k] - grid.bounds.corner[k])
                    / grid.bounds.size[k],
                0.));
            start[k] = unsigned(std::max(
                std::min(
                    s,
                    unsigned(
                        regular_grid.n_points[k] > 0
                            ? regular_grid.n_points[k] - 1
                            : 0)),
                0u));
            auto e = unsigned(
                (regular_grid.n_points[k]
                 * (box.corner[k] + box.size[k] - grid.bounds.corner[k])
                 / grid.bounds.size[k]));
            end[k] = std::min(e, regular_grid.n_points[k]);
        }

        // Set values in image
        vector3u index;
        for (auto r = start[0]; r < end[0]; ++r)
        {
            index[0] = r;
            for (auto s = start[1]; s < end[1]; ++s)
            {
                index[1] = s;
                for (auto t = start[2]; t < end[2]; ++t)
                {
                    index[2] = t;
                    *get_pixel_iterator(img, index) = field[i];
                }
            }
        }
    }

    img.units = img_unit;

    // Return image
    return as_variant(share(img));
}

image3
adaptive_grid_doubles_to_image(
    adaptive_grid const& grid,
    std::vector<double> const& field,
    units const& img_unit)
{
    if (field.size() != grid.voxels.size())
    {
        throw exception(
            "adaptive_grid: double field size does not match grid");
    }
    return to_image(grid, &field[0], img_unit);
}

image3
adaptive_grid_floats_to_image(
    adaptive_grid const& grid,
    std::vector<float> const& field,
    units const& img_unit)
{
    if (field.size() != grid.voxels.size())
    {
        throw exception("adaptive_grid: float field size does not match grid");
    }
    return to_image(grid, &field[0], img_unit);
}

image3
adaptive_grid_array_to_image(
    adaptive_grid const& grid,
    array<float> const& field,
    units const& img_unit)
{
    if (field.size() != grid.voxels.size())
    {
        throw exception(
            "adaptive_grid: array field size does not match grid: "
            + to_string(field.size()) + ", " + to_string(grid.voxels.size()));
    }
    return to_image(grid, &field[0], img_unit);
}

// TODO: Thread this function !!! (or better yet, refactor to use something
// else)
adaptive_grid
limit_adaptive_grid_by_structure(
    adaptive_grid const& grid, structure_geometry const& structure)
{
    adaptive_grid new_grid;
    new_grid.extents = grid.extents;
    new_grid.bounds = grid.bounds;
    new_grid.volumes = grid.volumes;

    std::vector<adaptive_grid_voxel> voxels;

    for (auto const& v : grid.voxels)
    {
        bool inside = false;
        auto box = get_octree_box(grid.extents, v.index);
        // Loop over each of the 8 corners of the box
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    // If corner is inside of structure, increment the count
                    if (point_in_structure(
                            make_vector(
                                box.corner[0] + box.size[0] * double(i),
                                box.corner[1] + box.size[1] * double(j),
                                box.corner[2] + box.size[2] * double(k)),
                            structure))
                    {
                        inside = true;
                        break;
                    }
                }
                if (inside)
                {
                    break;
                }
            }
            if (inside)
            {
                break;
            }
        }
        if (inside)
        {
            voxels.push_back(v);
        }
    }

    initialize(&new_grid.voxels, voxels);

    return new_grid;
}

std::vector<vector3d>
get_points_on_adaptive_grid(
    check_in_interface& original_check_in,
    progress_reporter_interface& reporter,
    adaptive_grid const& grid)
{
    check_in_frequency_reducer check_in(&original_check_in, 1000);
    std::vector<vector3d> points;
    points.reserve(grid.voxels.size());
    for (auto const& voxel : grid.voxels)
    {
        check_in();
        auto box = get_octree_box(grid.extents, voxel.index);
        points.push_back(get_center(box));
    }
    return points;
}

std::vector<box3d>
adaptive_grid_voxel_boxes(
    check_in_interface& original_check_in,
    progress_reporter_interface& reporter,
    adaptive_grid const& grid)
{
    check_in_frequency_reducer check_in(&original_check_in, 1000);
    auto voxel_count = grid.voxels.size();
    std::vector<box3d> boxes(voxel_count);
    for (size_t i = 0; i < voxel_count; ++i)
    {
        check_in();
        boxes[i] = get_octree_box(grid.extents, grid.voxels[i].index);
    }
    return boxes;
}

} // namespace astroid
