#include <boost/assign/std/vector.hpp>
#include <cradle/common.hpp>
#include <cradle/geometry/common.hpp>
#include <cradle/geometry/transformations.hpp>
#include <cradle/math/common.hpp>
#include <vector>

#include <cradle/test.hpp>

using namespace cradle;

TEST_CASE("sizeof_vector")
{
    // Check that vectors don't have any size overhead.
    REQUIRE(sizeof(vector<3, int>) == 3 * sizeof(int));
    REQUIRE(sizeof(vector<1, float>) == sizeof(float));
    REQUIRE(sizeof(vector<2, double>) == 2 * sizeof(double));
}

void
f(vector3i const& v)
{
}

// confirm that the vector operators work as expected
TEST_CASE("vector_operators")
{
    vector<3, int> p = make_vector<int>(1, 1, 0),
                   q = make_vector<int>(4, 2, 6);
    vector<3, int> v = make_vector<int>(3, 1, 2);

    REQUIRE(p - q == make_vector<int>(-3, -1, -6));
    REQUIRE(p + v == make_vector<int>(4, 2, 2));

    REQUIRE(v * 3 == make_vector<int>(9, 3, 6));
    REQUIRE(v / 2 == make_vector<int>(1, 0, 1));

    REQUIRE(!(p == q));
    REQUIRE(p != q);

    REQUIRE(!(p - q == v));
    REQUIRE(p - q != v);
}

TEST_CASE("vector_slice")
{
    {
        vector3i p = make_vector<int>(6, 7, 8);
        REQUIRE(slice(p, 0) == make_vector<int>(7, 8));
        REQUIRE(slice(p, 1) == make_vector<int>(6, 8));
        REQUIRE(slice(p, 2) == make_vector<int>(6, 7));
    }

    {
        vector2f p = make_vector<float>(9, 17);
        REQUIRE(slice(p, 0) == make_vector<float>(17));
        REQUIRE(slice(p, 1) == make_vector<float>(9));
    }

    {
        vector4i p = make_vector<int>(4, 3, 2, 1);
        REQUIRE(slice(p, 0) == make_vector<int>(3, 2, 1));
        REQUIRE(slice(p, 1) == make_vector<int>(4, 2, 1));
        REQUIRE(slice(p, 2) == make_vector<int>(4, 3, 1));
        REQUIRE(slice(p, 3) == make_vector<int>(4, 3, 2));
    }
}

TEST_CASE("test_unslice_vector")
{
    {
        vector3i p = make_vector<int>(6, 7, 8);
        REQUIRE(unslice(p, 0, 0) == make_vector<int>(0, 6, 7, 8));
        REQUIRE(unslice(p, 1, 0) == make_vector<int>(6, 0, 7, 8));
        REQUIRE(unslice(p, 2, 0) == make_vector<int>(6, 7, 0, 8));
        REQUIRE(unslice(p, 3, 0) == make_vector<int>(6, 7, 8, 0));
    }

    {
        vector2d p = make_vector<double>(9, 17);
        REQUIRE(unslice(p, 0, 2.1) == make_vector<double>(2.1, 9, 17));
        REQUIRE(unslice(p, 1, 2.1) == make_vector<double>(9, 2.1, 17));
        REQUIRE(unslice(p, 2, 2.1) == make_vector<double>(9, 17, 2.1));
    }
}

TEST_CASE("test_uniform_vector")
{
    REQUIRE((uniform_vector<3, int>(0)) == make_vector<int>(0, 0, 0));
    REQUIRE(
        (uniform_vector<4, unsigned>(1)) == make_vector<unsigned>(1, 1, 1, 1));
    REQUIRE((uniform_vector<2, float>(6)) == make_vector<float>(6, 6));
}

TEST_CASE("vector_almost_equal")
{
    REQUIRE(almost_equal(
        make_vector<double>(0, 0, 0),
        make_vector<double>(0, 0, default_equality_tolerance<double>() / 2)));
    REQUIRE(!almost_equal(
        make_vector<float>(0, 0, 0), make_vector<float>(0, 0, 1)));
    REQUIRE(almost_equal(
        make_vector<float>(0, 0, 0), make_vector<float>(0, 0, 1), 2));
}

