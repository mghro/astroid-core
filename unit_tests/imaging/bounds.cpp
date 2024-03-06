#include <astroid/common.hpp>
#include <astroid/imaging/bounds.hpp>
#include <astroid/imaging/view_transforms.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

TEST_CASE("get_memory_bounds_test")
{
    unsigned const s = 10;
    image<2, astroid::uint8_t, unique> img;
    create_image(img, make_vector(s, s));

    auto view = cast_storage_type<astroid::view>(img);

    memory_bounds<astroid::uint8_t> bounds;

    bounds = get_quick_bounds(img);
    REQUIRE(bounds.begin == img.pixels.ptr);
    REQUIRE(bounds.end == img.pixels.ptr + product(img.size));

    bounds = get_quick_bounds(aligned_view(flipped_view(view, 0)));
    REQUIRE(bounds.begin == img.pixels.ptr);
    REQUIRE(bounds.end == img.pixels.ptr + product(img.size));

    bounds = get_quick_bounds(aligned_view(flipped_view(view, 1)));
    REQUIRE(bounds.begin == img.pixels.ptr);
    REQUIRE(bounds.end == img.pixels.ptr + product(img.size));

    bounds = get_quick_bounds(
        aligned_view(flipped_view(flipped_view(view, 1), 0)));
    REQUIRE(bounds.begin == img.pixels.ptr);
    REQUIRE(bounds.end == img.pixels.ptr + product(img.size));
}
