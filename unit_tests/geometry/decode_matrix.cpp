#include <astroid/geometry/decode_matrix.hpp>
#include <astroid/geometry/transformations.hpp>

#include <astroid/test.hpp>

using namespace astroid;

TEST_CASE("has_rotation_2d_test")
{
    typedef angle<double, degrees> angle_t;
    REQUIRE(has_rotation(rotation(angle_t(1))));
    REQUIRE(!has_rotation(translation(make_vector<double>(1, 0))));
    REQUIRE(!has_rotation(rotation(angle_t(0))));
    REQUIRE(has_rotation(
        rotation(angle_t(1)) * translation(make_vector<double>(1, 0))));
    REQUIRE(!has_rotation(scaling_transformation(make_vector<double>(1, 0))));
    REQUIRE(has_rotation(
        rotation(angle_t(1))
        * scaling_transformation(make_vector<double>(1, 0))));
}
