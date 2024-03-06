#include <astroid/common.hpp>
#include <astroid/imaging/level_window.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("level_window_test")
{
    unsigned const s = 3;
    image<2, astroid::uint16_t, unique> src;
    create_image(src, make_vector(s, s));
    src.value_mapping = linear_function<double>(-2, 2);
    sequential_fill(src, 1, 1);

    {
        auto result = apply_level_window(src, 8, 6);
        astroid::uint8_t correct_results[] = {
            0,
            0,
            0,
            42,
            127,
            212,
            255,
            255,
            255,
        };
        ASTROID_CHECK_IMAGE(result, correct_results, correct_results + s * s);
    }

    {
        image<2, astroid::uint8_t, unique> result;
        create_image(result, make_vector(s, s));
        apply_paletted_level_window(result, src, 8, 6);
        astroid::uint8_t correct_results[] = {
            0,
            0,
            0,
            42,
            127,
            212,
            255,
            255,
            255,
        };
        ASTROID_CHECK_IMAGE(result, correct_results, correct_results + s * s);
    }
}
