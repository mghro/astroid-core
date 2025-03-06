#include <astroid/imaging/inclusion_image.hpp>

#include <boost/next_prior.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/thread/mutex.hpp>

#include <astroid/geometry/clipper.hpp>
#include <astroid/geometry/grid_points.hpp>
#include <astroid/geometry/regular_grid.hpp>
#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/merge_slices.hpp>
#include <astroid/imaging/utilities.hpp>
#include <astroid/simple_concurrency.hpp>

#include <algorithm>

namespace astroid {

namespace {

struct image_pixel_recorder
{
    float* pixels;
};

template<class T>
void static inline record_index_value(
    image_pixel_recorder& recorder, T index, float weight)
{
    recorder.pixels[index] = weight;
}

void static inline skip_indices(
    image_pixel_recorder& recorder, size_t start_index, size_t count)
{
    size_t end_index = start_index + count;
    for (size_t i = start_index; i != end_index; ++i)
        recorder.pixels[i] = 0;
}

struct weighted_index_recorder
{
    std::vector<weighted_grid_index>* indices;
};

void
record_index_value(
    weighted_index_recorder& recorder, unsigned index, float weight)
{
    if (weight != 0)
    {
        weighted_grid_index wgi;
        wgi.index = index;
        wgi.weight = weight;
        recorder.indices->push_back(wgi);
    }
}

void
skip_indices(
    weighted_index_recorder& recorder, unsigned start_index, unsigned count)
{
}

template<class Recorder>
void
compute_polyset_inclusion_indices(
    Recorder& recorder,
    unsigned start_index,
    regular_grid<2, double> const& grid,
    polyset const& set,
    bool use_small_voxel_assumption = false)
{
    if (is_empty(set))
    {
        skip_indices(recorder, start_index, product(grid.n_points));
        return;
    }

    auto clipper_set = to_clipper(set);

    clipper_polyset pixel_polyset(1);
    pixel_polyset[0].resize(4);

    auto bounds = bounding_box(set);

    auto index = start_index;

    auto grid_point_list = make_grid_point_list(grid);
    auto grid_point = grid_point_list.begin();
    auto half_spacing = 0.5 * grid.spacing;
    auto inv_grid_voxel_area = 1. / product(grid.spacing);
    auto bounds_high_corner_y = get_high_corner(bounds)[1];

    for (unsigned i = 0; i != grid.n_points[1]; ++i)
    {
        // If this grid row lies entirely outside the bounding box, skip it.
        // TODO: We could skip all rows before and after as single blocks.
        double y = grid.p0[1] - half_spacing[1] + grid.spacing[1] * i;
        if (y > bounds_high_corner_y || y + grid.spacing[1] < bounds.corner[1])
        {
            skip_indices(recorder, index, grid.n_points[0]);
            index += grid.n_points[0];
            grid_point += grid.n_points[0];
            continue;
        }

        for (unsigned j = 0; j != grid.n_points[0]; ++j, ++index, ++grid_point)
        {
            auto pixel_box
                = make_box(*grid_point - half_spacing, grid.spacing);

            // If there's no intersection between the pixel and the bounding
            // box, don't bother using Clipper.
            {
                box<2, double> tmp;
                if (!compute_intersection(&tmp, bounds, pixel_box))
                {
                    skip_indices(recorder, index, 1);
                    continue;
                }
            }

            // Check corners, which is valid if we assume the voxels are small
            if (use_small_voxel_assumption)
            {
                char corners_inside = 0;
                // Loop over each of the 4 corners of the box
                for (int i = 0; i < 2; ++i)
                {
                    for (int j = 0; j < 2; ++j)
                    {
                        // If corner is inside of polyset, increment the count
                        if (point_in_polyset(
                                make_vector(
                                    pixel_box.corner[0]
                                        + pixel_box.size[0] * double(i),
                                    pixel_box.corner[1]
                                        + pixel_box.size[1] * double(j)),
                                set))
                        {
                            ++corners_inside;
                        }
                    }
                }

                if (corners_inside == 0)
                {
                    record_index_value(recorder, index, 0.0);
                    continue;
                }
                else if (corners_inside == 4)
                {
                    record_index_value(recorder, index, 1.0);
                    continue;
                }
            }

            ClipperLib::IntPoint* vertices = &pixel_polyset[0][0];
            vertices[0] = to_clipper(pixel_box.corner);
            vertices[1] = to_clipper(
                pixel_box.corner + make_vector(pixel_box.size[0], 0.));
            vertices[2] = to_clipper(pixel_box.corner + pixel_box.size);
            vertices[3] = to_clipper(
                pixel_box.corner + make_vector(0., pixel_box.size[1]));

            clipper_polyset intersection;

            ClipperLib::Clipper clipper;
            clipper.AddPaths(clipper_set, ClipperLib::ptSubject, true);
            clipper.AddPaths(pixel_polyset, ClipperLib::ptClip, true);
            clipper.Execute(ClipperLib::ctIntersection, intersection);

            record_index_value(
                recorder,
                index,
                float(get_area(intersection) * inv_grid_voxel_area));
        }
    }
}

} // namespace

image<2, float, shared>
compute_polyset_inclusion_image(
    regular_grid<2, double> const& grid, polyset const& set)
{
    image<2, float, unique> img;
    create_image_on_grid(img, grid);
    image_pixel_recorder recorder;
    recorder.pixels = img.pixels.ptr;
    compute_polyset_inclusion_indices(recorder, 0, grid, set);
    return share(img);
}

grid_cell_inclusion_info
compute_grid_cells_in_polyset(
    regular_grid<2, double> const& grid, polyset const& set)
{
    grid_cell_inclusion_info info;
    weighted_index_recorder recorder;
    recorder.indices = &info.cells_inside;
    compute_polyset_inclusion_indices(recorder, 0, grid, set);
    return info;
}

template<class Recorder>
void
compute_circle_inclusion_indices(
    Recorder& recorder,
    unsigned start_index,
    regular_grid<2, double> const& grid,
    circle<double> const& circle)
{
    auto index = start_index;

    auto grid_point_list = make_grid_point_list(grid);
    auto grid_point = grid_point_list.begin();

    double const r_squared = square(circle.radius);

    vector2d const half_spacing = grid.spacing / 2;

    for (unsigned i = 0; i != grid.n_points[1]; ++i)
    {
        // If this grid row lies entirely outside the bounding box, skip it.
        // TODO: We could skip all rows before and after as single blocks.
        double y = grid.p0[1] - half_spacing[1] + grid.spacing[1] * i;
        if (y > circle.center[1] + circle.radius
            || y + grid.spacing[1] < circle.center[1] - circle.radius)
        {
            skip_indices(recorder, index, grid.n_points[0]);
            index += grid.n_points[0];
            grid_point += grid.n_points[0];
            continue;
        }

        auto row_start_index = index;

        int pixels_before = clamp<int>(
            int(std::floor(
                (circle.center[0] - circle.radius)
                - (grid.p0[0] - half_spacing[0]))),
            0,
            grid.n_points[0]);
        {
            skip_indices(recorder, index, pixels_before);
            index += pixels_before;
            grid_point += pixels_before;
        }

        int pixels_after = clamp<int>(
            int(std::floor(
                (grid.p0[0] - half_spacing[0]
                 + grid.n_points[0] * grid.spacing[0])
                - (circle.center[0] + circle.radius))),
            0,
            grid.n_points[0]);

        auto end_index = row_start_index + grid.n_points[0] - pixels_after;

#define IS_INSIDE_CIRCLE(p) (length2((p) - circle.center) < r_squared)

        for (; index < end_index; ++index, ++grid_point)
        {
            // If all four corners are inside, the whole cell is inside.
            if (IS_INSIDE_CIRCLE(*grid_point - half_spacing)
                && IS_INSIDE_CIRCLE(*grid_point + half_spacing)
                && IS_INSIDE_CIRCLE(
                    *grid_point
                    + make_vector(-half_spacing[0], half_spacing[1]))
                && IS_INSIDE_CIRCLE(
                    *grid_point
                    + make_vector(half_spacing[0], -half_spacing[1])))
            {
                record_index_value(recorder, index, 1);
                continue;
            }

            // Otherwise, sample a bunch of points in the grid cell and
            // calculate the fraction that are inside the circle.
            int const n_samples_along_axis = 10;
            vector2d start_p = *grid_point - half_spacing
                               + half_spacing / n_samples_along_axis;
            vector2d p_step = grid.spacing / n_samples_along_axis;
            int n_inside = 0;
            for (int i = 0; i != n_samples_along_axis; ++i)
            {
                vector2d p = start_p + make_vector(0., i * p_step[1]);
                for (int j = 0; j != n_samples_along_axis; ++j)
                {
                    if (IS_INSIDE_CIRCLE(p))
                        ++n_inside;
                    p[0] += p_step[0];
                }
            }

            record_index_value(
                recorder,
                index,
                float(n_inside) / square(n_samples_along_axis));
        }

        {
            skip_indices(recorder, index, pixels_after);
            index += pixels_after;
            grid_point += pixels_after;
        }

#undef IS_INSIDE_CIRCLE
    }
}

grid_cell_inclusion_info
compute_grid_cells_in_circle(
    regular_grid<2, double> const& grid, circle<double> const& circle)
{
    grid_cell_inclusion_info info;
    weighted_index_recorder recorder;
    recorder.indices = &info.cells_inside;
    compute_circle_inclusion_indices(recorder, 0, grid, circle);
    return info;
}

struct structure_inclusion_job : simple_job_interface
{
    structure_inclusion_job(
        boost::mutex* recorder_mutex,
        image<3, float, unique>& inclusion_image,
        regular_grid<3, double> const& grid,
        optional<structure_geometry_slice> const& slice,
        std::vector<unsigned> indices)
        : recorder_mutex_(recorder_mutex),
          inclusion_image_(&inclusion_image),
          grid_(&grid),
          slice_(slice),
          indices_(indices)
    {
    }

