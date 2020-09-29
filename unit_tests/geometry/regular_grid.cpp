#include <cradle/geometry/grid_points.hpp>
#include <cradle/geometry/regular_grid.hpp>

#include <boost/assign/std/vector.hpp>

#include <cradle/test.hpp>

using namespace cradle;
using namespace boost::assign;

TEST_CASE("regular_grid_test")
{
    regular_grid<1, float> default_constructed;

    vector2d p0 = make_vector<double>(0, 0);
    vector2d spacing = make_vector<double>(1, 0.5);
    vector2u n_points = make_vector<unsigned>(2, 3);

    regular_grid<2, double> grid(p0, spacing, n_points);
    REQUIRE(grid.p0 == p0);
    REQUIRE(grid.spacing == spacing);
    REQUIRE(grid.n_points == n_points);

    std::vector<vector2d> correct_points;
    correct_points += make_vector<double>(0, 0), make_vector<double>(1, 0),
        make_vector<double>(0, 0.5), make_vector<double>(1, 0.5),
        make_vector<double>(0, 1), make_vector<double>(1, 1);
    CRADLE_CHECK_RANGES_ALMOST_EQUAL(
        make_grid_point_list(grid), correct_points);
}

TEST_CASE("grid_bounding_box_test")
{
    vector2d p0 = make_vector<double>(-1, 0);
    vector2d spacing = make_vector<double>(1, 0.5);
    vector2u n_points = make_vector<unsigned>(2, 3);

    regular_grid<2, double> grid(p0, spacing, n_points);

    REQUIRE(
        bounding_box(grid)
        == box2d(make_vector<double>(-1, 0), make_vector<double>(1, 1)));
}

vector3d
get_point_at_index(regular_grid<3, double> const& grid, size_t index)
{
    size_t x_index = index % grid.n_points[0];
    size_t y_index = (index / grid.n_points[0]) % grid.n_points[1];
    size_t z_index = (index / grid.n_points[0]) / grid.n_points[1];
    return make_vector(
        grid.p0[0] + x_index * grid.spacing[0],
        grid.p0[1] + y_index * grid.spacing[1],
        grid.p0[2] + z_index * grid.spacing[2]);
}
