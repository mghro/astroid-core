#include <cradle/anonymous.hpp>
#include <cradle/math/common.hpp>

#include <cradle/test.hpp>

using namespace cradle;

TEST_CASE("compute_mean_integer_test")
{
    REQUIRE(
        compute_mean<float>(anonymous<std::vector<int>>(4, 2, 7, 3), 0) == 4);
}

TEST_CASE("compute_mean_float_test")
{
    REQUIRE(
        compute_mean<float>(anonymous<std::vector<float>>(4, 2, 1, 3), 0)
        == 2.5);
}

TEST_CASE("clamp_test")
{
    REQUIRE(cradle::clamp(-0.5, 0., 1.) == 0);
    REQUIRE(cradle::clamp(0.5, 0., 1.) == 0.5);
    REQUIRE(cradle::clamp(1.5, 0., 1.) == 1);
}

TEST_CASE("equality_test")
{
    REQUIRE(almost_equal(1., 1.01, 0.1));
    REQUIRE(!almost_equal(1., 1.2, 0.1));

    REQUIRE(almost_equal(1., 1 + default_equality_tolerance<double>() / 2));
    REQUIRE(!almost_equal(1., 1 + default_equality_tolerance<double>() * 2));
    REQUIRE(!almost_equal(1., 2.));

    REQUIRE(almost_equal(1.f, 1 + default_equality_tolerance<float>() / 2));
    REQUIRE(!almost_equal(1.f, 1 + default_equality_tolerance<float>() * 2));
}

TEST_CASE("is_power_of_two_test")
{
    REQUIRE(!is_power_of_two(0));
    REQUIRE(is_power_of_two(1));
    REQUIRE(is_power_of_two(2));
    REQUIRE(!is_power_of_two(3));
    REQUIRE(is_power_of_two(4));
    REQUIRE(!is_power_of_two(5));
    REQUIRE(!is_power_of_two(6));
    REQUIRE(!is_power_of_two(7));
    REQUIRE(is_power_of_two(8));
    REQUIRE(!is_power_of_two(9));
    REQUIRE(!is_power_of_two(10));
    REQUIRE(!is_power_of_two(15));
    REQUIRE(is_power_of_two(16));
    REQUIRE(is_power_of_two(64));
    REQUIRE(!is_power_of_two(65));
}

TEST_CASE("mod_test")
{
    REQUIRE(nonnegative_mod(-3, 4) == 1);
    REQUIRE(nonnegative_mod(11, 4) == 3);
    REQUIRE(nonnegative_mod(12, 4) == 0);
    REQUIRE(nonnegative_mod(2, 4) == 2);
    REQUIRE(nonnegative_mod(-1, 12) == 11);
    REQUIRE(nonnegative_mod(-13, 12) == 11);
    REQUIRE(nonnegative_mod(-12, 12) == 0);
    REQUIRE(nonnegative_mod(0, 12) == 0);
    REQUIRE(nonnegative_mod(4, 12) == 4);
    REQUIRE(nonnegative_mod(13, 12) == 1);
}

TEST_CASE("quadratic_function_test")
{
    quadratic_function<double> f(1, 3, 2);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, -2), 0.);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 1.5), 8.75);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 0), 2.);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 4), 30.);

    quadratic_function<double> g(2, 3, 0);
    REQUIRE(f != g);

    REQUIRE(f == f);

    // This is disabled because the prettier output was disabled when all
    // CRADLE types got automatically generated ostream operators.
    // REQUIRE(to_string(g) ==  "f(x) = 2x^2 + 3x + 0");
}

TEST_CASE("linear_function_test")
{
    linear_function<double> f(1, 3);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, -2), -5.);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 1.5), 5.5);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 2), 7.);

    f = inverse(f);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, -5), -2.);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 5.5), 1.5);
    CRADLE_CHECK_ALMOST_EQUAL(apply(f, 7), 2.);

    linear_function<double> g(2, 3);
    REQUIRE(f != g);

    REQUIRE(f == f);

    // This is disabled because the prettier output was disabled when all
    // CRADLE types got automatically generated ostream operators.
    // REQUIRE(to_string(g) ==  "f(x) = 3x + 2");
}