    void
    execute(
        check_in_interface& check_in, progress_reporter_interface& reporter)
    {
        if (indices_.empty())
        {
            return;
        }

        unsigned pixels_per_slice = grid_->n_points[0] * grid_->n_points[1];

        if (slice_)
        {
            image_pixel_recorder recorder;
            recorder.pixels = inclusion_image_->pixels.ptr
                              + indices_[0] * pixels_per_slice;
            compute_polyset_inclusion_indices(
                recorder, 0, slice(*grid_, 2), slice_->region, true);
            // Copy data to all other grid rows
            for (size_t ii = 1; ii < indices_.size(); ++ii)
            {
                std::copy_n(
                    inclusion_image_->pixels.ptr
                        + indices_[0] * pixels_per_slice,
                    pixels_per_slice,
                    inclusion_image_->pixels.ptr
                        + indices_[ii] * pixels_per_slice);
            }
        }
        else
        {
            for (size_t ii = 0; ii < indices_.size(); ++ii)
            {
                image_pixel_recorder recorder;
                recorder.pixels = inclusion_image_->pixels.ptr
                                  + indices_[ii] * pixels_per_slice;
                skip_indices(recorder, 0, pixels_per_slice);
            }
        }
    }

 private:
    boost::mutex* recorder_mutex_;
    image<3, float, unique>* inclusion_image_;
    regular_grid<3, double> const* grid_;
    optional<structure_geometry_slice> const slice_;
    std::vector<unsigned> const indices_;
};

image<3, float, shared>
compute_structure_inclusion_image(
    regular_grid<3, double> const& grid, structure_geometry const& structure)
{
    image<3, float, unique> inclusion_image;
    create_image_on_grid(inclusion_image, grid);

    boost::mutex recorder_mutex;
    std::vector<structure_inclusion_job> jobs;
    jobs.reserve(grid.n_points[2]);

    optional<structure_geometry_slice> batch_slice = none;
    std::vector<unsigned> batch_indices;
    for (unsigned i = 0; i != grid.n_points[2]; ++i)
    {
        double slice_position = grid.p0[2] + i * grid.spacing[2];
        auto slice = get_structure_slice(structure, slice_position);

        if (batch_indices.empty())
        {
            // Add this slice to batch to start the new batch, and move on
            batch_indices.push_back(i);
            batch_slice = slice;
            continue;
        }
        else if (!batch_slice)
        {
            if (slice)
            {
                // Batch does not match, submit job for active batch
                jobs.push_back(structure_inclusion_job(
                    &recorder_mutex,
                    inclusion_image,
                    grid,
                    batch_slice,
                    batch_indices));
                // Start new batch and move on
                batch_indices.clear();
                batch_slice = slice;
                batch_indices.push_back(i);
                continue;
            }
            else
            {
                // Another empty slice, add to batch and move one
                batch_indices.push_back(i);
                continue;
            }
        }
        else if (!slice)
        {
            // Batch does not match, submit job for active batch
            jobs.push_back(structure_inclusion_job(
                &recorder_mutex,
                inclusion_image,
                grid,
                batch_slice,
                batch_indices));
            // Start new batch and move on
            batch_indices.clear();
            batch_slice = slice;
            batch_indices.push_back(i);
            continue;
        }
        else if (batch_slice->position == slice->position)
        {
            // Matching slice, add to batch and move one
            batch_indices.push_back(i);
            continue;
        }
        else // Both slices existed, but are at different positions
        {
            // Batch does not match, submit job for active batch
            jobs.push_back(structure_inclusion_job(
                &recorder_mutex,
                inclusion_image,
                grid,
                batch_slice,
                batch_indices));
            // Start new batch and move on
            batch_indices.clear();
            batch_slice = slice;
            batch_indices.push_back(i);
            continue;
        }
    }
    // Submit the final batch
    if (!batch_indices.empty())
    {
        jobs.push_back(structure_inclusion_job(
            &recorder_mutex,
            inclusion_image,
            grid,
            batch_slice,
            batch_indices));
    }

    null_check_in check_in;
    null_progress_reporter reporter;
    execute_jobs_concurrently(check_in, reporter, jobs.size(), &jobs[0]);

    return share(inclusion_image);
}

struct voxel_inclusion_job : simple_job_interface
{
    voxel_inclusion_job(
        boost::mutex* recorder_mutex,
        box3d const& voxel_box,
        structure_geometry const& structure,
        box3d const& structure_bounds,
        std::vector<weighted_grid_index>& indices,
        unsigned ii)
        : recorder_mutex_(recorder_mutex),
          voxel_box_(&voxel_box),
          structure_(&structure),
          structure_bounds_(&structure_bounds),
          indices_(&indices),
          ii_(ii)
    {
    }

