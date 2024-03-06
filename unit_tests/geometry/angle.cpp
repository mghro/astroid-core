#include <astroid/geometry/angle.hpp>
#include <boost/assign/std/vector.hpp>

#include <astroid/test.hpp>

using namespace astroid;
using namespace boost::assign;

TEST_CASE("angle_test")
{
    angle<double, radians> r(7 * pi / 2);
    angle<double, degrees> d(r);
    d.normalize();
    ASTROID_CHECK_ALMOST_EQUAL(d.get(), -90.);
    d = -1000;
    d.normalize();
    ASTROID_CHECK_ALMOST_EQUAL(d.get(), 80.);
    d = -180;
    d.normalize();
    ASTROID_CHECK_ALMOST_EQUAL(d.get(), 180.);
    ASTROID_CHECK_ALMOST_EQUAL((angle<double, radians>(d).get()), pi);
    d = d + d;
    d.normalize();
    ASTROID_CHECK_ALMOST_EQUAL(d.get(), 0.);
    ASTROID_CHECK_ALMOST_EQUAL(sin(d), 0.);
    ASTROID_CHECK_ALMOST_EQUAL(cos(d), 1.);
    d = d + angle<double, degrees>(1);
    ASTROID_CHECK_ALMOST_EQUAL(d.get(), 1.);
    ASTROID_CHECK_ALMOST_EQUAL(sin(d), 0.017452406437283512819418978516316);
    ASTROID_CHECK_ALMOST_EQUAL(cos(d), 0.99984769515639123915701155881391);
}
