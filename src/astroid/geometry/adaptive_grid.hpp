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

#ifndef ASTROID_GEOMETRY_ADAPTIVE_GRID_HPP
#define ASTROID_GEOMETRY_ADAPTIVE_GRID_HPP

#include <astroid/common.hpp>
#include <astroid/geometry/common.hpp>
#include <astroid/geometry/meshing.hpp>
#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/variant.hpp>
#include <vector>

namespace astroid {

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
    astroid::array<adaptive_grid_voxel> voxels;
    // List of region indices that voxels interact with (us)
    astroid::array<uint16_t> volumes;
};

box3d
get_octree_box(box3d const& extents, uint64_t index);

int
get_octree_depth(box3d const& extents, uint64_t index);

} // namespace astroid

#endif
