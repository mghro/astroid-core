#include <astroid/date_time.hpp>

#include <chrono>
#include <cstdlib>
#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/scoped_array.hpp>

#include <astroid/test.hpp>

using namespace astroid;

using namespace std::chrono;

static void
check_thinknode_datetime_round_trip(
    datetime const& t, std::string const& expected_encoding)
{
    string encoded = to_thinknode_string(t);
    REQUIRE(encoded == expected_encoding);
    REQUIRE(parse_thinknode_datetime(encoded) == t);
}

TEST_CASE("thinknode_datetime_tests")
{
    check_thinknode_datetime_round_trip(
        sys_days(2017y / January / 25) + hours(6) + minutes(2) + seconds(3),
        "2017-01-25T06:02:03.000Z");

    check_thinknode_datetime_round_trip(
        sys_days(2024y / July / 16) + hours(2) + minutes(27) + seconds(13)
            + milliseconds(420),
        "2024-07-16T02:27:13.420Z");
}