TEST_CASE("vector_cross")
{
    // with objects
    REQUIRE(almost_equal(
        cross(make_vector<double>(1, 0, 0), make_vector<double>(0, 1, 0)),
        make_vector<double>(0, 0, 1)));
    REQUIRE(almost_equal(
        cross(make_vector<double>(0, 1, 0), make_vector<double>(1, 0, 0)),
        make_vector<double>(0, 0, -1)));

    // with expressions
    REQUIRE(almost_equal(
        cross(
            make_vector<double>(1, 0, 0) - make_vector<double>(0, 0, 0),
            make_vector<double>(0, 1, 0) - make_vector<double>(0, 0, 0)),
        make_vector<double>(0, 0, 1)));

    // mixing expressions and objects
    REQUIRE(almost_equal(
        cross(
            make_vector<double>(1, 0, 0) - make_vector<double>(0, 0, 0),
            make_vector<double>(0, 1, 0)),
        make_vector<double>(0, 0, 1)));
    REQUIRE(almost_equal(
        cross(
            make_vector<double>(1, 0, 0),
            make_vector<double>(0, 1, 0) - make_vector<double>(0, 0, 0)),
        make_vector<double>(0, 0, 1)));
}

TEST_CASE("vector_dot")
{
    // with objects
    REQUIRE(almost_equal(
        dot(make_vector<double>(1, 1), make_vector<double>(0.7, 0.3)), 1.));
    REQUIRE(almost_equal(
        dot(make_vector<double>(1, 0, 0), make_vector<double>(0, 1, 0)), 0.));
    REQUIRE(almost_equal(
        dot(make_vector<double>(1), make_vector<double>(0.6)), 0.6));
    REQUIRE(dot(make_vector<int>(1, 2, 0), make_vector<int>(2, 3, 0)) == 8);

    // with expressions
    REQUIRE(almost_equal(
        dot(make_vector<double>(1, 0, 1) - make_vector<double>(0, 0, 0),
            make_vector<double>(0.7, 0, 0.3) - make_vector<double>(0, 0, 0)),
        1.));

    // mixing expressions and objects
    REQUIRE(almost_equal(
        dot(make_vector<float>(1, 1),
            make_vector<float>(0.7f, 0.3f) - make_vector<float>(0, 0)),
        1.f));
    REQUIRE(almost_equal(
        dot(make_vector<double>(1, 0, 1) - make_vector<double>(0, 0, 0),
            make_vector<double>(0.7, 0, 0.3)),
        1.));
}

TEST_CASE("vector_length")
{
    // length2()
    REQUIRE(length2(make_vector<int>(2, 0, 1)) == 5);

    // length()
    REQUIRE(almost_equal(length(make_vector<double>(2, 1)), sqrt(5.)));

    // length() with an expression
    REQUIRE(almost_equal(
        length(make_vector<double>(2, 0, 1) - make_vector<double>(1, 0, 0)),
        sqrt(2.)));
}

TEST_CASE("unit_vector")
{
    REQUIRE(almost_equal(
        unit(make_vector<double>(4, 0, 3)), make_vector<double>(0.8, 0, 0.6)));

    // with an expression
    REQUIRE(almost_equal(
        unit(make_vector<double>(3, 0) - make_vector<double>(0, 4)),
        make_vector<double>(0.6, -0.8)));
}

TEST_CASE("perpendicular_vector")
{
    for (int x = -1; x < 2; ++x)
    {
        for (int y = -1; y < 2; ++y)
        {
            for (int z = -1; z < 2; ++z)
            {
                if (x != 0 || y != 0 || z != 0)
                {
                    vector<3, double> v = make_vector<double>(x, y, z);
                    CRADLE_CHECK_ALMOST_EQUAL(
                        dot(v, get_perpendicular(v)), 0.);
                    CRADLE_CHECK_ALMOST_EQUAL(
                        length(get_perpendicular(v)), 1.);
                }
            }
        }
    }
}

