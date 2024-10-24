#ifndef ASTROID_GEOMETRY_FORWARD_HPP
#define ASTROID_GEOMETRY_FORWARD_HPP

#include <vector>

// Many geometry objects are templated over dimensionality and scalar type.
// This macro will define the most common typedefs for such a type, using the
// standard ASTROID naming conventions...
#define ASTROID_GEOMETRY_DEFINE_TYPEDEFS(type)                                \
    typedef type<0, int> type##0i;                                            \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0i);                     \
    typedef type<1, int> type##1i;                                            \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1i);                     \
    typedef type<2, int> type##2i;                                            \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2i);                     \
    typedef type<3, int> type##3i;                                            \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3i);                     \
    typedef type<4, int> type##4i;                                            \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4i);                     \
    typedef type<0, unsigned> type##0u;                                       \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0u);                     \
    typedef type<1, unsigned> type##1u;                                       \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1u);                     \
    typedef type<2, unsigned> type##2u;                                       \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2u);                     \
    typedef type<3, unsigned> type##3u;                                       \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3u);                     \
    typedef type<4, unsigned> type##4u;                                       \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4u);                     \
    typedef type<0, float> type##0f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0f);                     \
    typedef type<1, float> type##1f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1f);                     \
    typedef type<2, float> type##2f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2f);                     \
    typedef type<3, float> type##3f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3f);                     \
    typedef type<4, float> type##4f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4f);                     \
    typedef type<0, double> type##0d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0d);                     \
    typedef type<1, double> type##1d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1d);                     \
    typedef type<2, double> type##2d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2d);                     \
    typedef type<3, double> type##3d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3d);                     \
    typedef type<4, double> type##4d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4d);
// Same, but only floating point types.
#define ASTROID_GEOMETRY_DEFINE_FLOATING_TYPEDEFS(type)                       \
    typedef type<0, float> type##0f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0f);                     \
    typedef type<1, float> type##1f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1f);                     \
    typedef type<2, float> type##2f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2f);                     \
    typedef type<3, float> type##3f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3f);                     \
    typedef type<4, float> type##4f;                                          \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4f);                     \
    typedef type<0, double> type##0d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##0d);                     \
    typedef type<1, double> type##1d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##1d);                     \
    typedef type<2, double> type##2d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##2d);                     \
    typedef type<3, double> type##3d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##3d);                     \
    typedef type<4, double> type##4d;                                         \
    ASTROID_DEFINE_NORMALIZATION_UUID(astroid, type##4d);

namespace astroid {

template<unsigned N, class T>
struct vector;
ASTROID_GEOMETRY_DEFINE_TYPEDEFS(vector)

template<unsigned N, class T>
struct box;
ASTROID_GEOMETRY_DEFINE_TYPEDEFS(box)

// adaptive_grid.hpp
struct adaptive_grid;

// meshing.hpp
struct optimized_triangle_mesh;
struct triangle_mesh;
struct triangle_mesh_with_normals;

// polygonal.hpp
struct polygon2;
struct polygon_with_holes;
struct polyset;
struct structure_geometry;

// regular_grid.hpp
template<unsigned N, class T>
struct regular_grid;

// scenes.hpp
template<unsigned N>
struct sliced_scene_geometry;

// slicing.hpp
struct slice_description;

} // namespace astroid

#endif
