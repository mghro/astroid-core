#include <astroid/imaging/api.hpp>
#include <astroid/imaging/variant.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("image_division")
{
    unsigned const s = 3;

    astroid::uint8_t data1[] = {
        1,
        5,
        0,
        4,
        10,
        7,
        0,
        3,
        0,
    };
    image<2, astroid::uint8_t, const_view> src1
        = make_const_view(data1, make_vector(s, s));

    astroid::uint8_t data2[] = {
        2,
        2,
        1,
        1,
        2,
        2,
        1,
        3,
        1,
    };
    image<2, astroid::uint8_t, const_view> src2
        = make_const_view(data2, make_vector(s, s));

    auto dst = image_division(
        as_variant(make_eager_image_copy(src1)),
        as_variant(make_eager_image_copy(src2)));

    double ref[] = {0.5, 2.5, 0, 4, 5, 3.5, 0, 1, 0};
    ASTROID_CHECK_IMAGE(cast_variant<double>(dst), ref, ref + s * s);
}
