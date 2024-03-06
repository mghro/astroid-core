#include <astroid/common.hpp>
#include <astroid/imaging/discretize.hpp>
#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/image.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("discretize_test")
{
    unsigned const s = 3;
    double data[s * s] = {
        13,
        11.5,
        7,
        4,
        1,
        4,
        2,
        2,
        5.5,
    };

    image<2, double, const_view> source
        = make_const_view(data, make_vector(s, s));
    source.value_mapping = linear_function<double>(0, 1);
    set_spatial_mapping(
        source, make_vector<double>(4, 0), make_vector<double>(3, 2));

    image<2, astroid::uint8_t, shared> result;
    discretize(result, source, 255);

    astroid::uint8_t correct_result[] = {255, 223, 128, 64, 0, 64, 21, 21, 96};
    ASTROID_CHECK_IMAGE(result, &correct_result[0], correct_result + s * s);
}
