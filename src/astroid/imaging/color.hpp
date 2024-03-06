#ifndef ASTROID_IMAGING_COLOR_HPP
#define ASTROID_IMAGING_COLOR_HPP

#include <astroid/common.hpp>
#include <astroid/imaging/channel.hpp>
#include <astroid/math/common.hpp>
#include <astroid/math/interpolate.hpp>

namespace astroid {

// RGB

// RGB color/pixel
api(struct with(Channel : uint8_t))
template<class Channel>
struct rgb
{
    Channel r, g, b;
};

using rgb8 = rgb<uint8_t>;

std::ostream&
operator<<(std::ostream& s, rgb8 const& c);

static inline uint8_t
multiply_uint8_channels(uint8_t a, uint8_t b)
{
    return uint8_t(unsigned(a) * b / 0xff);
}

// interpolate(a, b, factor) = a * (1 - factor) + b * factor
rgb8
interpolate(rgb8 const& a, rgb8 const& b, double factor);

namespace colors {

// standard color names
static rgb8 const white(0xff, 0xff, 0xff), silver(0xc0, 0xc0, 0xc0),
    gray(0x80, 0x80, 0x80), black(0x00, 0x00, 0x00), red(0xff, 0x00, 0x00),
    maroon(0x80, 0x00, 0x00), yellow(0xff, 0xff, 0x00),
    olive(0x80, 0x80, 0x00), lime(0x00, 0xff, 0x00), green(0x00, 0x80, 0x00),
    aqua(0x00, 0xff, 0xff), teal(0x00, 0x80, 0x80), blue(0x00, 0x00, 0xff),
    navy(0x00, 0x00, 0x80), fuchsia(0xff, 0x00, 0xff),
    purple(0x80, 0x00, 0x80);

} // namespace colors

static inline uint8_t
max_channel_value(uint8_t)
{
    return 0xff;
}

// RGBA color/pixel, w/ premultiplied alpha
api(struct with(Channel : uint8_t))
template<class Channel>
struct rgba
{
    Channel r, g, b, a;
};

using rgba8 = rgba<uint8_t>;

std::ostream&
operator<<(std::ostream& s, rgba8 const& c);

template<class Channel>
rgba<Channel>
to_rgba(rgb<Channel> color)
{
    return rgba<Channel>{
        color.r, color.g, color.b, max_channel_value(Channel())};
}

// premultiply the color by the alpha to form an rgba8 value
rgba8
apply_alpha(rgb8 color, uint8_t alpha);
rgba8
apply_alpha(rgba8 color, uint8_t alpha);

// interpolate(a, b, factor) = a * (1 - factor) + b * factor
rgba8
interpolate(rgba8 const& a, rgba8 const& b, double factor);

using rgb16 = rgb<uint16_t>;

// interpolation, scaling
template<class T, class F>
rgb<T>
interpolate(rgb<T> const& a, rgb<T> const& b, F f)
{
    return rgb<T>(
        astroid::interpolate(a.r, b.r, f),
        astroid::interpolate(a.g, b.g, f),
        astroid::interpolate(a.b, b.b, f));
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
    astroid::linear_function<MappingT> const& mapping, rgb<T> const& p)
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

// Given a color palette and a list of colors that are already in use, select
// a new color to add to the group that will stand out.
rgb8
choose_new_color(
    std::vector<rgb8> const& palette, std::vector<rgb8> const& already_in_use);

// interpolation, scaling
template<class T, class F>
rgba<T>
interpolate(rgba<T> const& a, rgba<T> const& b, F f)
{
    return rgba<T>(
        astroid::interpolate(a.r, b.r, f),
        astroid::interpolate(a.g, b.g, f),
        astroid::interpolate(a.b, b.b, f),
        astroid::interpolate(a.a, b.a, f));
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
    astroid::linear_function<MappingT> const& mapping, rgba<T> const& p)
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

} // namespace astroid

#endif