    void
    execute(
        check_in_interface& check_in, progress_reporter_interface& reporter)
    {
        float inclusion = 0;

        // Determine if structure is within the voxel

        // First check if bounding boxes overlap
        if (!overlapping(*voxel_box_, *structure_bounds_))
        {
            inclusion = 0;
        }
        else
        {
            char corners_inside = 0;
            // Loop over each of the 8 corners of the box
            for (int k = 0; k < 2; ++k)
            {
                double z = (*voxel_box_).corner[2]
                           + (*voxel_box_).size[2] * double(k);
                auto poly_on_slice = get_slice(*structure_, z);
                for (int i = 0; i < 2; ++i)
                {
                    double x = (*voxel_box_).corner[0]
                               + (*voxel_box_).size[0] * double(i);
                    for (int j = 0; j < 2; ++j)
                    {
                        // If corner is inside of structure, increment the
                        // count
                        double y = (*voxel_box_).corner[1]
                                   + (*voxel_box_).size[1] * double(j);
                        if (point_in_polyset(make_vector(x, y), poly_on_slice))
                        {
                            ++corners_inside;
                        }
                    }
                }
            }

            inclusion = float(corners_inside) / 8.0f;

            // If the voxel isn't completely in/out, compute the inclusion more
            // accurately by creating a 5x5x5 grid over the voxel
            if ((inclusion > 0.01) && (inclusion < 0.99))
            {
                short grid_points_inside = 0;
                unsigned res = 5;
                auto grid = make_grid_covering_box(
                    *voxel_box_, uniform_vector<3>(res));

                // Loop over grid points, tally up the number of points within
                // the structure. This is that voxel's inclusion fraction.
                auto z_points = get_points_on_grid(slice(slice(grid, 1), 0));
                auto points = get_points_on_grid(slice(grid, 2));
                optional<double> previous_slice_z = none;
                short points_on_slice = 0;
                for (auto z : z_points)
                {
                    auto slice = get_structure_slice(*structure_, z[0]);
                    if (!slice)
                    {
                        continue;
                    }
                    else if (
                        previous_slice_z
                        && (*previous_slice_z) == slice->position)
                    {
                        grid_points_inside += points_on_slice;
                        continue;
                    }
                    points_on_slice = 0;
                    previous_slice_z = slice->position;
                    for (auto point : points)
                    {
                        if (point_in_polyset(point, slice->region))
                        {
                            ++points_on_slice;
                        }
                    }
                    grid_points_inside += points_on_slice;
                }

                inclusion = float(grid_points_inside)
                            / float(product(grid.n_points));
            }
        }

        // This is scoping for the mutex
        {
            boost::mutex::scoped_lock lock(*recorder_mutex_);
            if (inclusion > 0.0)
            {
                // Scale by the volume of the box too
                float vol = float(product(voxel_box_->size));
                indices_->push_back(weighted_grid_index(ii_, vol * inclusion));
            }
        }
    }

