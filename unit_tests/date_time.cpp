#include <astroid/date_time.h>

#include <chrono>
#include <cradle/inner/core/hash.h>
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

TEST_CASE("date/time dynamic value conversion")
{
    datetime t = sys_days(2024y / July / 16) + hours(2) + minutes(27)
                 + seconds(13) + milliseconds(420);
    datetime u;
    cradle::dynamic v;
    to_dynamic(&v, t);
    from_dynamic(&u, v);
    REQUIRE(t == u);
}

TEST_CASE("date/time hashes")
{
    datetime t = sys_days(2024y / July / 16) + hours(2) + minutes(27)
                 + seconds(13) + milliseconds(420);
    // The specific return value doesn't really matter, but this should at
    // least compile.
    cradle::invoke_hash(t);
}

TEST_CASE("date/time deep_sizeof")
{
    datetime t = sys_days(2024y / July / 16) + hours(2) + minutes(27)
                 + seconds(13) + milliseconds(420);
    REQUIRE(cradle::deep_sizeof(t) == sizeof(t));

    // There have been issues with this not compiling.
    std::optional<datetime> o;
    REQUIRE(cradle::deep_sizeof(o) > 0);
}
