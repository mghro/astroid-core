#ifndef ASTROID_PREPROCESSED_HPP
#define ASTROID_PREPROCESSED_HPP

#include <chrono>
#include <functional>

namespace astroid {

using date = std::chrono::year_month_day;
using datetime = std::chrono::system_clock::time_point;

} // namespace astroid

namespace cradle {

inline size_t
deep_sizeof(astroid::date const& x)
{
    return sizeof(x);
}

inline size_t
deep_sizeof(astroid::datetime const& x)
{
    return sizeof(x);
}

} // namespace cradle

template<>
struct std::hash<astroid::date>
{
    std::size_t
    operator()(astroid::date const& d) const noexcept
    {
        return std::chrono::sys_days(d).time_since_epoch().count();
    }
};

template<>
struct std::hash<astroid::datetime>
{
    std::size_t
    operator()(astroid::datetime const& t) const noexcept
    {
        return t.time_since_epoch().count();
    }
};

// TODO: This is bad, but it's the only way I've found to make Boost hashes
// work.
namespace std {

inline std::size_t
hash_value(astroid::date const& d) noexcept
{
    return std::hash<astroid::date>()(d);
}

inline std::size_t
hash_value(astroid::datetime const& t) noexcept
{
    return std::hash<astroid::datetime>()(t);
}

} // namespace std

#include <cradle/inner/encodings/cereal_value.h>
#include <cradle/inner/encodings/msgpack_adaptors_main.h>
#include <cradle/inner/requests/function.h>
#include <cradle/inner/requests/normalization_uuid.h>
#include <cradle/typing/core/preprocessed.h>

#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>

#include <astroid/common.hpp>

#endif
