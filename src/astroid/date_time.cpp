#include <astroid/date_time.h>

#include <chrono>
#include <format>

#include <astroid/common.hpp>

namespace astroid {

date
parse_thinknode_date(std::string const& s)
{
    std::istringstream is(s);
    date d;
    is >> std::chrono::parse("%Y-%m-%d", d);
    if (!is.fail() && is.peek() == std::istringstream::traits_type::eof())
        return d;
    throw exception("unrecognized date format");
}

string
to_string(date const& d)
{
    return std::format("{:%Y-%m-%d}", d);
}

string
to_thinknode_string(date const& d)
{
    return std::format("{:%Y-%m-%d}", d);
}

datetime
parse_thinknode_datetime(std::string const& s)
{
    std::istringstream is(s);
    datetime t;
    is >> std::chrono::parse("%Y-%m-%dT%H:%M:%S%Z", t);
    if (!is.fail() && is.peek() == std::istringstream::traits_type::eof())
        return t;
    throw exception("unrecognized date/time format");
}

string
to_string(datetime const& t)
{
    return std::format("{:%Y-%m-%d %X}", t);
}

string
to_local_string(datetime const& t)
{
    return std::format(
        "{:%Y-%m-%d %X}", std::chrono::current_zone()->to_local(t));
}

string
to_local_date_string(datetime const& t)
{
    return std::format(
        "{:%Y-%m-%d}", std::chrono::current_zone()->to_local(t));
}

string
to_local_time_string(datetime const& t)
{
    return std::format("{:%X}", std::chrono::current_zone()->to_local(t));
}

string
to_thinknode_string(datetime const& t)
{
    return std::format(
        "{:%Y-%m-%dT%H:%M:%SZ}",
        std::chrono::floor<std::chrono::milliseconds>(t));
}

} // namespace astroid

namespace cradle {

void
to_dynamic(cradle::dynamic* v, astroid::datetime const& x)
{
    using namespace std::chrono;
    auto milliseconds_since_epoch
        = duration_cast<milliseconds>(x - sys_days(1970y / January / 1));
    *v = boost::posix_time::ptime(
        boost::gregorian::date(1970, 1, 1),
        boost::posix_time::milliseconds(milliseconds_since_epoch.count()));
}

void
from_dynamic(astroid::datetime* x, cradle::dynamic const& v)
{
    using namespace std::chrono;
    auto t = cast<boost::posix_time::ptime>(v);
    auto milliseconds_since_epoch
        = (t - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1)))
              .total_milliseconds();
    using namespace std::chrono;
    *x = sys_days(1970y / January / 1)
         + milliseconds(milliseconds_since_epoch);
}

} // namespace cradle
