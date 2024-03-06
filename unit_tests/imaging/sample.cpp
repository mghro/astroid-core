#include <astroid/imaging/geometry.hpp>
#include <astroid/imaging/sample.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

template<class View>
void
do_raw_gray_text(View const& view)
{
    REQUIRE(
        int(raw_image_sample(view, make_vector<double>(-2.9, 0.1)).value())
        == 0.);
    REQUIRE(
        int(raw_image_sample(view, make_vector<double>(0, 0.9)).value())
        == 1.);
    REQUIRE(
        int(raw_image_sample(view, make_vector<double>(-2.9, 1.1)).value())
        == 3.);

    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(-2.9, 0.1))
                .value()),
        0.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(-2, 0.5))
                .value()),
        0.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(-1, 0.5))
                .value()),
        0.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(raw_interpolated_image_sample(view, make_vector<double>(-2, 1))
                   .value()),
        1.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(raw_interpolated_image_sample(view, make_vector<double>(-1, 1))
                   .value()),
        2.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(raw_interpolated_image_sample(view, make_vector<double>(2.9, 1))
                   .value()),
        3.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(2.9, 0.9))
                .value()),
        3.2);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(raw_interpolated_image_sample(view, make_vector<double>(0, 2.9))
                   .value()),
        7.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(0.1, 2.9))
                .value()),
        7.05);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            raw_interpolated_image_sample(view, make_vector<double>(2.9, 2.9))
                .value()),
        8.);

    REQUIRE(!raw_image_sample_over_box(
        view,
        box2d(make_vector<double>(-4, 0), make_vector<double>(0.1, 0.1))));
    REQUIRE(!raw_image_sample_over_box(
        view,
        box2d(make_vector<double>(3, 0), make_vector<double>(0.1, 0.1))));
    REQUIRE(!raw_image_sample_over_box(
        view,
        box2d(make_vector<double>(0, -1), make_vector<double>(0.1, 0.1))));
    REQUIRE(!raw_image_sample_over_box(
        view,
        box2d(make_vector<double>(0, 3), make_vector<double>(0.1, 0.1))));

    ASTROID_CHECK_ALMOST_EQUAL(
        double(raw_image_sample_over_box(
                   view,
                   box2d(
                       make_vector<double>(-2, 0.1),
                       make_vector<double>(0.1, 0.1)))
                   .value()),
        0.);
}

template<class View>
void
do_gray_test(View const& view)
{
    REQUIRE(!image_sample(view, make_vector<double>(-3.1, 0)));
    REQUIRE(!image_sample(view, make_vector<double>(3.1, 0)));
    REQUIRE(!image_sample(view, make_vector<double>(0, -0.1)));
    REQUIRE(!image_sample(view, make_vector<double>(0, 3.1)));

    ASTROID_CHECK_ALMOST_EQUAL(
        double(image_sample(view, make_vector<double>(-2.9, 0.1)).value()),
        -1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(image_sample(view, make_vector<double>(0, 0.9)).value()), -0.5);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(image_sample(view, make_vector<double>(-2.9, 1.1)).value()),
        0.5);

    REQUIRE(!interpolated_image_sample(view, make_vector<double>(-3.1, 0)));
    REQUIRE(!interpolated_image_sample(view, make_vector<double>(3.1, 0)));
    REQUIRE(!interpolated_image_sample(view, make_vector<double>(0, -0.1)));
    REQUIRE(!interpolated_image_sample(view, make_vector<double>(0, 3.1)));

    ASTROID_CHECK_ALMOST_EQUAL(
        double(interpolated_image_sample(view, make_vector<double>(-2.9, 0.1))
                   .value()),
        -1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(interpolated_image_sample(view, make_vector<double>(-2, 0.5))
                   .value()),
        -1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(interpolated_image_sample(view, make_vector<double>(-1, 0.5))
                   .value()),
        -0.75);

    REQUIRE(!image_sample_over_box(
        view,
        box2d(make_vector<double>(-4, 0), make_vector<double>(0.1, 0.1))));

    ASTROID_CHECK_ALMOST_EQUAL(
        double(image_sample_over_box(
                   view,
                   box2d(make_vector<double>(0, 1), make_vector<double>(2, 1)))
                   .value()),
        1.25);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(make_vector<double>(0.5, 1), make_vector<double>(2, 1)))
                .value()),
        1.375);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(make_vector<double>(-4, 0), make_vector<double>(4, 0.5)))
                .value()),
        -5. / 6);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(image_sample_over_box(
                   view,
                   box2d(
                       make_vector<double>(-2, 0.1),
                       make_vector<double>(0.1, 0.1)))
                   .value()),
        -1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(make_vector<double>(0, 1.5), make_vector<double>(4, 2)))
                .value()),
        7. / 3);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(
                    make_vector<double>(0, 1.5), make_vector<double>(3, 1.5)))
                .value()),
        7. / 3);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(make_vector<double>(-4, -1), make_vector<double>(8, 5)))
                .value()),
        1.);
    ASTROID_CHECK_ALMOST_EQUAL(
        double(
            image_sample_over_box(
                view,
                box2d(
                    make_vector<double>(-4, -1), make_vector<double>(6, 3.5)))
                .value()),
        0.6);
}

TEST_CASE("gray_test")
{
    unsigned const s = 3;
    image<2, astroid::uint8_t, unique> img;
    create_image(img, make_vector(s, s));
    img.value_mapping = linear_function<double>(-1, 0.5);
    set_spatial_mapping(
        img, make_vector<double>(-3, 0), make_vector<double>(2, 1));
    sequential_fill(img, 0, 1);

    do_raw_gray_text(img);
    do_gray_test(img);

    do_gray_test(as_variant(as_const_view(img)));
}
