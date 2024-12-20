#ifndef ASTROID_PREPROCESSED_HPP
#define ASTROID_PREPROCESSED_HPP

#include <chrono>
#include <functional>

namespace astroid {

using date = std::chrono::year_month_day;
using datetime = std::chrono::system_clock::time_point;

} // namespace astroid

// Because these overloads can't rely on ADL, they have to be provided before
// the generic implementations are included. (There must be a better way...)

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

namespace boost {

inline std::size_t
hash_value(astroid::date const& d) noexcept
{
    return std::chrono::sys_days(d).time_since_epoch().count();
}

inline std::size_t
hash_value(astroid::datetime const& t) noexcept
{
    return t.time_since_epoch().count();
}

} // namespace boost

#include <cradle/inner/encodings/cereal_value.h>
#include <cradle/inner/encodings/msgpack_adaptors_main.h>
#include <cradle/inner/requests/function.h>
#include <cradle/inner/requests/normalization_uuid.h>
#include <cradle/typing/core/preprocessed.h>

#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>

#include <astroid/common.hpp>

#endif