TEST_CASE("product_test")
{
    REQUIRE(product(make_vector<int>(2, 3, 1)) == 6);
    REQUIRE(product(make_vector<int>(2, -1, 3, 1)) == -6);

    REQUIRE(almost_equal(product(make_vector<float>(2.5, 4, 2)), 20.f));
    REQUIRE(almost_equal(product(make_vector<double>(2.5, 4)), 10.));

    // with an expression
    REQUIRE(
        product(make_vector<int>(2, -1, 3, 0) - make_vector<int>(6, 0, 0, 1))
        == -12);
}

TEST_CASE("vector_io")
{
    vector3i p = make_vector<int>(2, 0, 3);
    REQUIRE(to_string(p) == "(2, 0, 3)");
}

TEST_CASE("compute_mean_vector_test")
{
    using namespace boost::assign;

    std::vector<vector3d> vectors;
    vectors += make_vector<double>(2, 0, 3), make_vector<double>(6, 1, 7),
        make_vector<double>(0, 0, 0), make_vector<double>(1, 2, 0),
        make_vector<double>(3, 2, 1), make_vector<double>(6, 4, 1);

    REQUIRE(almost_equal(
        compute_mean(vectors, uniform_vector<3>(0.)),
        make_vector<double>(3, 1.5, 2)));
}

TEST_CASE("plane_test")
{
    plane<double> default_constructed;

    vector3d p = make_vector<double>(0, 0, 0);
    vector3d normal = make_vector<double>(1, 0, 0);

    plane<double> plane(p, normal);
    REQUIRE(plane.point == p);
    REQUIRE(plane.normal == normal);

    vector3d q = make_vector<double>(0, 0, 1);
    plane.point = q;
    REQUIRE(plane.point == q);
    REQUIRE(plane.normal == normal);

    normal = make_vector<double>(0, 1, 0);
    plane.normal = normal;
    REQUIRE(plane.point == q);
    REQUIRE(plane.normal == normal);
}

TEST_CASE("simple_box1i_test")
{
    box1i b(make_vector<int>(-1), make_vector<int>(4));

    REQUIRE(get_center(b)[0] == 1);
    REQUIRE(b.corner[0] == -1);
    REQUIRE(b.size[0] == 4);

    REQUIRE(!is_inside(b, make_vector<int>(-2)));
    REQUIRE(is_inside(b, make_vector<int>(-1)));
    REQUIRE(is_inside(b, make_vector<int>(2)));
    REQUIRE(!is_inside(b, make_vector<int>(3)));
    REQUIRE(!is_inside(b, make_vector<int>(4)));
}

TEST_CASE("simple_box1d_test")
{
    box1d b(make_vector<double>(-1), make_vector<double>(3));

    CRADLE_CHECK_ALMOST_EQUAL(get_center(b), make_vector<double>(0.5));
    CRADLE_CHECK_ALMOST_EQUAL(b.corner, make_vector<double>(-1));
    CRADLE_CHECK_ALMOST_EQUAL(b.size, make_vector<double>(3));

    REQUIRE(!is_inside(b, make_vector<double>(-2)));
    REQUIRE(is_inside(b, make_vector<double>(-1)));
    REQUIRE(is_inside(b, make_vector<double>(0)));
    REQUIRE(is_inside(b, make_vector<double>(1)));
    REQUIRE(is_inside(b, make_vector<double>(1.5)));
    REQUIRE(is_inside(b, make_vector<double>(1.9)));
    REQUIRE(!is_inside(b, make_vector<double>(2)));
    REQUIRE(!is_inside(b, make_vector<double>(4)));
}

