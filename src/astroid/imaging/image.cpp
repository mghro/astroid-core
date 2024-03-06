#include <astroid/imaging/image.hpp>
#include <astroid/imaging/variant.hpp>
#include <cradle/typing/core/type_definitions.h>

namespace astroid {

// All this has already been defined for variant images, and since we're only
// defining this for images that are convertible to variants, the easiest way
// is to simply convert them to variants and apply those functions.
// This is a little inefficient for the comparisons, but the performance of
// those is dominated by the actual comparison of the pixels anyway.
// Also, note that this method is the desired behavior for the conversion to
// and from dynamic values, since we want those to have the same dynamic
// representation as the variant form.
#define ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(N, T)                          \
    bool operator==(                                                          \
        image<N, T, shared> const& a, image<N, T, shared> const& b)           \
    {                                                                         \
        return as_variant(a) == as_variant(b);                                \
    }                                                                         \
    bool operator!=(                                                          \
        image<N, T, shared> const& a, image<N, T, shared> const& b)           \
    {                                                                         \
        return !(a == b);                                                     \
    }                                                                         \
    bool operator<(                                                           \
        image<N, T, shared> const& a, image<N, T, shared> const& b)           \
    {                                                                         \
        return as_variant(a) < as_variant(b);                                 \
    }                                                                         \
    void to_dynamic(cradle::dynamic* v, image<N, T, shared> const& x)         \
    {                                                                         \
        to_dynamic(v, as_variant(x));                                         \
    }                                                                         \
    void from_dynamic(image<N, T, shared>* x, cradle::dynamic const& v)       \
    {                                                                         \
        image<N, variant, shared> tmp;                                        \
        from_dynamic(&tmp, v);                                                \
        *x = cast_variant<T>(tmp);                                            \
    }                                                                         \
    size_t deep_sizeof(image<N, T, shared> const& x)                          \
    {                                                                         \
        return deep_sizeof(as_variant(x));                                    \
    }                                                                         \
    std::ostream& operator<<(std::ostream& s, image<N, T, shared> const& x)   \
    {                                                                         \
        return (s << as_variant(x));                                          \
    }                                                                         \
    }                                                                         \
    void cradle::definitive_type_info_query<                                  \
        astroid::image<N, T, astroid::shared>>::get(api_type_info* info)      \
    {                                                                         \
        cradle::definitive_type_info_query<                                   \
            astroid::image<N, astroid::variant, astroid::shared>>::get(info); \
    }                                                                         \
    namespace astroid {                                                       \
    size_t                                                                    \
    invoke_hash(image<N, T, shared> const& x)                                 \
    {                                                                         \
        using cradle::invoke_hash;                                            \
        return invoke_hash(as_variant(x));                                    \
    }

#define ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(T)                    \
    ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(1, T)                              \
    ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(2, T)                              \
    ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE(3, T)

ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::int8_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::uint8_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::int16_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::uint16_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::int32_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::uint32_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::int64_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::uint64_t)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(float)
ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(double)

ASTROID_DEFINE_REGULAR_IMAGE_INTERFACE_FOR_TYPE(astroid::rgba8)

} // namespace astroid
