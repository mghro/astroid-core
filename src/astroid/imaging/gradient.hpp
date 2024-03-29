#ifndef ASTROID_IMAGING_GRADIENT_HPP
#define ASTROID_IMAGING_GRADIENT_HPP

#include <astroid/math/interpolate.hpp>
#include <boost/range/functions.hpp>

namespace astroid {

template<class Color>
void
compute_gradient(
    std::vector<Color>& gradient,
    std::vector<Color> const& colors,
    std::vector<double> const& positions)
{
    assert(positions.size() == colors.size());
    regular_grid<1, double> grid(0, 1, unsigned(gradient.size()));
    interpolate(&gradient, grid, colors, positions);
}

template<class Color>
void
compute_gradient(
    std::vector<Color>& gradient,
    std::vector<Color> const& colors,
    std::vector<double> const& positions,
    linear_function<double> const& scale)
{
    assert(positions.size() == colors.size());
    regular_grid<1, double> grid(
        scale.intercept, scale.slope, unsigned(gradient.size()));
    interpolate(&gradient, grid, colors, positions);
}

} // namespace astroid

#endif