TEST_CASE("simple_box2d_test")
{
    box2d b(make_vector<double>(-1, -1), make_vector<double>(3, 3));

    CRADLE_CHECK_ALMOST_EQUAL(area(b), 9.);

    CRADLE_CHECK_ALMOST_EQUAL(get_center(b), make_vector<double>(0.5, 0.5));
    CRADLE_CHECK_ALMOST_EQUAL(b.corner, make_vector<double>(-1, -1));
    CRADLE_CHECK_ALMOST_EQUAL(b.size, make_vector<double>(3, 3));

    REQUIRE(!is_inside(b, make_vector<double>(-2, -2)));
    REQUIRE(!is_inside(b, make_vector<double>(-2, 0)));
    REQUIRE(!is_inside(b, make_vector<double>(0, 4)));
    REQUIRE(!is_inside(b, make_vector<double>(0, 2)));
    REQUIRE(is_inside(b, make_vector<double>(-1, -1)));
    REQUIRE(is_inside(b, make_vector<double>(0, 1.9)));
    REQUIRE(is_inside(b, make_vector<double>(0, 0)));
    REQUIRE(is_inside(b, make_vector<double>(1.5, 1.5)));
    REQUIRE(is_inside(b, make_vector<double>(0, 1)));
}

TEST_CASE("box_slicing_test")
{
    REQUIRE(
        slice(
            box3d(make_vector<double>(0, 2, 1), make_vector<double>(4, 3, 5)),
            0)
        == box2d(make_vector<double>(2, 1), make_vector<double>(3, 5)));
    REQUIRE(
        slice(
            box3d(make_vector<double>(0, 2, 1), make_vector<double>(4, 3, 5)),
            1)
        == box2d(make_vector<double>(0, 1), make_vector<double>(4, 5)));
    REQUIRE(
        slice(
            box3d(make_vector<double>(0, 2, 1), make_vector<double>(4, 3, 5)),
            2)
        == box2d(make_vector<double>(0, 2), make_vector<double>(4, 3)));

    REQUIRE(
        slice(box2i(make_vector<int>(0, 2), make_vector<int>(4, 3)), 0)
        == box1i(make_vector<int>(2), make_vector<int>(3)));
    REQUIRE(
        slice(box2i(make_vector<int>(0, 2), make_vector<int>(4, 3)), 1)
        == box1i(make_vector<int>(0), make_vector<int>(4)));
}

TEST_CASE("add_box_border_test")
{
    REQUIRE(
        add_border(
            box3i(make_vector<int>(0, 2, 1), make_vector<int>(4, 3, 5)), 2)
        == box3i(make_vector<int>(-2, 0, -1), make_vector<int>(8, 7, 9)));

    REQUIRE(
        add_border(
            box3i(make_vector<int>(0, 2, 1), make_vector<int>(4, 3, 5)),
            make_vector<int>(2, 1, 0))
        == box3i(make_vector<int>(-2, 1, 1), make_vector<int>(8, 5, 5)));
}

TEST_CASE("simple_vector_test")
{
    circle<double> c(make_vector<double>(0, 0), 1);
    CRADLE_CHECK_ALMOST_EQUAL(area(c), pi);
    REQUIRE(!is_inside(c, make_vector<double>(0, 2)));
    REQUIRE(!is_inside(c, make_vector<double>(2, 0)));
    REQUIRE(!is_inside(c, make_vector<double>(1.1, 0)));
    REQUIRE(!is_inside(c, make_vector<double>(0.9, 0.9)));
    REQUIRE(!is_inside(c, make_vector<double>(0, -1.1)));
    REQUIRE(is_inside(c, make_vector<double>(0.9, 0)));
    REQUIRE(is_inside(c, make_vector<double>(0, 0)));
    REQUIRE(is_inside(c, make_vector<double>(0, -0.9)));
    REQUIRE(is_inside(c, make_vector<double>(-0.7, 0.7)));
    REQUIRE(is_inside(c, make_vector<double>(0.3, 0.5)));
}

