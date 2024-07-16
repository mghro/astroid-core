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

inline size_t
deep_sizeof(date const& x)
{
    return sizeof(x);
}

inline size_t
deep_sizeof(datetime const& x)
{
    return sizeof(x);
}

inline void
to_dynamic(cradle::dynamic* v, date const& x)
{
    *v = to_thinknode_string(x);
}

inline void
from_dynamic(date* x, cradle::dynamic const& v)
{
    *x = parse_thinknode_date(cast<std::string>(v));
}

void
to_dynamic(cradle::dynamic* v, datetime const& x);

void
from_dynamic(datetime* x, cradle::dynamic const& v);

} // namespace astroid

#endif
