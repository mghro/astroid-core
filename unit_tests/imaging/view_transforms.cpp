#include <astroid/common.hpp>
#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/sample.hpp>
#include <astroid/imaging/view_transforms.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("r90ccw_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed
        = raw_rotated_90ccw_view(view);
    astroid::uint8_t correct[] = {
        3,
        6,
        9,
        2,
        5,
        8,
        1,
        4,
        7,
    };
    ASTROID_CHECK_IMAGE(xformed, correct, correct + sizeof(correct));
}
TEST_CASE("r90cw_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed
        = raw_rotated_90cw_view(view);
    astroid::uint8_t correct[] = {
        7,
        4,
        1,
        8,
        5,
        2,
        9,
        6,
        3,
    };
    ASTROID_CHECK_IMAGE(xformed, correct, correct + sizeof(correct));
}
TEST_CASE("r180_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed
        = raw_rotated_180_view(view);
    astroid::uint8_t correct[] = {
        9,
        8,
        7,
        6,
        5,
        4,
        3,
        2,
        1,
    };
    ASTROID_CHECK_IMAGE(xformed, correct, correct + sizeof(correct));
}

TEST_CASE("raw_flipx_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed = raw_flipped_view(view, 0);
    astroid::uint8_t correct[] = {
        3,
        2,
        1,
        6,
        5,
        4,
        9,
        8,
        7,
    };
    ASTROID_CHECK_IMAGE(xformed, correct, correct + sizeof(correct));
}
TEST_CASE("raw_flipy_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed = raw_flipped_view(view, 1);
    astroid::uint8_t correct[] = {
        7,
        8,
        9,
        4,
        5,
        6,
        1,
        2,
        3,
    };
    ASTROID_CHECK_IMAGE(xformed, correct, correct + sizeof(correct));
}

TEST_CASE("flipx_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed = flipped_view(view, 0);

    size_t const n_points = 10;
    double points[n_points][2]
        = {{1, 0},
           {2, 1},
           {1.5, 1},
           {0, 0},
           {2.5, 1.5},
           {2.5, 1},
           {-1, 0},
           {0, 1},
           {0.5, 2},
           {0.5, 1}};

    for (size_t i = 0; i != n_points; ++i)
    {
        optional<double> s1 = interpolated_image_sample(
            view, make_vector(points[i][0], points[i][1]));
        optional<double> s2 = interpolated_image_sample(
            xformed, make_vector(-points[i][0], points[i][1]));
        REQUIRE((s1 ? true : false) == (s2 ? true : false));
        if (s1 && s2)
            ASTROID_CHECK_ALMOST_EQUAL(s1.value(), s2.value());
    }
}
TEST_CASE("flipy_test")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    image<2, astroid::uint8_t, const_view> xformed = flipped_view(view, 1);

    size_t const n_points = 10;
    double points[n_points][2]
        = {{1, 0},
           {2, 1},
           {1.5, 1},
           {1, -1},
           {2.5, 1.5},
           {1, 2.5},
           {-1, 2.5},
           {0, 1},
           {0.5, 2},
           {0.5, 0.5}};

    for (size_t i = 0; i != n_points; ++i)
    {
        optional<double> s1 = interpolated_image_sample(
            view, make_vector(points[i][0], points[i][1]));
        optional<double> s2 = interpolated_image_sample(
            xformed, make_vector(points[i][0], -points[i][1]));
        REQUIRE((s1 ? true : false) == (s2 ? true : false));
        if (s1 && s2)
            ASTROID_CHECK_ALMOST_EQUAL(s1.value(), s2.value());
    }
}

void
test_aligned_view(image<2, astroid::uint8_t, const_view> const& view)
{
    REQUIRE(is_orthogonal_to_axes(view));

    image<2, astroid::uint8_t, const_view> aligned = aligned_view(view);

    REQUIRE(is_axis_aligned(aligned));

    size_t const n_points = 20;
    double points[n_points][2]
        = {{1, 0.1},   {2, 1},     {1.5, 1},   {1, -1},   {2.5, 1.5},
           {1, 2.5},   {-1, 2.5},  {-0.1, 1},  {0.5, 2},  {0.5, 0.5},
           {-1, 0.5},  {-2, 1},    {-1.5, -1}, {-1, 1},   {-2.5, -1.5},
           {-1, -2.5}, {-1, -2.5}, {0.1, -1},  {0.5, -2}, {-0.5, 0.5}};

    for (size_t i = 0; i != n_points; ++i)
    {
        vector2d p = make_vector(points[i][0], points[i][1]);
        optional<double> s1 = interpolated_image_sample(view, p);
        optional<double> s2 = interpolated_image_sample(aligned, p);
        REQUIRE((s1 ? true : false) == (s2 ? true : false));
        if (s1 && s2)
            ASTROID_CHECK_ALMOST_EQUAL(s1.value(), s2.value());
    }
}

TEST_CASE("aligned_test_2d")
{
    astroid::uint8_t original[] = {
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
    };
    image<2, astroid::uint8_t, const_view> view
        = make_const_view(original, make_vector<unsigned>(3, 3));

    test_aligned_view(view);
    test_aligned_view(flipped_view(view, 0));
    test_aligned_view(flipped_view(view, 1));

    test_aligned_view(
        transformed_view(view, rotation(angle<double, degrees>(90))));
    test_aligned_view(
        transformed_view(view, rotation(angle<double, degrees>(-90))));
    test_aligned_view(
        transformed_view(view, rotation(angle<double, degrees>(180))));
    test_aligned_view(transformed_view(
        flipped_view(view, 1), rotation(angle<double, degrees>(-90))));
}
