/*
 * Author(s):  Salvadore Gerace <sgerace@dotdecimal.com>
 * Date:       11/21/2013
 *
 * Copyright:
 * This work was developed as a joint effort between .decimal, LLC and
 * Partners HealthCare under research agreement A213686; as such, it is
 * jointly copyrighted by the participating organizations.
 * (c) 2013 .decimal, LLC. All rights reserved.
 * (c) 2013 Partners HealthCare. All rights reserved.
 */

#ifndef CRADLE_GEOMETRY_ADAPTIVE_GRID_HPP
#define CRADLE_GEOMETRY_ADAPTIVE_GRID_HPP

#include <cradle/common.hpp>
#include <cradle/geometry/common.hpp>
#include <cradle/geometry/meshing.hpp>
#include <cradle/imaging/geometry.hpp>
#include <cradle/imaging/variant.hpp>
#include <vector>

namespace cradle {

// Defines a volume in space surrounding a structure that will be discretized
// down below a given maximum spacing.
api(struct)
struct adaptive_grid_region
{
    // A triangulated structure to define the region.
    optimized_triangle_mesh region;
    // The region will be discretized so that point spacings are below this
    // value.
    double maximum_spacing;
};

typedef std::vector<adaptive_grid_region> adaptive_grid_region_list;

// An individual box within an adaptive grid octree.
api(struct)
struct adaptive_grid_voxel
{
    // Index of the box.
    uint64_t index;
    // Offset into the volumes array for the grid (size is inside_count +
    // surface_count)
    uint32_t volume_offset;
    // Count of regions this voxel is within
    uint16_t inside_count;
    // Count of regions whose surface intersects this voxel
    uint16_t surface_count;
};

// An octree grid that allows for variable point spacings within each region.
api(struct)
struct adaptive_grid
{
    // The bounds of the octree
    box3d extents;
    // The bounds of the voxels
    box3d bounds;
    // List of all octree boxes; these boxes must lie within the bounds
    // (struct: adaptive_grid_voxel)
    cradle::array<adaptive_grid_voxel> voxels;
    // List of region indices that voxels interact with (us)
    cradle::array<uint16_t> volumes;
};

box3d
get_octree_box(box3d const& extents, uint64_t index);

int
get_octree_depth(box3d const& extents, uint64_t index);

template<class Pixel>
image3
to_image(
    // Adaptive grid to translate into an image
    adaptive_grid const& grid,
    // Array of numeric values that correspond to the value at the grid point
    Pixel const* field)
{
    // Create image
    regular_grid3d regular_grid = regularize(grid);
    image<3, Pixel, cradle::unique> img;
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

    // Return image
    return as_variant(share(img));
}

} // namespace cradle

#endif
