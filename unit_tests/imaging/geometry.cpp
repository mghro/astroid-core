#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/image.hpp>

#include <astroid/test.hpp>

using namespace astroid;

TEST_CASE("default_bounding_box_test")
{
    unsigned const s = 10;
    image<2, astroid::uint8_t, unique> img;
    create_image(img, make_vector(s, s));

    box2d box = get_bounding_box(img);
    ASTROID_CHECK_ALMOST_EQUAL(box.corner, make_vector<double>(0, 0));
    ASTROID_CHECK_ALMOST_EQUAL(box.size, make_vector<double>(10, 10));
}

TEST_CASE("bounding_box_test")
{
    unsigned const s = 10;
    image<2, astroid::uint8_t, unique> img;
    create_image(img, make_vector(s, s));

    set_spatial_mapping(
        img, make_vector<double>(-2, -6), make_vector<double>(1, 2));
    img.value_mapping = linear_function<double>(-2, 2);

    box2d box = get_bounding_box(img);
    ASTROID_CHECK_ALMOST_EQUAL(box.corner, make_vector<double>(-2, -6));
    ASTROID_CHECK_ALMOST_EQUAL(box.size, make_vector<double>(10, 20));
}
