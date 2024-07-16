#ifndef ASTROID_DATE_TIME_HPP
#define ASTROID_DATE_TIME_HPP

#include <chrono>

#include <astroid/common.hpp>

namespace astroid {

typedef std::chrono::year_month_day date;
typedef std::chrono::system_clock::time_point datetime;

date
parse_thinknode_date(std::string const& s);

string
to_thinknode_string(date const& d);

// Get a string representation of a date.
std::string
to_string(date const& d);

// Get a string representation of a date/time.
std::string
to_string(datetime const& t);

// Get a string representation of a date/time adjust to the local time zone.
std::string
to_local_string(datetime const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the date portion.
std::string
to_local_date_string(datetime const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the time portion.
std::string
to_local_time_string(datetime const& t);

struct value;

datetime
parse_thinknode_datetime(std::string const& s);

string
to_thinknode_string(datetime const& t);

} // namespace astroid

// Hmm... Is there a better way to manage namespaces/lookup?

namespace cradle {

template<>
struct type_info_query<astroid::date>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_string_type(api_string_type());
    }
};

template<>
struct type_info_query<astroid::datetime>
{
    static void
    get(api_type_info* info)
    {
        *info = make_api_type_info_with_datetime_type(api_datetime_type());
    }
};

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

inline void
to_dynamic(cradle::dynamic* v, astroid::date const& x)
{
    *v = astroid::to_thinknode_string(x);
}

inline void
from_dynamic(astroid::date* x, cradle::dynamic const& v)
{
    *x = astroid::parse_thinknode_date(cast<std::string>(v));
}

void
to_dynamic(cradle::dynamic* v, astroid::datetime const& x);

void
from_dynamic(astroid::datetime* x, cradle::dynamic const& v);

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

#endif
