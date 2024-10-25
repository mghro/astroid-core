#ifndef ASTROID_IMAGING_VALUE_TRANSFORMS_HPP
#define ASTROID_IMAGING_VALUE_TRANSFORMS_HPP

#include <astroid/imaging/variant.hpp>

namespace astroid {

// api(fun)
image3
transform_image_values_3d(
    image3 const& image,
    interpolated_function const& transform,
    units const& transformed_units);

} // namespace astroid

#endif
