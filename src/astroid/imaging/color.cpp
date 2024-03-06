#include <astroid/imaging/color.hpp>

#include <iomanip>

namespace astroid {

rgb8
interpolate(rgb8 const& a, rgb8 const& b, double f)
{
    rgb8 r;
    r.r = uint8_t(a.r * (1 - f) + b.r * f + 0.5);
    r.g = uint8_t(a.g * (1 - f) + b.g * f + 0.5);
    r.b = uint8_t(a.b * (1 - f) + b.b * f + 0.5);
    return r;
}

rgba8
apply_alpha(rgb8 color, uint8_t alpha)
{
    return rgba8(
        multiply_uint8_channels(color.r, alpha),
        multiply_uint8_channels(color.g, alpha),
        multiply_uint8_channels(color.b, alpha),
        alpha);
}

rgba8
apply_alpha(rgba8 color, uint8_t alpha)
{
    return rgba8(
        multiply_uint8_channels(color.r, alpha),
        multiply_uint8_channels(color.g, alpha),
        multiply_uint8_channels(color.b, alpha),
        multiply_uint8_channels(color.a, alpha));
}

rgba8
interpolate(rgba8 const& a, rgba8 const& b, double f)
{
    rgba8 r;
    r.r = uint8_t(a.r * (1 - f) + b.r * f + 0.5);
    r.g = uint8_t(a.g * (1 - f) + b.g * f + 0.5);
    r.b = uint8_t(a.b * (1 - f) + b.b * f + 0.5);
    r.a = uint8_t(a.a * (1 - f) + b.a * f + 0.5);
    return r;
}

std::ostream&
operator<<(std::ostream& s, rgb8 const& c)
{
    return s << "#" << std::hex << std::setw(2) << std::setfill('0')
             << int(c.r) << int(c.g) << int(c.b);
}
std::ostream&
operator<<(std::ostream& s, rgba8 const& c)
{
    return s << "#" << std::hex << std::setw(2) << std::setfill('0')
             << int(c.r) << int(c.g) << int(c.b) << int(c.a);
}

rgb8
choose_new_color(
    std::vector<rgb8> const& palette, std::vector<rgb8> const& already_in_use)
{
    // This just tries to maximize the minimum RGB distance from the other
    // colors.
    // TODO: Handle the case where the palette is entirely used up.
    rgb8 best_color;
    double best_distance = -1;
    for (auto const& p : palette)
    {
        auto p_distance = std::numeric_limits<double>::max();
        for (auto const& a : already_in_use)
        {
            auto distance = length2(
                make_vector<double>(p.r, p.g, p.b)
                - make_vector<double>(a.r, a.g, a.b));
            if (distance < p_distance)
                p_distance = distance;
        }
        if (p_distance > best_distance)
        {
            best_color = p;
            best_distance = p_distance;
        }
    }
    return best_color;
}

} // namespace astroid
