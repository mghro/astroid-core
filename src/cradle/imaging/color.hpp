#ifndef CRADLE_IMAGING_COLOR_HPP
#define CRADLE_IMAGING_COLOR_HPP

#include <cradle/imaging/channel.hpp>
#include <cradle/math/common.hpp>
#include <cradle/math/interpolate.hpp>

namespace cradle {

// RGB

// RGB color/pixel
template<class Channel>
struct rgb
{
    rgb()
    {
    }
    rgb(Channel r, Channel g, Channel b) : r(r), g(g), b(b)
    {
    }
    Channel r, g, b;
};
typedef rgb<uint8_t> rgb8;

static inline bool
operator==(rgb8 const& a, rgb8 const& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
static inline bool
operator!=(rgb8 const& a, rgb8 const& b)
{
    return !(a == b);
}
bool
operator<(rgb8 const& a, rgb8 const& b);

std::ostream&
operator<<(std::ostream& s, rgb8 const& c);

} // namespace cradle

namespace std {
template<class Channel>
struct hash<cradle::rgb<Channel>>
{
    size_t
    operator()(cradle::rgb<Channel> const& x) const
    {
        return cradle::combine_hashes(
            cradle::combine_hashes(hash<Channel>()(x.r), hash<Channel>()(x.g)),
            hash<Channel>()(x.b));
    }
};
} // namespace std

namespace cradle {

static inline uint8_t
multiply_uint8_channels(uint8_t a, uint8_t b)
{
    return uint8_t(unsigned(a) * b / 0xff);
}

// interpolate(a, b, factor) = a * (1 - factor) + b * factor
rgb8
interpolate(rgb8 const& a, rgb8 const& b, double factor);

// standard color names
static rgb8 const white(0xff, 0xff, 0xff), silver(0xc0, 0xc0, 0xc0),
    gray(0x80, 0x80, 0x80), black(0x00, 0x00, 0x00), red(0xff, 0x00, 0x00),
    maroon(0x80, 0x00, 0x00), yellow(0xff, 0xff, 0x00),
    olive(0x80, 0x80, 0x00), lime(0x00, 0xff, 0x00), green(0x00, 0x80, 0x00),
    aqua(0x00, 0xff, 0xff), teal(0x00, 0x80, 0x80), blue(0x00, 0x00, 0xff),
    navy(0x00, 0x00, 0x80), fuchsia(0xff, 0x00, 0xff),
    purple(0x80, 0x00, 0x80);

static inline uint8_t max_channel_value(uint8_t)
{
    return 0xff;
}

// RGBA color/pixel, w/ premultiplied alpha
template<class Channel>
struct rgba
{
    rgba()
    {
    }
    rgba(Channel r, Channel g, Channel b, Channel a) : r(r), g(g), b(b), a(a)
    {
    }
    rgba(rgb<Channel> color)
        : r(color.r), g(color.g), b(color.b), a(max_channel_value(Channel()))
    {
    }
    Channel r, g, b, a;
};
typedef rgba<uint8_t> rgba8;

static inline bool
operator==(rgba8 const& a, rgba8 const& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
static inline bool
operator!=(rgba8 const& a, rgba8 const& b)
{
    return !(a == b);
}
bool
operator<(rgba8 const& a, rgba8 const& b);

std::ostream&
operator<<(std::ostream& s, rgba8 const& c);

} // namespace cradle

namespace std {
template<class Channel>
struct hash<cradle::rgba<Channel>>
{
    size_t
    operator()(cradle::rgba<Channel> const& x) const
    {
        return cradle::combine_hashes(
            cradle::combine_hashes(hash<Channel>()(x.r), hash<Channel>()(x.g)),
            cradle::combine_hashes(
                hash<Channel>()(x.b), hash<Channel>()(x.a)));
    }
};
} // namespace std

namespace cradle {

// premultiply the color by the alpha to form an rgba8 value
rgba8
apply_alpha(rgb8 color, uint8_t alpha);
rgba8
apply_alpha(rgba8 color, uint8_t alpha);

// interpolate(a, b, factor) = a * (1 - factor) + b * factor
rgba8
interpolate(rgba8 const& a, rgba8 const& b, double factor);

typedef rgb<uint16_t> rgb16;

cradle::raw_type_info
get_type_info(rgb8 const& x);
cradle::raw_type_info
get_proper_type_info(rgb8 const& x);
template<class T>
void
to_value(cradle::value* v, rgb<T> const& x)
{
    cradle::value_map r;
    to_value(&r[cradle::value("r")], x.r);
    to_value(&r[cradle::value("g")], x.g);
    to_value(&r[cradle::value("b")], x.b);
    set(*v, r);
}
template<class T>
void
from_value(rgb<T>* x, cradle::value const& v)
{
    cradle::value_map const& r = cradle::cast<cradle::value_map>(v);
    from_value(&x->r, get_field(r, "r"));
    from_value(&x->g, get_field(r, "g"));
    from_value(&x->b, get_field(r, "b"));
}
template<class T>
size_t deep_sizeof(rgb<T>)
{
    using cradle::deep_sizeof;
    return deep_sizeof(T()) * 3;
}
// comparison
template<class T>
bool
operator==(rgb<T> const& a, rgb<T> const& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
template<class T>
bool
operator!=(rgb<T> const& a, rgb<T> const& b)
{
    return !(a == b);
}
template<class T>
bool
operator<(rgb<T> const& a, rgb<T> const& b)
{
    if (a.r < b.r)
        return true;
    if (b.r < a.r)
        return false;
    if (a.g < b.g)
        return true;
    if (b.g < a.g)
        return false;
    if (a.b < b.b)
        return true;
    if (b.b < a.b)
        return false;
    return false;
}
// interpolation, scaling
template<class T, class F>
rgb<T>
interpolate(rgb<T> const& a, rgb<T> const& b, F f)
{
    return rgb<T>(
        cradle::interpolate(a.r, b.r, f),
        cradle::interpolate(a.g, b.g, f),
        cradle::interpolate(a.b, b.b, f));
}
template<class T, class F>
rgb<T>
scale(rgb<T> const& a, F f)
{
    return interpolate(rgb<T>(0, 0, 0), a, f);
}
// componentwise +
template<class T>
rgb<T>
operator+(rgb<T> const& a, rgb<T> const& b)
{
    return rgb<T>(a.r + b.r, a.g + b.g, a.b + b.b);
}
template<class T>
rgb<T>&
operator+=(rgb<T>& a, rgb<T> const& b)
{
    a.r += b.r;
    a.g += b.g;
    a.b += b.b;
    return a;
}
// componentwise -
template<class T>
rgb<T>
operator-(rgb<T> const& a, rgb<T> const& b)
{
    return rgb<T>(a.r - b.r, a.g - b.g, a.b - b.b);
}
template<class T>
rgb<T>&
operator-=(rgb<T>& a, rgb<T> const& b)
{
    a.r -= b.r;
    a.g -= b.g;
    a.b -= b.b;
    return a;
}
template<class MappingT, class T>
rgb<MappingT>
apply_linear_function(
    cradle::linear_function<MappingT> const& mapping, rgb<T> const& p)
{
    return rgb<MappingT>(mapping(p.r), mapping(p.g), mapping(p.b));
}
template<class T, class Value>
void
fill_channels(rgb<T>& p, Value v)
{
    p.r = v;
    p.g = v;
    p.b = v;
}

} // namespace cradle

namespace cradle {

// metafunctions, etc
template<class T>
struct pixel_channel_type<rgb<T>>
{
    typedef T type;
};
template<class T, class NewChannelT>
struct replace_channel_type<rgb<T>, NewChannelT>
{
    typedef rgb<NewChannelT> type;
};
template<class DstT, class SrcT>
struct channel_converter<rgb<DstT>, rgb<SrcT>>
{
    static rgb<DstT>
    apply(rgb<SrcT> const& src)
    {
        rgb<DstT> dst;
        dst.r = channel_convert<DstT>(src.r);
        dst.g = channel_convert<DstT>(src.g);
        dst.b = channel_convert<DstT>(src.b);
        return dst;
    }
};

// RGBA

// Given a color palette and a list of colors that are already in use, select
// a new color to add to the group that will stand out.
rgb8
choose_new_color(
    std::vector<rgb8> const& palette, std::vector<rgb8> const& already_in_use);

} // namespace cradle

namespace cradle {

cradle::raw_type_info
get_type_info(rgba8 const& x);
cradle::raw_type_info
get_proper_type_info(rgba8 const& x);
template<class T>
void
to_value(cradle::value* v, rgba<T> const& x)
{
    cradle::value_map r;
    to_value(&r[cradle::value("r")], x.r);
    to_value(&r[cradle::value("g")], x.g);
    to_value(&r[cradle::value("b")], x.b);
    to_value(&r[cradle::value("a")], x.a);
    set(*v, r);
}
template<class T>
void
from_value(rgba<T>* x, cradle::value const& v)
{
    cradle::value_map const& r = cradle::cast<cradle::value_map>(v);
    from_value(&x->r, get_field(r, "r"));
    from_value(&x->g, get_field(r, "g"));
    from_value(&x->b, get_field(r, "b"));
    from_value(&x->a, get_field(r, "a"));
}
template<class T>
size_t deep_sizeof(rgba<T>)
{
    using cradle::deep_sizeof;
    return deep_sizeof(T()) * 4;
}
// comparison
template<class T>
bool
operator==(rgba<T> const& a, rgba<T> const& b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
template<class T>
bool
operator!=(rgba<T> const& a, rgba<T> const& b)
{
    return !(a == b);
}
template<class T>
bool
operator<(rgba<T> const& a, rgba<T> const& b)
{
    if (a.r < b.r)
        return true;
    if (b.r < a.r)
        return false;
    if (a.g < b.g)
        return true;
    if (b.g < a.g)
        return false;
    if (a.b < b.b)
        return true;
    if (b.b < a.b)
        return false;
    if (a.a < b.a)
        return true;
    if (b.a < a.a)
        return false;
    return false;
}
// interpolation, scaling
template<class T, class F>
rgba<T>
interpolate(rgba<T> const& a, rgba<T> const& b, F f)
{
    return rgba<T>(
        cradle::interpolate(a.r, b.r, f),
        cradle::interpolate(a.g, b.g, f),
        cradle::interpolate(a.b, b.b, f),
        cradle::interpolate(a.a, b.a, f));
}
template<class T, class F>
rgba<T>
scale(rgba<T> const& a, F f)
{
    return interpolate(rgba<T>(0, 0, 0, a.a), a, f);
}
// componentwise +
template<class T>
rgba<T>
operator+(rgba<T> const& a, rgba<T> const& b)
{
    return rgba<T>(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}
template<class T>
rgba<T>&
operator+=(rgba<T>& a, rgba<T> const& b)
{
    a.r += b.r;
    a.g += b.g;
    a.b += b.b;
    a.a += b.a;
    return a;
}
// componentwise -
template<class T>
rgba<T>
operator-(rgba<T> const& a, rgba<T> const& b)
{
    return rgba<T>(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}
template<class T>
rgba<T>&
operator-=(rgba<T>& a, rgba<T> const& b)
{
    a.r -= b.r;
    a.g -= b.g;
    a.b -= b.b;
    a.a -= b.a;
    return a;
}
template<class MappingT, class T>
rgba<MappingT>
apply_linear_function(
    cradle::linear_function<MappingT> const& mapping, rgba<T> const& p)
{
    return rgba<MappingT>(
        mapping(p.r), mapping(p.g), mapping(p.b), mapping(p.a));
}
template<class T, class Value>
void
fill_channels(rgba<T>& p, Value v)
{
    p.r = v;
    p.g = v;
    p.b = v;
    p.a = v;
}

} // namespace cradle

namespace cradle {

// metafunctions, etc
template<class T>
struct pixel_channel_type<rgba<T>>
{
    typedef T type;
};
template<class T, class NewChannelT>
struct replace_channel_type<rgba<T>, NewChannelT>
{
    typedef rgba<NewChannelT> type;
};
template<class DstT, class SrcT>
struct channel_converter<rgba<DstT>, rgba<SrcT>>
{
    static rgba<DstT>
    apply(rgba<SrcT> const& src)
    {
        rgba<DstT> dst;
        dst.r = channel_convert<DstT>(src.r);
        dst.g = channel_convert<DstT>(src.g);
        dst.b = channel_convert<DstT>(src.b);
        dst.a = channel_convert<DstT>(src.a);
        return dst;
    }
};

typedef rgba<uint16_t> rgba16;

} // namespace cradle

#endif
