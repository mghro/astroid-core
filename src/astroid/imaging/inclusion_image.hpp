#ifndef ASTROID_IMAGING_INCLUSION_IMAGE_HPP
#define ASTROID_IMAGING_INCLUSION_IMAGE_HPP

#include <astroid/common.hpp>
#include <astroid/geometry/adaptive_grid.hpp>
#include <astroid/geometry/polygonal.hpp>
#include <astroid/geometry/regular_grid.hpp>
#include <astroid/imaging/image.hpp>
#include <astroid/imaging/variant.hpp>

#include <vector>

namespace astroid {

// Given a polyset, this returns a 2D image where the value of each pixel is
// equal to the fraction of the pixel that's within the polyset.
api(fun disk_cached)
// The 2D image in which the pixel values correspond to the fraction of the
// pixel that's within the polyset.
image<2, float, shared>
compute_polyset_inclusion_image(
    // The grid whose cells may be located inside the polyset.
    regular_grid<2, double> const& grid,
    // the polyset which may overlap with the grid cells.
    polyset const& set);

// grid_cell_inclusion_info stores information about what grid cells are
// included in a shape.
api(struct)
struct grid_cell_inclusion_info
{
    // List of grid cells at least partially inside the shape.
    std::vector<weighted_grid_index> cells_inside;
};

// Get the list of grid cells included in a polyset.
api(fun disk_cached)
// The list of grid cells included in the given polyset.
grid_cell_inclusion_info
compute_grid_cells_in_polyset(
    // The grid whose cells may be included in the polyset.
    regular_grid<2, double> const& grid,
    // The polyset which may overlap with the grid cells.
    polyset const& set);

// Get the list of grid cells included in a circle.
api(fun disk_cached)
// The list of grid cells included in the given circle.
grid_cell_inclusion_info
compute_grid_cells_in_circle(
    // The grid whose cells may be included in the circle.
    regular_grid<2, double> const& grid,
    // The circle which may overlap with the grid cells.
    astroid::circle<double> const& circle);

// Given a structure_geometry, this returns a 3D image where the value of each
// voxel is equal to the fraction of the voxel that's within the structure.
api(fun disk_cached execution_class(cpu.x8))
// The 3D image in which the voxel values correspond to the fraction of the
// voxel that's within the structure.
image<3, float, shared>
compute_structure_inclusion_image(
    // The grid whose cells may be located inside the structure.
    regular_grid<3, double> const& grid,
    // The structure which may overlap with the grid cells.
    structure_geometry const& structure);

// Given a structure_geometry, this returns a vector where the value of each
// voxel is equal to the fraction of the voxel that's within the structure.
api(fun disk_cached monitored execution_class(cpu.x8))
// Inclusion fractions of the adaptive grid voxels.
grid_cell_inclusion_info
compute_adaptive_voxels_in_structure(
    // Adaptive grid to check inclusion of against structure.
    adaptive_grid const& grid,
    // Structure to check adaptive grid voxels against.
    structure_geometry const& structure);

// Returns a list of vector3d points that correspond to the adaptive grid
// points inside the structure.
api(fun remote monitored execution_class(cpu.x4))
// The vector of vector3d points from the adaptive grid that are within the
// structure.
std::vector<vector3d>
compute_adaptive_points_in_structure(
    // Adaptive grid from which the points are derived from.
    adaptive_grid const& grid,
    // The structure used to check point inclusion.
    structure_geometry const& structure);

// Get the list of grid cells included in a structure.
api(fun disk_cached execution_class(cpu.x16))
// The list of grid cells included in the given structure.
grid_cell_inclusion_info
compute_grid_cells_in_structure(
    // The grid whose cells may be included in the structure.
    regular_grid<3, double> const& grid,
    // The structure which may overlap with the grid cells.
    structure_geometry const& structure);

// Gets a list of boxes from a grid_cell_inclusion_info.
/// api(fun disk_cached)
api(fun execution_class(cpu.x16))
// Box list of all voxels
std::vector<box3d>
get_inclusion_cell_boxes(
    // Inclusion fractions of the adaptive grid voxels.
    grid_cell_inclusion_info const& voxel_inclusion,
    // Adaptive grid
    adaptive_grid const& grid);

// Given a structure_geometry and 3D image (stopping power image), this
// returns a new 3D image where the value of each voxel, that is more than
// cutoff % contained within the structure, is set to the provided override
// value.
api(fun disk_cached execution_class(cpu.x8))
// 3D image of voxels set to the override value.
image<3, double, shared>
override_image_inside_structure(
    // 3d Stopping power image to check.
    image<3, double, shared> const& img,
    // Structure to compare against image.
    structure_geometry const& structure,
    // New value of pixel point inside the structure.
    double override_value,
    // Percentage of inclusion (x,y only) of the image pixel within the
    // structure.
    float cutoff_fraction);

// Given a structure_geometry and 3D image (stopping power image), this
// returns a new 3D image where the value of each voxel, that is more than
// cutoff % contained within the structure, is set to the provided override
// value.
api(fun disk_cached execution_class(cpu.x8))
// 3D image of voxels set to the override value.
image3
override_image_variant_inside_structure(
    // 3d Stopping power image to check.
    image3 const& img,
    // Structure to compare against image.
    structure_geometry const& structure,
    // New value of pixel point inside the structure.
    double override_value,
    // Percentage of inclusion (x,y only) of the image pixel within the
    // structure.
    float cutoff_fraction,
    // String indicating the data type of the image pixels
    string const& data_type);

// Given a structure_geometry and 3D image (stopping power image), this
// returns a new 3D image where the value of each voxel, that is more than
// cutoff % outside the structure, is set to the provided override value.
api(fun disk_cached execution_class(cpu.x8))
// 3D image of voxels set to the override value.
image<3, double, shared>
override_image_outside_structure(
    // 3d Stopping power image to check.
    image<3, double, shared> const& img,
    // Structure to compare against image.
    structure_geometry const& structure,
    // New value of pixel point inside the structure.
    double override_value,
    // Percentage of inclusion (x,y only) of the image pixel within the
    // structure.
    float cutoff_fraction);

// Given a structure_geometry and 3D image (stopping power image), this
// returns a new 3D image where the value of each voxel, that is more than
// cutoff % outside the structure, is set to the provided override value.
api(fun remote execution_class(cpu.x8) level(2))
// 3D image of voxels set to the override value.
image<3, variant, shared>
override_image_variant_outside_structure(
    // 3d Stopping power image to check
    image<3, variant, shared> const& img,
    // Structure to compare against image.
    structure_geometry const& structure,
    // New value of pixel point inside the structure.
    double override_value,
    // Percentage of inclusion (x,y only) of the image pixel within the
    // structure.
    float cutoff_fraction);

} // namespace astroid

#endif