 private:
    boost::mutex* recorder_mutex_;
    box3d const* voxel_box_;
    structure_geometry const* structure_;
    box3d const* structure_bounds_;
    std::vector<weighted_grid_index>* indices_;
    unsigned ii_;
};

grid_cell_inclusion_info static compute_voxels_in_structure(
    check_in_interface& check_in,
    progress_reporter_interface& progress_reporter,
    std::vector<box3d> const& voxels,
    structure_geometry const& structure)
{
    progress_reporter(0.f);

    box3d bounds = bounding_box(structure);

    // Output data
    std::vector<weighted_grid_index> indices;

    // We may want to consider limiting the number of jobs created by batching
    // voxels together in the future. -Kevin
    boost::mutex recorder_mutex;
    std::vector<voxel_inclusion_job> jobs;
    jobs.reserve(voxels.size());

    for (unsigned i = 0; i < boost::numeric_cast<unsigned>(voxels.size()); ++i)
    {
        jobs.push_back(voxel_inclusion_job(
            &recorder_mutex, voxels[i], structure, bounds, indices, i));
    }

    execute_jobs_concurrently(
        check_in, progress_reporter, jobs.size(), &jobs[0]);
    std::sort(
        indices.begin(),
        indices.end(),
        [](weighted_grid_index const& a, weighted_grid_index const& b) {
            return a.index < b.index;
        });

    return grid_cell_inclusion_info(indices);
}

grid_cell_inclusion_info
compute_adaptive_voxels_in_structure(
    check_in_interface& check_in,
    progress_reporter_interface& progress_reporter,
    adaptive_grid const& grid,
    structure_geometry const& structure)
{
    std::vector<box3d> boxes = adaptive_grid_voxel_boxes(grid);
    size_t voxel_count = grid.voxels.size();

    if (voxel_count != boxes.size())
    {
        throw astroid::exception(
            "Number of voxel boxes doesn't match number of voxels in adaptive "
            "grid.");
    }

    return compute_voxels_in_structure(
        check_in, progress_reporter, boxes, structure);
}

std::vector<vector3d>
compute_adaptive_points_in_structure(
    check_in_interface& check_in,
    progress_reporter_interface& progress_reporter,
    adaptive_grid const& grid,
    structure_geometry const& structure)
{
    std::vector<vector3d> result;

    for (auto const& voxel : grid.voxels)
    {
        auto box = get_octree_box(grid.extents, voxel.index);
        auto point = get_center(box);

        if (point_in_structure(point, structure))
        {
            result.push_back(point);
        }
    }

    return result;
}

namespace {

template<unsigned N>
std::vector<weighted_grid_index>
get_nonzero_pixel_indices(image<N, float, shared> const& img)
{
    std::vector<weighted_grid_index> indices;
    auto n_pixels = product(img.size);
    float const* pixels = img.pixels.view;
    for (unsigned i = 0; i != n_pixels; ++i)
    {
        if (pixels[i] != 0)
        {
            float weight = pixels[i];
            indices.push_back(weighted_grid_index(i, weight));
        }
    }
    return indices;
}

} // namespace

grid_cell_inclusion_info
compute_grid_cells_in_structure(
    regular_grid<3, double> const& grid, structure_geometry const& structure)
{
    auto img = compute_structure_inclusion_image(grid, structure);
    return grid_cell_inclusion_info(get_nonzero_pixel_indices(img));
}

// This was added as an alternative to the above function. It should be
// benchmarked for speed and accuracy before making a switch as it relies an
// approximation not made in the above version.
grid_cell_inclusion_info
compute_grid_cells_in_structure2(
    regular_grid<3, double> const& grid, structure_geometry const& structure)
{
    auto points = get_points_on_grid(grid);

    std::vector<box3d> boxes;
    boxes.reserve(points.size());
    auto half_spacing = 0.5 * grid.spacing;
    for (size_t i = 0; i < points.size(); ++i)
    {
        boxes.emplace_back(make_box(points[i] - half_spacing, grid.spacing));
    }

    null_check_in check;
    null_progress_reporter progress;

    return compute_voxels_in_structure(check, progress, boxes, structure);
}

std::vector<box3d>
get_inclusion_cell_boxes(
    grid_cell_inclusion_info const& voxel_inclusion, adaptive_grid const& grid)
{
    auto all_boxes = adaptive_grid_voxel_boxes(grid);
    std::vector<box3d> boxes;
    size_t voxel_count = voxel_inclusion.cells_inside.size();
    boxes.reserve(voxel_count);
    for (size_t i = 0; i < voxel_count; ++i)
    {
        assert(voxel_inclusion.cells_inside[i].weight > 0.0);
        boxes.push_back(all_boxes[voxel_inclusion.cells_inside[i].index]);
    }
    return boxes;
}

namespace impl {
struct image_override_fn
{
    double override_value;
    optional<float> cutoff_fraction;
    structure_geometry const& structure;
    bool set_inside_structure;
    image3 result;

