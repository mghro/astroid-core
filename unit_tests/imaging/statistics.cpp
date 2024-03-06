#include <astroid/common.hpp>
#include <astroid/imaging/image.hpp>
#include <astroid/imaging/statistics.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("uint8_test")
{
    unsigned const s = 3;
    int initial_n = 61;
    image<3, astroid::uint8_t, unique> img;
    create_image(img, make_vector(s, s, s));
    img.value_mapping = linear_function<double>(1, 2);
    sequential_fill(img, initial_n, 1);

    auto min_max = raw_image_min_max(img);
    REQUIRE(min_max);
    REQUIRE(int(min_max.value().min) == initial_n);
    REQUIRE(int(min_max.value().max) == initial_n + 26);
    sequential_fill(img, initial_n, -1);

    min_max = raw_image_min_max(img);
    REQUIRE(min_max);
    REQUIRE(int(min_max.value().min) == initial_n - 26);
    REQUIRE(int(min_max.value().max) == initial_n);

    auto mapped_min_max = image_min_max(img);
    ASTROID_CHECK_ALMOST_EQUAL(
        mapped_min_max.value().min, (initial_n - 26) * 2 + 1.);
    ASTROID_CHECK_ALMOST_EQUAL(mapped_min_max.value().max, initial_n * 2 + 1.);

    auto stats = raw_image_statistics(img);
    REQUIRE(stats.min);
    REQUIRE(int(stats.min.value()) == initial_n - 26);
    REQUIRE(stats.max);
    REQUIRE(int(stats.max.value()) == initial_n);
    REQUIRE(stats.mean);
    REQUIRE(int(stats.mean.value()) == initial_n - 13);

    auto mapped_stats = image_statistics(img);
    REQUIRE(mapped_stats.min);
    ASTROID_CHECK_ALMOST_EQUAL(
        mapped_stats.min.value(), (initial_n - 26) * 2 + 1.);
    REQUIRE(mapped_stats.max);
    ASTROID_CHECK_ALMOST_EQUAL(mapped_stats.max.value(), initial_n * 2 + 1.);
    REQUIRE(mapped_stats.mean);
    ASTROID_CHECK_ALMOST_EQUAL(
        mapped_stats.mean.value(), (initial_n - 13) * 2 + 1.);
}

TEST_CASE("float_test")
{
    float random_data[] = {
        17.0f,
        12.1f,
        43.2f,
        16.2f,
        25.0f,
        34.7f,
        71.3f,
        27.0f,
        19.1f,
    };
    image<2, float, const_view> view
        = make_const_view(random_data, make_vector<unsigned>(3, 3));

    auto min_max = raw_image_min_max(view);
    REQUIRE(min_max);
    REQUIRE(min_max.value().min == 12.1f);
    REQUIRE(min_max.value().max == 71.3f);

    auto stats = image_statistics(view);
    REQUIRE(stats.min);
    REQUIRE(stats.min.value() == 12.1f);
    REQUIRE(stats.max);
    REQUIRE(stats.max.value() == 71.3f);
    REQUIRE(stats.mean);
    ASTROID_CHECK_WITHIN_TOLERANCE(stats.mean.value(), 265.6 / 9, 0.0001);
}