TEST_CASE("off_center_test")
{
    circle<double> c(make_vector<double>(4, 1), 2);
    CRADLE_CHECK_ALMOST_EQUAL(area(c), 4 * pi);
    REQUIRE(!is_inside(c, make_vector<double>(0, 0)));
    REQUIRE(!is_inside(c, make_vector<double>(1.9, 1)));
    REQUIRE(!is_inside(c, make_vector<double>(6.1, 1)));
    REQUIRE(!is_inside(c, make_vector<double>(4, 3.1)));
    REQUIRE(!is_inside(c, make_vector<double>(4, -1.1)));
    REQUIRE(is_inside(c, make_vector<double>(4, 1)));
    REQUIRE(is_inside(c, make_vector<double>(2.6, 2.4)));
}

TEST_CASE("by_value_test")
{
    vector2d p0 = make_vector<double>(0, 1), p1 = make_vector<double>(4, 4);
    line_segment<2, double> segment(p0, p1);

    REQUIRE(segment[0] == p0);
    REQUIRE(segment[1] == p1);
    CRADLE_CHECK_ALMOST_EQUAL(length(segment), 5.);
}

TEST_CASE("identity_matrix_test")
{
    REQUIRE(
        (identity_matrix<4, double>())
        == make_matrix<double>(
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

    REQUIRE(
        (identity_matrix<3, double>())
        == make_matrix<double>(1, 0, 0, 0, 1, 0, 0, 0, 1));
}

TEST_CASE("matrix_operations_test")
{
    matrix<3, 3, double> m, i = identity_matrix<3, double>();

    m = i - 2 * i;
    REQUIRE(m == make_matrix<double>(-1, 0, 0, 0, -1, 0, 0, 0, -1));

    m *= 2;
    REQUIRE(m == make_matrix<double>(-2, 0, 0, 0, -2, 0, 0, 0, -2));

    m = i;
    m /= 2;
    REQUIRE(m == make_matrix<double>(0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5));

    m += i * 3;
    REQUIRE(m == make_matrix<double>(3.5, 0, 0, 0, 3.5, 0, 0, 0, 3.5));

    m -= 2 * i;
    REQUIRE(m == make_matrix<double>(1.5, 0, 0, 0, 1.5, 0, 0, 0, 1.5));

    REQUIRE(m == m);
    REQUIRE(m != i);
}

TEST_CASE("matrix_conversion_test")
{
    matrix<3, 3, double> m(identity_matrix<3, float>());
}

TEST_CASE("matrix_inverse3_test")
{
    matrix<4, 4, double> m
        = translation(make_vector<double>(4, 3, 7))
          * scaling_transformation(make_vector<double>(.1, 2, 1.2))
          * rotation_about_x(angle<double, degrees>(90));

    matrix<4, 4, double> inv_m = inverse(m);

    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(
            inv_m, transform_point(m, make_vector<double>(0, 0, 0))),
        make_vector<double>(0, 0, 0));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(
            inv_m, transform_point(m, make_vector<double>(2, 1, 7))),
        make_vector<double>(2, 1, 7));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(
            inv_m, transform_point(m, make_vector<double>(1, 0, 17))),
        make_vector<double>(1, 0, 17));
}

TEST_CASE("matrix_inverse2_test")
{
    matrix<3, 3, double> m
        = translation(make_vector<double>(3, 7))
          * scaling_transformation(make_vector<double>(.1, 1.2))
          * rotation(angle<double, degrees>(90));

    matrix<3, 3, double> inv_m = inverse(m);

    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(0, 0))),
        make_vector<double>(0, 0));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(1, 7))),
        make_vector<double>(1, 7));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(0, 17))),
        make_vector<double>(0, 17));
}

TEST_CASE("matrix_inverse1_test")
{
    matrix<2, 2, double> m = translation(make_vector<double>(1))
                             * scaling_transformation(make_vector<double>(.1));

    matrix<2, 2, double> inv_m = inverse(m);

    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(0))),
        make_vector<double>(0));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(7))),
        make_vector<double>(7));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(17))),
        make_vector<double>(17));
    CRADLE_CHECK_ALMOST_EQUAL(
        transform_point(inv_m, transform_point(m, make_vector<double>(1))),
        make_vector<double>(1));
}
