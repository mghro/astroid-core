#include <astroid/imaging/binary_ops.hpp>
#include <astroid/imaging/variant.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("unaligned_test")
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
        72,
        2,
        0,
        0,
        1,
        6,
        0,
        4,
        0,
    };
    image<2, astroid::uint8_t, const_view> src2
        = make_const_view(data2, make_vector(s, s));
    set_spatial_mapping(
        src2, make_vector<double>(-3, -3), make_vector<double>(2, 2));

    image<2, double, shared> dst
        = compute_difference(as_variant(src1), as_variant(src2));

    double ref[] = {0, -1, -6, 0, 10, 7, -4, 3, 0};
    ASTROID_CHECK_IMAGE(dst, ref, ref + s * s);
}

TEST_CASE("unaligned_max_test")
{
    unsigned const s = 3;

    astroid::uint8_t data1[] = {
        0,
        0,
        0,
        0,
        10,
        0,
        0,
        0,
        0,
    };
    image<2, astroid::uint8_t, const_view> src1
        = make_const_view(data1, make_vector(s, s));

    astroid::uint8_t data2[] = {
        72,
        2,
        0,
        0,
        1,
        6,
        0,
        4,
        0,
    };
    image<2, astroid::uint8_t, const_view> src2
        = make_const_view(data2, make_vector(s, s));
    set_spatial_mapping(
        src2, make_vector<double>(-3, -3), make_vector<double>(2, 2));

    REQUIRE(compute_max_difference(as_variant(src1), as_variant(src2)) == 10);
}
