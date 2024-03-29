#ifndef ASTROID_GEOMETRY_DECODE_MATRIX_HPP
#define ASTROID_GEOMETRY_DECODE_MATRIX_HPP

#include <astroid/geometry/angle.hpp>
#include <astroid/geometry/common.hpp>
#include <boost/optional/optional.hpp>

// This file provides various functions for decoding transformation matrices.

namespace astroid {

// Determine if the given transformation matrix has a rotational component.
template<unsigned N, typename T>
bool
has_rotation(matrix<N, N, T> const& m);

// Given a matrix that represents a 3D rotation about the X-axis (and only
// that), this returns the angle of the rotation.  For any other matrix,
// it returns an uninitialized value.
template<typename T>
optional<angle<T, radians>>
decode_rotation_about_x(matrix<3, 3, T> const& m);

// Decode a rotation about the Y-axis.
template<typename T>
optional<angle<T, radians>>
decode_rotation_about_y(matrix<3, 3, T> const& m);

// Decode a rotation about the Z-axis.
template<typename T>
optional<angle<T, radians>>
decode_rotation_about_z(matrix<3, 3, T> const& m);

} // namespace astroid

#include <astroid/geometry/decode_matrix.ipp>

#endif
