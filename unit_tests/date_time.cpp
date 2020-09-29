#include <cradle/common.hpp>
#include <cradle/date_time.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <cradle/test.hpp>

TEST_CASE("value_conversion")
{
    using namespace cradle;

    date d(2012, boost::gregorian::Jun, 13);
    value v;
    to_value(&v, d);
    date e;
    from_value(&e, v);
    REQUIRE(d == e);

    cradle::time t(d, boost::posix_time::hours(1));
    to_value(&v, t);
    cradle::time u;
    from_value(&u, v);
    REQUIRE(t == u);
}
