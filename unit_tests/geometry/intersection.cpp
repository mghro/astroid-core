#include <astroid/geometry/intersection.hpp>

#include <astroid/test.hpp>

using namespace astroid;

template<typename T>
void
assert_intersection(
    astroid::plane<T> const& plane,
    line_segment<3, T> const& segment,
    vector<3, T> const& correct_p)
{
    optional<vector<3, T>> p = intersection(plane, segment);
    REQUIRE(p);
    ASTROID_CHECK_ALMOST_EQUAL(*p, correct_p);
}

template<typename T>
void
assert_no_intersection(
    astroid::plane<T> const& plane, line_segment<3, T> const& segment)
{
    optional<vector<3, T>> p = intersection(plane, segment);
    REQUIRE(!p);
}

TEST_CASE("plane_line_segment_intersection_test")
{
    assert_intersection(
        plane<double>(
            make_vector<double>(0, 1, 0), make_vector<double>(0, 1, 0)),
        make_line_segment(
            make_vector<double>(0, 0, 0), make_vector<double>(0, 3, 0)),
        make_vector<double>(0, 1, 0));
    assert_intersection(
        plane<double>(
            make_vector<double>(0, 1, 0), make_vector<double>(0, 1, 0)),
        make_line_segment(
            make_vector<double>(0, 0, 0), make_vector<double>(3, 3, 0)),
        make_vector<double>(1, 1, 0));
    assert_intersection(
        plane<double>(
            make_vector<double>(0, 1, 0), unit(make_vector<double>(1, 1, 0))),
        make_line_segment(
            make_vector<double>(3, 2, 0), make_vector<double>(0, -1, 0)),
        make_vector<double>(1, 0, 0));
    assert_intersection(
        plane<double>(
            make_vector<double>(-1, 1, 0), make_vector<double>(0, 1, 0)),
        make_line_segment(
            make_vector<double>(0, 0, 0), make_vector<double>(3, 3, 0)),
        make_vector<double>(1, 1, 0));
    assert_no_intersection(
        plane<double>(
            make_vector<double>(-1, 1, 0), make_vector<double>(0, 1, 0)),
        make_line_segment(
            make_vector<double>(0, 0, 0), make_vector<double>(-3, -3, 0)));
}

template<typename T>
bool
almost_equal(vector<3, T> const& a, vector<3, T> const& b)
{
    return almost_equal<T>(length(a - b), 0);
}

template<typename T>
void
assert_intersection(
    astroid::plane<T> const& plane,
    triangle<3, T> const& tri,
    line_segment<3, T> const& correct_segment)
{
    optional<line_segment<3, T>> segment = intersection(plane, tri);
    REQUIRE(segment);
    REQUIRE(
        (almost_equal((*segment)[0], correct_segment[0])
             && almost_equal((*segment)[1], correct_segment[1])
         || almost_equal((*segment)[0], correct_segment[1])
                && almost_equal((*segment)[1], correct_segment[0])));
}

template<typename T>
void
assert_no_intersection(
    astroid::plane<T> const& plane, triangle<3, T> const& tri)
{
    optional<line_segment<3, T>> segment = intersection(plane, tri);
    REQUIRE(!segment);
}

TEST_CASE("plane_triangle_intersection_test")
{
    assert_intersection(
        plane<double>(
            make_vector<double>(0, 1, 0), make_vector<double>(0, 1, 0)),
        make_triangle(
            make_vector<double>(-2, 0, 0),
            make_vector<double>(2, 0, 0),
            make_vector<double>(0, 2, 0)),
        make_line_segment(
            make_vector<double>(-1, 1, 0), make_vector<double>(1, 1, 0)));
}

template<unsigned N, typename T>
void
assert_intersection(
    astroid::ray<N, T> const& ray,
    astroid::box<N, T> const& box,
    unsigned n_intersections,
    T entrance = 0,
    T exit = 0)
{
    auto result = intersection(ray, box);
    REQUIRE(result.n_intersections == n_intersections);
    if (n_intersections > 0)
    {
        REQUIRE(result.entrance_distance == entrance);
        REQUIRE(result.exit_distance == exit);
    }
}

TEST_CASE("ray_box_intersection_test")
{
    assert_intersection(
        ray2d(make_vector<double>(-4, 0), make_vector<double>(1, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        2,
        2.,
        6.);
    assert_intersection(
        ray2d(make_vector<double>(-4, 0), make_vector<double>(-1, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        0);
    assert_intersection(
        ray2d(make_vector<double>(0, 0), make_vector<double>(-1, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        1,
        0.,
        2.);
    assert_intersection(
        ray2d(make_vector<double>(0, -4.5), unit(make_vector<double>(1, 1))),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        0);
    assert_intersection(
        ray2d(make_vector<double>(-4, -4), unit(make_vector<double>(1, 1))),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        2,
        2 * sqrt(2.),
        6 * sqrt(2.));
    assert_intersection(
        ray2d(make_vector<double>(-4, 0), unit(make_vector<double>(1, 1))),
        box2d(make_vector<double>(-2, 0), make_vector<double>(4, 6)),
        2,
        2 * sqrt(2.),
        6 * sqrt(2.));
}

template<unsigned N, typename T>
void
assert_intersection(
    line_segment<N, T> const& segment,
    astroid::box<N, T> const& box,
    line_segment<N, T> const& correct_result)
{
    optional<line_segment<N, T>> result = intersection(segment, box);
    REQUIRE(result);
    ASTROID_CHECK_ALMOST_EQUAL(correct_result[0], (*result)[0]);
    ASTROID_CHECK_ALMOST_EQUAL(correct_result[1], (*result)[1]);
}

template<unsigned N, typename T>
void
assert_no_intersection(
    line_segment<N, T> const& segment, astroid::box<N, T> const& box)
{
    optional<line_segment<N, T>> result = intersection(segment, box);
    REQUIRE(!result);
}

TEST_CASE("segment_box_intersection")
{
    assert_intersection(
        make_line_segment(
            make_vector<double>(-4, 0), make_vector<double>(4, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        make_line_segment(
            make_vector<double>(-2, 0), make_vector<double>(2, 0)));
    assert_no_intersection(
        make_line_segment(
            make_vector<double>(-4, 0), make_vector<double>(-12, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)));
    assert_intersection(
        make_line_segment(
            make_vector<double>(0, 0), make_vector<double>(-4, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        make_line_segment(
            make_vector<double>(0, 0), make_vector<double>(-2, 0)));
    assert_no_intersection(
        make_line_segment(
            make_vector<double>(0, -4.5), make_vector<double>(-4.5, 0)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)));
    assert_intersection(
        make_line_segment(
            make_vector<double>(-4, -4), make_vector<double>(4, 4)),
        box2d(make_vector<double>(-2, -2), make_vector<double>(4, 4)),
        make_line_segment(
            make_vector<double>(-2, -2), make_vector<double>(2, 2)));
    assert_intersection(
        make_line_segment(
            make_vector<double>(-4, 0), make_vector<double>(4, 8)),
        box2d(make_vector<double>(-2, 0), make_vector<double>(4, 6)),
        make_line_segment(
            make_vector<double>(-2, 2), make_vector<double>(2, 6)));
    assert_intersection(
        make_line_segment(
            make_vector<double>(-4, 0), make_vector<double>(0, 4)),
        box2d(make_vector<double>(-2, 0), make_vector<double>(4, 6)),
        make_line_segment(
            make_vector<double>(-2, 2), make_vector<double>(0, 4)));
}
