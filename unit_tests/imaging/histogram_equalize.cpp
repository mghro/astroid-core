#include <astroid/common.hpp>
#include <astroid/imaging/histogram_equalize.hpp>
#include <astroid/imaging/image.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("histogram_equalize_test")
{
    unsigned const s = 2;
    astroid::uint16_t data[s * s] = {4, 4, 5, 6};
    image<2, astroid::uint16_t, const_view> src
        = make_const_view(data, make_vector(s, s));

    image<2, astroid::uint8_t, unique> result;
    create_image(result, make_vector(s, s));
    histogram_equalize(result, src);
    astroid::uint8_t results[s * s] = {
        0,
        0,
        170,
        255,
    };
    ASTROID_CHECK_IMAGE(result, &results[0], results + s * s);
}
