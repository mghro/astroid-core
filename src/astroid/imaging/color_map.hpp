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

template<unsigned N, class Pixel, class Storage, class Color>
image<N, Color, shared>
apply_raw_color_map(
    image<N, Pixel, Storage> const& src,
    std::vector<color_map_level<Color>> const& map);

api(fun with(N : 2, 3; Pixel
             : astroid::variant;
             Storage
             : astroid::shared;
             Color
             : astroid::rgba8))
template<unsigned N, class Pixel, class Storage, class Color>
image<N, Color, shared>
apply_color_map(
    image<N, Pixel, Storage> const& src,
    std::vector<color_map_level<Color>> const& map);

} // namespace astroid

#include <astroid/imaging/color_map.ipp>

#endif
