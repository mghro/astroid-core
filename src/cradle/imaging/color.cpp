#include <cradle/imaging/color.hpp>

#include <iomanip>

namespace cradle {

bool
operator<(rgb8 const& a, rgb8 const& b)
{
    if (a.r < b.r)
        return true;
    if (a.r > b.r)
        return false;
    if (a.g < b.g)
        return true;
    if (a.g > b.g)
        return false;
    if (a.b < b.b)
        return true;
    return false;
}
bool
operator<(rgba8 const& a, rgba8 const& b)
{
    if (a.r < b.r)
        return true;
    if (a.r > b.r)
        return false;
    if (a.g < b.g)
        return true;
    if (a.g > b.g)
        return false;
    if (a.b < b.b)
        return true;
    if (a.b > b.b)
        return false;
    if (a.a < b.a)
        return true;
    return false;
}

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

cradle::raw_type_info
get_type_info(rgb8 const& x)
{
    return cradle::raw_type_info(
        cradle::raw_kind::NAMED_TYPE_REFERENCE,
        cradle::any(cradle::raw_named_type_reference(
            THINKNODE_TYPES_APP, string("rgb8"))));
}
cradle::raw_type_info
get_proper_type_info(rgb8 const& x)
{
    using cradle::get_type_info;
    std::vector<cradle::raw_structure_field_info> fields;
    fields.push_back(cradle::raw_structure_field_info(
        "r", "Red", get_type_info(uint8_t())));
    fields.push_back(cradle::raw_structure_field_info(
        "g", "Green", get_type_info(uint8_t())));
    fields.push_back(cradle::raw_structure_field_info(
        "b", "Blue", get_type_info(uint8_t())));
    return cradle::raw_type_info(
        cradle::raw_kind::STRUCTURE,
        cradle::any(
            cradle::raw_structure_info("rgb8", "RGB triplet", fields)));
}

cradle::raw_type_info
get_type_info(rgba8 const& x)
{
    return cradle::raw_type_info(
        cradle::raw_kind::NAMED_TYPE_REFERENCE,
        cradle::any(cradle::raw_named_type_reference(
            THINKNODE_TYPES_APP, string("rgba8"))));
}
cradle::raw_type_info
get_proper_type_info(rgba8 const& x)
{
    using cradle::get_type_info;
    std::vector<cradle::raw_structure_field_info> fields;
    fields.push_back(cradle::raw_structure_field_info(
        "r", "Red", get_type_info(uint8_t())));
    fields.push_back(cradle::raw_structure_field_info(
        "g", "Green", get_type_info(uint8_t())));
    fields.push_back(cradle::raw_structure_field_info(
        "b", "Blue", get_type_info(uint8_t())));
    fields.push_back(cradle::raw_structure_field_info(
        "a", "Alpha", get_type_info(uint8_t())));
    return cradle::raw_type_info(
        cradle::raw_kind::STRUCTURE,
        cradle::any(
            cradle::raw_structure_info("rgba8", "RGB triplet", fields)));
}

} // namespace cradle

namespace cradle {

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

} // namespace cradle
