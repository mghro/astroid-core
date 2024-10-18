#include <astroid/common.hpp>
#include <astroid/imaging/apply_palette.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("apply_palette_test")
{
    unsigned const s = 3;
    astroid::uint8_t data[s * s] = {
        4,
        3,
        0,
        0,
        10,
        70,
        1,
        0,
        9,
    };
    image<2, astroid::uint8_t, const_view> src
        = make_const_view(data, make_vector(s, s));

    astroid::uint16_t palette[256];
    for (unsigned i = 0; i < 256; ++i)
        palette[i] = astroid::uint16_t(i * 7);

    image<2, astroid::uint16_t, unique> result;
    create_image(result, make_vector(s, s));

    apply_palette(result, src, palette);
    astroid::uint16_t correct_results[] = {
        28,
        21,
        0,
        0,
        70,
        490,
        7,
        0,
        63,
    };
    ASTROID_CHECK_IMAGE(result, correct_results, correct_results + s * s);
}
