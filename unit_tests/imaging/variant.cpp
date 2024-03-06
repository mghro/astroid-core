#include <astroid/imaging/utilities.hpp>
#include <astroid/imaging/variant.hpp>

#include <astroid/imaging/test.hpp>

using namespace astroid;

struct size_fn
{
    std::size_t result;
    template<class Pixel, class SP>
    void
    operator()(image<2, Pixel, SP> const& img)
    {
        result = sizeof(Pixel);
    }
};

// template<class Pixel>
// void test_dispatch()
//{
//    unsigned const s = 3;
//    image<2,Pixel,unique> src;
//    create_image(src, vector2u(s, s));
//    size_fn f;
//    f.result = 1025;
//    image<2,variant,shared> v = as_variant(share(src));
//    apply_fn_to_variant(f, v);
//    REQUIRE(f.result ==  sizeof(Pixel));
//}
//
// TEST_CASE("dispatch")
//{
//    test_dispatch<int8>();
//    test_dispatch<uint8>();
//    test_dispatch<rgb<uint8> >();
//    test_dispatch<rgba<uint8> >();
//    test_dispatch<double>();
//    test_dispatch<rgb<float> >();
//    test_dispatch<int16>();
//    test_dispatch<uint16>();
//}

template<class Pixel>
void
test_gray_dispatch()
{
    unsigned const s = 3;
    image<2, Pixel, unique> src;
    create_image(src, make_vector(s, s));
    size_fn f;
    f.result = 1025;
    apply_fn_to_gray_variant(f, as_variant(as_const_view(src)));
    REQUIRE(f.result == sizeof(Pixel));
}

TEST_CASE("gray_dispatch")
{
    test_gray_dispatch<astroid::int8_t>();
    test_gray_dispatch<astroid::uint8_t>();
    test_gray_dispatch<double>();
    test_gray_dispatch<astroid::int16_t>();
    test_gray_dispatch<astroid::uint16_t>();
}

TEST_CASE("copying")
{
    unsigned const s = 3;
    image<2, double, unique> tmp;
    create_image(tmp, make_vector(s, s));
    sequential_fill(tmp, 4.1, 0.);
    auto src = as_variant(share(tmp));
    auto dst = make_eager_image_copy(src);
    auto dst_view = cast_image<image<2, double, const_view>>(dst);
    // TODO: more exhaustive checking (should have a function for this)
    REQUIRE(dst.size == src.size);
    REQUIRE(dst.value_mapping == src.value_mapping);
    for (unsigned i = 0; i < s * s; ++i)
        REQUIRE(dst_view.pixels[i] == 4.1);
}
