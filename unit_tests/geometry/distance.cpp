#include <astroid/geometry/distance.hpp>
#include <boost/assign/std/vector.hpp>
#include <cmath>

#include <astroid/test.hpp>

using namespace boost::assign;
using namespace astroid;

TEST_CASE("point_line_segment_2d_test")
{
    line_segment<2, double> ls(
        make_vector<double>(0, 0), make_vector<double>(0, 5));
    vector2d cp;
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(1, 2), &cp), 1.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 2));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(-3, 3), &cp), 3.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 3));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(0, 7), &cp), 2.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 5));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(-3, -4), &cp), 5.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 0));

    ls = line_segment<2, double>(
        make_vector<double>(0, 0), make_vector<double>(3, 3));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(-3, -4), &cp), 5.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 0));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(3, 5), &cp), 2.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(3, 3));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(ls, make_vector<double>(0, 4), &cp), sqrt(8.));
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(2, 2));
}

TEST_CASE("point_polygon_test")
{
    polygon2 poly;
    {
        std::vector<vertex2> vertices;
        vertices += make_vector<double>(0, 0), make_vector<double>(0, 6),
            make_vector<double>(6, 6);
        initialize(&poly.vertices, vertices);
    }

    vector2d cp;
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(poly, make_vector<double>(7, 6), &cp), 1.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(6, 6));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(poly, make_vector<double>(1, 4), &cp), -1.);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 4));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(poly, make_vector<double>(-1, -1), &cp), sqrt(2.));
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(0, 0));
}

TEST_CASE("point_polyset_test")
{
    polygon2 poly, hole;
    {
        std::vector<vertex2> vertices;
        vertices += make_vector<double>(-2, -2), make_vector<double>(-2, 2),
            make_vector<double>(2, 2), make_vector<double>(2, -2);
        initialize(&poly.vertices, vertices);
    }
    {
        std::vector<vertex2> vertices;
        vertices += make_vector<double>(-1, -1), make_vector<double>(-1, 1),
            make_vector<double>(1, 1), make_vector<double>(1, -1);
        initialize(&hole.vertices, vertices);
    }
    polyset area;
    add_polygon(area, poly);
    add_hole(area, hole);

    ASTROID_CHECK_ALMOST_EQUAL(get_area(area), 12.);

    REQUIRE(!is_inside(area, make_vector<double>(-3, 3)));
    REQUIRE(!is_inside(area, make_vector<double>(0, 0)));
    REQUIRE(is_inside(area, make_vector<double>(-1.5, 1.5)));

    vector2d cp;
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(area, make_vector<double>(0.5, 0), &cp), 0.5);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(1, 0));
    ASTROID_CHECK_ALMOST_EQUAL(distance(area, make_vector<double>(0, 0)), 1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(area, make_vector<double>(1.25, 1), &cp), -0.25);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(1, 1));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(area, make_vector<double>(1.75, 1), &cp), -0.25);
    ASTROID_CHECK_ALMOST_EQUAL(cp, make_vector<double>(2, 1));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(area, make_vector<double>(2.5, 1)), 0.5);
}

TEST_CASE("point_plane_test")
{
    plane<double> plane(
        make_vector<double>(0, 0, 0), make_vector<double>(1, 0, 0));
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(plane, make_vector<double>(0, 0, 0)), 0.);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(plane, make_vector<double>(-7.1, 0, 0)), -7.1);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(plane, make_vector<double>(13, 0, 0)), 13.);

    plane.point = make_vector<double>(0, 0, 1);
    plane.normal = unit(make_vector<double>(1, 0, 1));
    double srt = std::sqrt(2.);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(plane, make_vector<double>(0, 0, 0)), -1 / srt);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance(plane, make_vector<double>(0, 0, 6)), 5 / srt);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance2(plane, make_vector<double>(0, 0, 0)), -0.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        distance2(plane, make_vector<double>(0, 0, 6)), 12.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        absolute_distance2(plane, make_vector<double>(0, 0, 0)), 0.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        absolute_distance2(plane, make_vector<double>(0, 0, 6)), 12.5);
}
