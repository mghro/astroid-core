#include <cradle/common.hpp>
#include <cradle/geometry/line_strip.hpp>
#include <cradle/geometry/polygonal.hpp>
#include <cradle/imaging/geometry.hpp>
#include <cradle/imaging/image.hpp>
#include <cradle/imaging/isolines.hpp>

#include <cradle/test.hpp>

static double const tolerance = 0.00001;

using namespace cradle;

TEST_CASE("isolines_test")
{
    unsigned const s = 3;
    cradle::uint8_t data[] = {
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
    };
    image<2, cradle::uint8_t, const_view> img
        = make_const_view(data, make_vector(s, s));
    img.value_mapping = linear_function<double>(1, 2);
    set_spatial_mapping(
        img, make_vector<double>(4, 0), make_vector<double>(3, 2));

    auto lines = compute_isolines(img, 2);
    optional<polygon2> poly
        = as_polygon(connect_line_segments(lines, tolerance), tolerance);
    REQUIRE(poly);
    CRADLE_CHECK_WITHIN_TOLERANCE(get_area(poly.get()), 3., tolerance);
    REQUIRE(is_inside(poly.get(), make_vector<double>(8.5, 3)));
}

TEST_CASE("edge_contour_test0")
{
    unsigned const s = 3;
    cradle::uint8_t data[] = {
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
    };
    image<2, cradle::uint8_t, const_view> img
        = make_const_view(data, make_vector(s, s));
    img.value_mapping = linear_function<double>(0, 1);
    set_spatial_mapping(
        img, make_vector<double>(-1, -1), make_vector<double>(2, 2));

    auto lines = compute_isolines(img, 0.5);
    optional<polygon2> poly
        = as_polygon(connect_line_segments(lines, tolerance), tolerance);
    REQUIRE(!poly);
    close_isoline_contours(lines, img, 0.5);
    poly = as_polygon(connect_line_segments(lines, tolerance), tolerance);
    REQUIRE(poly);
    CRADLE_CHECK_WITHIN_TOLERANCE(get_area(poly.get()), 16.5, tolerance);
    REQUIRE(is_inside(poly.get(), make_vector<double>(0, 1)));
    REQUIRE(is_inside(poly.get(), make_vector<double>(1, 1)));
    REQUIRE(is_inside(poly.get(), make_vector<double>(1, 3)));
    REQUIRE(is_inside(poly.get(), make_vector<double>(2, 4)));
}

TEST_CASE("edge_contour_test1")
{
    unsigned const s = 3;
    cradle::uint8_t data[] = {
        1,
        1,
        1,
        1,
        0,
        1,
        1,
        1,
        1,
    };
    image<2, cradle::uint8_t, const_view> img
        = make_const_view(data, make_vector(s, s));
    img.value_mapping = linear_function<double>(0, 1);
    set_spatial_mapping(
        img, make_vector<double>(-1, -1), make_vector<double>(2, 2));

    auto lines = compute_isolines(img, 0.5);
    optional<polygon2> poly
        = as_polygon(connect_line_segments(lines, tolerance), tolerance);
    REQUIRE(poly);
    close_isoline_contours(lines, img, 0.5);
    optional<polyset> region
        = as_polyset(connect_line_segments(lines, tolerance), tolerance);
    REQUIRE(region);
    REQUIRE(region.get().polygons.size() == 1);
    REQUIRE(region.get().holes.size() == 1);
    CRADLE_CHECK_WITHIN_TOLERANCE(get_area(region.get()), 34., tolerance);
    REQUIRE(!is_inside(region.get(), make_vector<double>(2, 2)));
    REQUIRE(is_inside(region.get(), make_vector<double>(0, 2)));
    REQUIRE(is_inside(region.get(), make_vector<double>(2, 0)));
    REQUIRE(is_inside(region.get(), make_vector<double>(4, 2)));
    REQUIRE(is_inside(region.get(), make_vector<double>(2, 4)));
    REQUIRE(is_inside(region.get(), make_vector<double>(0, 0)));
    REQUIRE(is_inside(region.get(), make_vector<double>(0, 4)));
    REQUIRE(is_inside(region.get(), make_vector<double>(4, 0)));
    REQUIRE(is_inside(region.get(), make_vector<double>(4, 4)));
    REQUIRE(is_inside(region.get(), make_vector<double>(1, 1)));
    REQUIRE(is_inside(region.get(), make_vector<double>(1, 3)));
    REQUIRE(is_inside(region.get(), make_vector<double>(3, 1)));
    REQUIRE(is_inside(region.get(), make_vector<double>(3, 3)));
}
