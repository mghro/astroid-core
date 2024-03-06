#include <astroid/geometry/transformations.hpp>

#include <astroid/test.hpp>

using namespace astroid;

TEST_CASE("simple_2d_transformations_test")
{
    // 2D translation
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            translation(make_vector<double>(14, 1)),
            make_vector<double>(7, 3)),
        make_vector<double>(21, 4));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_vector(
            translation(make_vector<double>(14, 1)),
            make_vector<double>(-7, 3)),
        make_vector<double>(-7, 3));

    // 3D translation
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            translation(make_vector<float>(14, 1, 0)),
            make_vector<float>(7, 0, 3)),
        make_vector<float>(21, 1, 3));

    // 2D rotation
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            rotation(angle<double, degrees>(90)), make_vector<double>(1, 0)),
        make_vector<double>(0, 1));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_vector(
            rotation(angle<double, degrees>(-90)), make_vector<double>(1, 0)),
        make_vector<double>(0, -1));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_vector(
            rotation(angle<double, degrees>(180)), make_vector<double>(1, 0)),
        make_vector<double>(-1, 0));

    // 2D scaling
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(
            scaling_transformation(make_vector<double>(7, 3)),
            make_vector<double>(5, 9)),
        make_vector<double>(35, 27));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_vector(
            scaling_transformation(make_vector<double>(7, 3)),
            make_vector<double>(5, 9) - make_vector<double>(0, 2)),
        make_vector<double>(35, 21));
}

TEST_CASE("composed_2d_transformations_test")
{
    matrix<3, 3, double> m = translation(make_vector<double>(4, 3))
                             * rotation(angle<double, degrees>(90));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(m, make_vector<double>(0, 0)),
        make_vector<double>(4, 3));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(m, make_vector<double>(1, 0)),
        make_vector<double>(4, 4));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_point(m, make_vector<double>(0, 1)),
        make_vector<double>(3, 3));
    ASTROID_CHECK_ALMOST_EQUAL(
        transform_vector(m, make_vector<double>(1, 0)),
        make_vector<double>(0, 1));
}
