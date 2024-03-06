#include <astroid/common.hpp>
#include <astroid/imaging/image.hpp>
#include <astroid/imaging/merge_slices.hpp>
#include <astroid/imaging/statistics.hpp>
#include <astroid/io/pxio_io.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("along_x_test")
{
    unsigned const s = 3, n_slices = 7;
    std::vector<image_slice<2, astroid::uint8_t, shared>> slices(n_slices);
    double slice_locations[n_slices] = {0, 2, 4, 6, 8, 10, 12};

    for (int i = 0; i < n_slices; ++i)
    {
        image<2, astroid::uint8_t, unique> tmp;
        create_image(tmp, make_vector(s, s));
        fill_pixels(tmp, astroid::uint8_t(i));
        slices[i].content = share(tmp);
        slices[i].position = slice_locations[i];
        set_spatial_mapping(
            slices[i].content,
            make_vector<double>(1, 0),
            make_vector<double>(3, 1));
        set_value_mapping(slices[i].content, 1, 1, no_units);
        slices[i].axis = 0;
    }

    image<3, astroid::uint8_t, shared> merged = merge_slices(slices);

    REQUIRE(merged.size == make_vector(n_slices, s, s));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(0, 0, 0)),
        make_vector<double>(-1, 1, 0));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(1, 1, 1)),
        make_vector<double>(1, 4, 1));
    REQUIRE(merged.value_mapping == linear_function<double>(1, 1));
}

TEST_CASE("along_y_test")
{
    unsigned const s = 3, n_slices = 7;
    std::vector<image_slice<2, astroid::uint8_t, shared>> slices(n_slices);
    double slice_locations[n_slices] = {0, 2, 4, 6, 8, 10, 12};

    for (int i = 0; i < n_slices; ++i)
    {
        image<2, astroid::uint8_t, unique> tmp;
        create_image(tmp, make_vector(s, s));
        fill_pixels(tmp, astroid::uint8_t(i));
        slices[i].content = share(tmp);
        slices[i].position = slice_locations[i];
        set_spatial_mapping(
            slices[i].content,
            make_vector<double>(1, 0),
            make_vector<double>(3, 1));
        set_value_mapping(slices[i].content, 1, 1, no_units);
        slices[i].axis = 1;
    }

    image<3, astroid::uint8_t, shared> merged = merge_slices(slices);

    REQUIRE(merged.size == make_vector(s, n_slices, s));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(0, 0, 0)),
        make_vector<double>(1, -1, 0));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(1, 1, 1)),
        make_vector<double>(4, 1, 1));
    REQUIRE(merged.value_mapping == linear_function<double>(1, 1));
}

TEST_CASE("along_z_test")
{
    unsigned const s = 3, n_slices = 7;
    std::vector<image_slice<2, astroid::uint8_t, shared>> slices(n_slices);
    double slice_locations[n_slices] = {0, 2, 4, 6, 8, 10, 12};

    for (int i = 0; i < n_slices; ++i)
    {
        image<2, astroid::uint8_t, unique> tmp;
        create_image(tmp, make_vector(s, s));
        fill_pixels(tmp, astroid::uint8_t(i));
        slices[i].content = share(tmp);
        slices[i].position = slice_locations[i];
        set_spatial_mapping(
            slices[i].content,
            make_vector<double>(1, 0),
            make_vector<double>(3, 1));
        set_value_mapping(slices[i].content, 1, 1, no_units);
        slices[i].axis = 2;
    }

    image<3, astroid::uint8_t, shared> merged = merge_slices(slices);

    REQUIRE(merged.size == make_vector(s, s, n_slices));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(0, 0, 0)),
        make_vector<double>(1, 0, -1));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            get_spatial_mapping(merged), make_vector<double>(1, 1, 1)),
        make_vector<double>(4, 1, 1));
    REQUIRE(merged.value_mapping == linear_function<double>(1, 1));
}
