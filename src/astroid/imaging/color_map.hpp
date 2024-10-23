#ifndef ASTROID_IMAGING_COLOR_MAP_HPP
#define ASTROID_IMAGING_COLOR_MAP_HPP

#include <astroid/imaging/variant.hpp>

namespace astroid {

api(struct with(Color : astroid::rgba8))
template<class Color>
struct color_map_level
{
    double level;
    Color color;
};

template<class Color>
using color_map = std::vector<color_map_level<astroid::rgba8>>;

using rgba8_color_map = color_map<astroid::rgba8>;

ASTROID_DEFINE_NORMALIZATION_UUID(astroid, rgba8_color_map)

template<unsigned N, class Pixel, class Storage, class Color>
image<N, Color, shared>
apply_raw_color_map(
    image<N, Pixel, Storage> const& src, color_map<Color> const& map);

template<unsigned N, class Pixel, class Storage, class Color>
image<N, Color, shared>
apply_color_map(
    image<N, Pixel, Storage> const& src, color_map<Color> const& map);

} // namespace astroid

#include <astroid/imaging/color_map.ipp>

#endif
