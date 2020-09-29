#include <cradle/anonymous.hpp>
#include <cradle/geometry/common.hpp>
#include <cradle/math/interpolate.hpp>

#include <vector>

#include <cradle/test.hpp>

using namespace cradle;

TEST_CASE("compute_interpolation_grid_test0")
{
    std::vector<double> positions
        = anonymous<std::vector<double>>(0, 1, 3, 4, 5, 7, 8, 9);
    auto grid = compute_interpolation_grid(positions);
    CRADLE_CHECK_ALMOST_EQUAL(grid.p0[0], 0.);
    CRADLE_CHECK_ALMOST_EQUAL(grid.spacing[0], 1.);
    REQUIRE(grid.n_points[0] == 10);
}

TEST_CASE("compute_interpolation_grid_test1")
{
    std::vector<double> positions = anonymous<std::vector<double>>(
        0.2, 1.2, 3.2, 4.2, 5.2, 7.2, 8.2, 9.2);
    auto grid = compute_interpolation_grid(positions);
    CRADLE_CHECK_ALMOST_EQUAL(grid.p0[0], 0.2);
    CRADLE_CHECK_ALMOST_EQUAL(grid.spacing[0], 1.);
    REQUIRE(grid.n_points[0] == 10);
}

TEST_CASE("compute_interpolation_grid_test2")
{
    std::vector<double> positions
        = anonymous<std::vector<double>>(-4, -3, 1, 3, 4, 5, 7, 8, 9);
    auto grid = compute_interpolation_grid(positions);
    CRADLE_CHECK_ALMOST_EQUAL(grid.p0[0], -4.);
    CRADLE_CHECK_ALMOST_EQUAL(grid.spacing[0], 1.);
    REQUIRE(grid.n_points[0] == 14);
}

TEST_CASE("interpolate_test")
{
    std::vector<double> source_positions
        = anonymous<std::vector<double>>(0, 1, 3, 4, 5, 6.5, 8, 9);
    regular_grid<1, double> grid(
        make_vector(0.), make_vector(1.), make_vector(10u));

    std::vector<double> source_values
        = anonymous<std::vector<double>>(3, 0, 1, 2, 7, 0, -1, 2);
    std::vector<double> interpolated_values;

    interpolate(&interpolated_values, grid, source_values, source_positions);

    std::vector<double> correct_values = anonymous<std::vector<double>>(
        3, 0, 0.5, 1, 2, 7, 7. / 3, -1. / 3, -1, 2);

    CRADLE_CHECK_RANGES_ALMOST_EQUAL(interpolated_values, correct_values);
}

TEST_CASE("simple_interpolated_function_test")
{
    std::vector<double> samples
        = anonymous<std::vector<double>>(4, 2, 1, 7, 6, 4);
    interpolated_function f;
    initialize(f, 1, 4, samples, outside_domain_policy::ALWAYS_ZERO);

    REQUIRE(get_sample_grid(f).p0[0] == 1);
    REQUIRE(get_sample_grid(f).spacing[0] == 4);
    REQUIRE(get_sample_grid(f).n_points[0] == samples.size());

    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -1), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 0), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -0.1), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1), 4.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1.5), 3.75);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 2), 3.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 5), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 15), 6.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 20), 4.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 21), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 23), 0.);
}

TEST_CASE("regularly_sampled_function_test")
{
    regularly_sampled_function data_set(
        1,
        4,
        anonymous<std::vector<double>>(4, 2, 1, 7, 6, 4),
        outside_domain_policy::ALWAYS_ZERO);

    interpolated_function f;
    initialize(f, data_set);

    REQUIRE(get_sample_grid(f).p0[0] == 1);
    REQUIRE(get_sample_grid(f).spacing[0] == 4);
    REQUIRE(get_sample_grid(f).n_points[0] == 6);

    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -1), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 0), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -0.1), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1), 4.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1.5), 3.75);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 2), 3.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 5), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 15), 6.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 20), 4.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 21), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 23), 0.);
}

TEST_CASE("irregularly_sampled_function_test")
{
    irregularly_sampled_function data_set(
        anonymous<std::vector<cradle::vector<2, double>>>(
            cradle::make_vector<double>(0, 4),
            cradle::make_vector<double>(1, 2),
            cradle::make_vector<double>(3, 1),
            cradle::make_vector<double>(4, 3),
            cradle::make_vector<double>(5, 3),
            cradle::make_vector<double>(7, 1),
            cradle::make_vector<double>(8, 1),
            cradle::make_vector<double>(9, 2)),
        outside_domain_policy::ALWAYS_ZERO);

    interpolated_function f;
    initialize(f, data_set);

    CRADLE_CHECK_ALMOST_EQUAL(get_sample_grid(f).p0[0], 0.);
    CRADLE_CHECK_ALMOST_EQUAL(get_sample_grid(f).spacing[0], 1.);
    REQUIRE(get_sample_grid(f).n_points[0] == 10);

    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -0.5), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 0), 4.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 0.5), 3.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1.5), 1.75);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 2), 1.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 3.5), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 6), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 7), 1.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 9.1), 0.);
}

TEST_CASE("interpolated_function_extend_with_copies_test")
{
    std::vector<double> samples
        = anonymous<std::vector<double>>(6, 2, 1, 7, 6, 4);
    interpolated_function f;
    initialize(f, 1, 4, samples, outside_domain_policy::EXTEND_WITH_COPIES);

    REQUIRE(get_sample_grid(f).p0[0] == 1);
    REQUIRE(get_sample_grid(f).spacing[0] == 4);
    REQUIRE(get_sample_grid(f).n_points[0] == samples.size());

    CRADLE_CHECK_ALMOST_EQUAL(sample(f, -1), 6.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 0), 6.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 1), 6.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 20), 4.5);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 22), 4.);
    CRADLE_CHECK_ALMOST_EQUAL(sample(f, 23), 4.);
}
