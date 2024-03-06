#ifndef ASTROID_DATE_TIME_HPP
#define ASTROID_DATE_TIME_HPP

#include <astroid/common.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace astroid {

typedef boost::gregorian::date date;
typedef boost::posix_time::ptime time;

// Get a string representation of a date.
std::string
to_string(date const& d);

// Get a string representation of a date/time.
std::string
to_string(time const& t);

// Get a string representation of a date/time adjust to the local time zone.
std::string
to_local_string(time const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the date portion.
std::string
to_local_date_string(time const& t);

// Given a date/time, convert it to the local time zone and get a string
// represention of just the time portion.
std::string
to_local_time_string(time const& t);

struct value;

time
parse_time(std::string const& s);
string
to_value_string(time const& t);

// expanded representation of a date
api(struct internal)
struct expanded_date
{
    int year;
    // January is 1
    int month;
    int day;
};

expanded_date
expand_date(date const& collapsed);

date
collapse_date(expanded_date const& expanded);

} // namespace astroid

#endif
