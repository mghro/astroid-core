#include <astroid/imaging/contiguous.hpp>
#include <astroid/imaging/variant.hpp>

namespace astroid {

string
get_pixel_format_name(pixel_format fmt)
{
    switch (fmt)
    {
        case pixel_format::RGB:
            return "rgb";
        case pixel_format::RGBA:
            return "rgba";
        case pixel_format::GRAY:
            return "gray";
        default:
            throw exception("invalid pixel format");
    }
}

unsigned
get_channel_count(pixel_format fmt)
{
    switch (fmt)
    {
        case pixel_format::RGB:
            return 3;
        case pixel_format::RGBA:
            return 4;
        case pixel_format::GRAY:
            return 1;
        default:
            throw exception("invalid pixel format");
    }
}

string
get_channel_type_name(channel_type type)
{
    switch (type)
    {
        case channel_type::INT8:
            return "8-bit signed integer";
        case channel_type::UINT8:
            return "8-bit unsigned integer";
        case channel_type::INT16:
            return "16-bit signed integer";
        case channel_type::UINT16:
            return "16-bit unsigned integer";
        case channel_type::INT32:
            return "32-bit signed integer";
        case channel_type::UINT32:
            return "32-bit unsigned integer";
        case channel_type::INT64:
            return "64-bit signed integer";
        case channel_type::UINT64:
            return "64-bit unsigned integer";
        case channel_type::FLOAT:
            return "32-bit float";
        case channel_type::DOUBLE:
            return "64-bit float";
        default:
            throw exception("invalid channel type");
    }
}

size_t
get_channel_size(channel_type type)
{
    switch (type)
    {
        case channel_type::INT8:
            return 1;
        case channel_type::UINT8:
            return 1;
        case channel_type::INT16:
            return 2;
        case channel_type::UINT16:
            return 2;
        case channel_type::INT32:
            return 4;
        case channel_type::UINT32:
            return 4;
        case channel_type::INT64:
            return 8;
        case channel_type::UINT64:
            return 8;
        case channel_type::FLOAT:
            return 4;
        case channel_type::DOUBLE:
            return 8;
        default:
            throw exception("invalid channel type");
    }
}

struct pixel_equality_comparison
{
    bool equal;
    pixel_equality_comparison() : equal(true)
    {
    }
    template<class T>
    void
    operator()(T const& a, T const& b)
    {
        if (a != b)
            equal = false;
    }
};
template<unsigned N>
struct image_equality_comparison
{
    bool equal;
    image<N, variant, shared> const* variant_b;
    template<class Pixel>
    void
    operator()(image<N, Pixel, const_view> const& a)
    {
        image<N, Pixel, const_view> b
            = cast_variant<Pixel>(as_const_view(*variant_b));
        pixel_equality_comparison comparison;
        foreach_pixel2(a, b, comparison);
        equal = comparison.equal;
    }
};

template<unsigned N, class Pixel>
bool
variant_images_equal(
    image<N, Pixel, shared> const& a, image<N, Pixel, shared> const& b)
{
    if (a.pixels.type_info != b.pixels.type_info || a.size != b.size
        || a.origin != b.origin || a.axes != b.axes
        || a.value_mapping != b.value_mapping || a.units != b.units)
    {
        return false;
    }
    image_equality_comparison<N> comparison;
    comparison.variant_b = &b;
    apply_fn_to_variant(comparison, as_const_view(a));
    return comparison.equal;
}

struct pixel_less_than_comparison
{
    int state;
    pixel_less_than_comparison() : state(0)
    {
    }
    template<class T>
    void
    operator()(T const& a, T const& b)
    {
        if (state == 0)
        {
            if (a < b)
                state = -1;
            else if (b < a)
                state = 1;
        }
    }
};
template<unsigned N>
struct image_less_than_comparison
{
    bool less_than;
    image<N, variant, shared> const* variant_b;
    template<class Pixel>
    void
    operator()(image<N, Pixel, const_view> const& a)
    {
        image<N, Pixel, const_view> b
            = cast_variant<Pixel>(as_const_view(*variant_b));
        pixel_less_than_comparison comparison;
        foreach_pixel2(a, b, comparison);
        less_than = comparison.state < 0;
    }
};
template<unsigned N>
bool
variant_less_than(
    image<N, variant, shared> const& a, image<N, variant, shared> const& b)
{
    if (a.pixels.type_info < b.pixels.type_info)
        return true;
    if (b.pixels.type_info < a.pixels.type_info)
        return false;
    if (a.size < b.size)
        return true;
    if (b.size < a.size)
        return false;
    if (a.origin < b.origin)
        return true;
    if (b.origin < a.origin)
        return false;
    for (unsigned i = 0; i != N; ++i)
    {
        if (a.axes[i] < b.axes[i])
            return true;
        if (b.axes[i] < a.axes[i])
            return false;
    }
    if (a.value_mapping < b.value_mapping)
        return true;
    if (b.value_mapping < a.value_mapping)
        return false;
    if (a.units < b.units)
        return true;
    if (b.units < a.units)
        return false;

    image_less_than_comparison<N> comparison;
    comparison.variant_b = &b;
    apply_fn_to_variant(comparison, as_const_view(a));
    return comparison.less_than;
}

template<unsigned N>
void
variant_from_dynamic(image<N, variant, shared>* x, cradle::dynamic const& v)
{
    cradle::dynamic_map const& map = cast<cradle::dynamic_map>(v);
    from_dynamic(&x->pixels.type_info, get_field(map, "type_info"));
    from_dynamic(&x->size, get_field(map, "size"));
    x->step = get_contiguous_steps(x->size);
    from_dynamic(&x->origin, get_field(map, "origin"));
    from_dynamic(&x->axes, get_field(map, "axes"));
    from_dynamic(&x->value_mapping, get_field(map, "value_mapping"));
    from_dynamic(&x->units, get_field(map, "units"));
    cradle::dynamic pixels = get_field(map, "pixels");
    cradle::blob const& b = cast<cradle::blob>(pixels);
    size_t expected_size = product(x->size)
                           * get_channel_size(x->pixels.type_info.type)
                           * get_channel_count(x->pixels.type_info.format);
    cradle::check_array_size(expected_size, b.size());
    x->pixels.ownership = b.shared_owner();
    x->pixels.view = b.data();
}

template<unsigned N>
void
variant_to_dynamic(cradle::dynamic* v, image<N, variant, shared> const& y)
{
    image<N, variant, shared> x = get_contiguous_version(y);
    cradle::dynamic_map map;
    to_dynamic(&map[cradle::dynamic("type_info")], x.pixels.type_info);
    to_dynamic(&map[cradle::dynamic("size")], x.size);
    to_dynamic(&map[cradle::dynamic("origin")], x.origin);
    to_dynamic(&map[cradle::dynamic("axes")], x.axes);
    to_dynamic(&map[cradle::dynamic("value_mapping")], x.value_mapping);
    to_dynamic(&map[cradle::dynamic("units")], x.units);
    map[cradle::dynamic("pixels")] = cradle::blob(
        x.pixels.ownership,
        reinterpret_cast<std::byte const*>(x.pixels.view),
        product(x.size) * get_channel_size(x.pixels.type_info.type)
            * get_channel_count(x.pixels.type_info.format));
    *v = std::move(map);
}

template<unsigned N>
size_t
deep_sizeof_variant(image<N, variant, shared> const& x)
{
    return sizeof(x)
           + product(x.size) * get_channel_size(x.pixels.type_info.type)
                 * get_channel_count(x.pixels.type_info.format);
}

template<unsigned N>
cradle::api_type_info
get_variant_type_info()
{
    static string name = "image" + astroid::to_string(N);
    static string description = astroid::to_string(N) + "D image";
    std::map<string, cradle::api_structure_field_info> fields;
    image<N, variant, shared> x;
    fields["type_info"] = cradle::make_api_structure_field_info(
        "the type of the image pixels",
        cradle::get_type_info<decltype(x.pixels.type_info)>(),
        some(false));
    fields["size"] = cradle::make_api_structure_field_info(
        "the size (in pixels) of the image",
        cradle::get_type_info<decltype(x.size)>(),
        some(false));
    fields["origin"] = cradle::make_api_structure_field_info(
        "the location in space of the outside corner of the first pixel",
        cradle::get_type_info<decltype(x.origin)>(),
        some(false));
    fields["axes"] = cradle::make_api_structure_field_info(
        "vectors describing the orientation of the image axes in space - Each "
        "vector is one pixel long.",
        cradle::get_type_info<decltype(x.axes)>(),
        some(false));
    fields["value_mapping"] = cradle::make_api_structure_field_info(
        "a linear function mapping raw pixel values to image values",
        cradle::get_type_info<decltype(x.value_mapping)>(),
        some(false));
    fields["units"] = cradle::make_api_structure_field_info(
        "the units of the image values",
        cradle::get_type_info<decltype(x.units)>(),
        some(false));
    fields["pixels"] = cradle::make_api_structure_field_info(
        "the array of raw pixel values",
        cradle::make_api_type_info_with_blob_type(cradle::api_blob_type{}),
        some(false));
    return cradle::make_api_type_info_with_structure_type(
        cradle::make_api_structure_info(fields));
}

template<unsigned N>
size_t
hash_variant_image(image<N, variant, shared> const& x)
{
    size_t hash = 0;
    hash = cradle::combine_hashes(hash, cradle::invoke_hash(x.size));
    hash = cradle::combine_hashes(hash, cradle::invoke_hash(x.origin));
    hash = cradle::combine_hashes(hash, cradle::invoke_hash(x.axes));
    hash = cradle::combine_hashes(hash, cradle::invoke_hash(x.value_mapping));
    hash = cradle::combine_hashes(hash, cradle::invoke_hash(x.units));
    hash = cradle::combine_hashes(
        hash, cradle::invoke_hash(x.pixels.type_info));
    // TODO: Hash pixels.
    return hash;
}

template<unsigned N>
void
update_variant_image_unique_hash(
    cradle::unique_hasher& hasher, image<N, variant, shared> const& x)
{
    using cradle::update_unique_hash;
    update_unique_hash(hasher, x.size);
    update_unique_hash(hasher, x.origin);
    update_unique_hash(hasher, x.axes);
    update_unique_hash(hasher, x.value_mapping);
    update_unique_hash(hasher, x.units);
    update_unique_hash(hasher, x.pixels.type_info);
    hasher.encode_bytes(
        x.pixels.view,
        product(x.size) * get_channel_size(x.pixels.type_info.type)
            * get_channel_count(x.pixels.type_info.format));
}

#define ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(N, T)                          \
    bool operator==(T const& a, T const& b)                                   \
    {                                                                         \
        return variant_images_equal(a, b);                                    \
    }                                                                         \
    bool operator!=(T const& a, T const& b)                                   \
    {                                                                         \
        return !(a == b);                                                     \
    }                                                                         \
    bool operator<(T const& a, T const& b)                                    \
    {                                                                         \
        return variant_less_than(a, b);                                       \
    }                                                                         \
    void to_dynamic(cradle::dynamic* v, T const& x)                           \
    {                                                                         \
        return variant_to_dynamic(v, x);                                      \
    }                                                                         \
    void from_dynamic(T* x, cradle::dynamic const& v)                         \
    {                                                                         \
        return variant_from_dynamic(x, v);                                    \
    }                                                                         \
    size_t deep_sizeof(T const& x)                                            \
    {                                                                         \
        return deep_sizeof_variant(x);                                        \
    }                                                                         \
    std::ostream& operator<<(std::ostream& s, T const& x)                     \
    {                                                                         \
        cradle::dynamic v;                                                    \
        variant_to_dynamic(&v, x);                                            \
        return s << v;                                                        \
    }                                                                         \
    }                                                                         \
    void cradle::definitive_type_info_query<                                  \
        astroid::image<N, astroid::variant, astroid::shared>>::               \
        get(api_type_info* info)                                              \
    {                                                                         \
        *info = astroid::get_variant_type_info<N>();                          \
    }                                                                         \
    namespace astroid {                                                       \
    size_t                                                                    \
    hash_value(image<N, variant, shared> const& x)                            \
    {                                                                         \
        return astroid::hash_variant_image(x);                                \
    }                                                                         \
    void                                                                      \
    update_unique_hash(                                                       \
        cradle::unique_hasher& hasher, image<N, variant, shared> const& x)    \
    {                                                                         \
        return astroid::update_variant_image_unique_hash(hasher, x);          \
    }

ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(1, astroid::image1)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(2, astroid::image2)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(3, astroid::image3)

} // namespace astroid