    image_override_fn(structure_geometry const& s) : structure(s)
    {
    }

    template<class Pixel>
    void
    operator()(image<3, Pixel, shared> const& img)
    {
        image<3, Pixel, unique> tmp;
        create_image(tmp, img.size);
        copy_image(tmp, img);
        double cutoff = 0.5;
        if (cutoff_fraction)
            cutoff = clamp(double(*cutoff_fraction), 1.0e-5, 1.0 - 1.0e-5);
        auto grid = get_grid(tmp);
        image<3, float, shared> fractions
            = compute_structure_inclusion_image(grid, structure);
        auto fract_cv = as_const_view(fractions);
        Pixel value = Pixel(override_value);
        if (set_inside_structure)
        {
            for (unsigned i = 0; i < product(tmp.size); ++i)
            {
                if (fract_cv.pixels[i] >= cutoff)
                {
                    tmp.pixels.ptr[i] = value;
                }
            }
        }
        else
        {
            for (unsigned i = 0; i < product(tmp.size); ++i)
            {
                if (fract_cv.pixels[i] <= cutoff)
                {
                    tmp.pixels.ptr[i] = value;
                }
            }
        }
        result = as_variant(share(tmp));
    }
};
} // namespace impl

image<3, variant, shared>
override_image_from_structure(
    image<3, variant, shared> const& img,
    structure_geometry const& structure,
    double override_value,
    optional<float> cutoff_fraction,
    bool set_inside_structure)
{
    impl::image_override_fn fn(structure);
    fn.override_value = override_value;
    fn.cutoff_fraction = cutoff_fraction;
    fn.set_inside_structure = set_inside_structure;
    apply_fn_to_gray_variant(fn, img);
    return fn.result;
}

image3
override_image_variant_inside_structure(
    image3 const& img,
    structure_geometry const& structure,
    double override_value,
    float cutoff_fraction,
    string const& data_type)
{
    return override_image_from_structure(
        img, structure, override_value, cutoff_fraction, true);
}

image<3, double, shared>
override_image_inside_structure(
    image<3, double, shared> const& img,
    structure_geometry const& structure,
    double override_value,
    float cutoff_fraction)
{
    return cast_variant<double>(override_image_from_structure(
        as_variant(img), structure, override_value, cutoff_fraction, true));
}

image<3, double, shared>
override_image_outside_structure(
    image<3, double, shared> const& img,
    structure_geometry const& structure,
    double override_value,
    float cutoff_fraction)
{
    return cast_variant<double>(override_image_from_structure(
        as_variant(img), structure, override_value, cutoff_fraction, false));
}

image<3, variant, shared>
override_image_variant_outside_structure(
    image<3, variant, shared> const& img,
    structure_geometry const& structure,
    double override_value,
    float cutoff_fraction)
{
    return override_image_from_structure(
        img, structure, override_value, cutoff_fraction, false);
}

} // namespace astroid
