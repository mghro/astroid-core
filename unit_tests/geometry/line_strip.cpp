#include <astroid/geometry/line_strip.hpp>
#include <boost/assign/std/vector.hpp>

#include <astroid/test.hpp>

double const tolerance = 0.00001;

using namespace boost::assign;
using namespace astroid;

TEST_CASE("actual_poly_test")
{
    std::vector<vertex2> vertices;
    vertices += make_vector<double>(-1, 0), make_vector<double>(1, 0),
        make_vector<double>(0, 1);
    polygon2 original;
    initialize(&original.vertices, vertices);

    std::vector<line_strip> strips;
    std::vector<line_segment<2, double>> segments;

    for (polygon2_edge_view ev(original); !ev.done(); ev.advance())
        segments.push_back(make_line_segment(ev.p0(), ev.p1()));

    strips = connect_line_segments(segments, tolerance);

    REQUIRE(is_polygon(strips, tolerance));
    REQUIRE(as_polygon(strips, tolerance));

    optional<polygon2> poly;
    as_polygon(&poly, strips, tolerance);
    REQUIRE(poly);
    REQUIRE(almost_equal(original, poly.value()));
}

TEST_CASE("not_connected_test")
{
    std::vector<vertex2> vertices;
    vertices += make_vector<double>(-1, 0), make_vector<double>(1, 0),
        make_vector<double>(0, 1);
    polygon2 original;
    initialize(&original.vertices, vertices);

    std::vector<line_strip> strips;
    std::vector<line_segment<2, double>> segments;

    polygon2_edge_view ev(original);
    ev.advance();
    for (; !ev.done(); ev.advance())
        segments.push_back(make_line_segment(ev.p0(), ev.p1()));

    strips = connect_line_segments(segments, tolerance);

    REQUIRE(!is_polygon(strips, tolerance));
    REQUIRE(!as_polygon(strips, tolerance));

    optional<polygon2> poly;
    as_polygon(&poly, strips, tolerance);
    REQUIRE(!poly);
}

TEST_CASE("extraneous_segments_test")
{
    std::vector<vertex2> vertices;
    vertices += make_vector<double>(-1, 0), make_vector<double>(1, 0),
        make_vector<double>(0, 1);
    polygon2 original;
    initialize(&original.vertices, vertices);

    std::vector<line_strip> strips;
    std::vector<line_segment<2, double>> segments;

    for (polygon2_edge_view ev(original); !ev.done(); ev.advance())
        segments.push_back(make_line_segment(ev.p0(), ev.p1()));

    segments.push_back(make_line_segment(
        make_vector<double>(2, 0), make_vector<double>(2, 1)));

    strips = connect_line_segments(segments, tolerance);

    REQUIRE(!is_polygon(strips, tolerance));
    REQUIRE(!as_polygon(strips, tolerance));

    optional<polygon2> poly;
    as_polygon(&poly, strips, tolerance);
    REQUIRE(!poly);
}

TEST_CASE("multiple_polys_test")
{
    polygon2 original;
    {
        std::vector<vertex2> vertices;
        vertices += make_vector<double>(-1, 0), make_vector<double>(1, 0),
            make_vector<double>(0, 1);
        initialize(&original.vertices, vertices);
    }

    std::vector<line_strip> strips;
    std::vector<line_segment<2, double>> segments;

    for (polygon2_edge_view ev(original); !ev.done(); ev.advance())
        segments.push_back(make_line_segment(ev.p0(), ev.p1()));

    strips = connect_line_segments(segments, tolerance);

    polygon2 other;
    {
        std::vector<vertex2> vertices;
        vertices += make_vector<double>(2, 0), make_vector<double>(3, 0),
            make_vector<double>(0, 1);
        initialize(&other.vertices, vertices);
    }

    for (polygon2_edge_view ev(other); !ev.done(); ev.advance())
        segments.push_back(make_line_segment(ev.p0(), ev.p1()));

    strips = connect_line_segments(segments, tolerance);

    REQUIRE(!is_polygon(strips, tolerance));
    REQUIRE(!as_polygon(strips, tolerance));

    optional<polygon2> poly;
    as_polygon(&poly, strips, tolerance);
    REQUIRE(!poly);
}
