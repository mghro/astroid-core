#include <astroid/geometry/common.hpp>
#include <boost/assign/std/vector.hpp>

#include <astroid/test.hpp>

using namespace astroid;
using namespace boost::assign;

TEST_CASE("vector_of_points_test")
{
    std::vector<vector2i> points;
    points += make_vector(0, 0), make_vector(-1, -1), make_vector(-3, 0),
        make_vector(0, 7), make_vector(3, 3), make_vector(3, 2);
    REQUIRE(
        bounding_box(points) == box2i(make_vector(-3, -1), make_vector(6, 8)));
}

TEST_CASE("structure_geometry_test")
{
}
