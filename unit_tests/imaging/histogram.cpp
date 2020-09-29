#include <cradle/imaging/histogram.hpp>

#include <cradle/imaging/test.hpp>

using namespace cradle;

TEST_CASE("raw_bytes_test")
{
    unsigned const s = 3;
    image<3, cradle::uint8_t, unique> img;
    create_image(img, make_vector(s, s, s));
    int const initial_n = 61;
    sequential_fill(img, initial_n, 1);

    std::vector<unsigned> hist;
    compute_raw_histogram(&hist, img);
    REQUIRE(hist.size() == 0x100);
    for (int i = 0; i < 0x100; ++i)
    {
        if (i < initial_n || i >= initial_n + s * s * s)
            REQUIRE(hist[i] == 0);
        else
            REQUIRE(hist[i] == 1);
    }
}

TEST_CASE("raw_random_data_test")
{
    cradle::uint16_t image_data[] = {
        17,
        12,
        43,
        16,
        43,
        17,
        71,
        17,
        19,
    };

    std::vector<unsigned> hist;
    compute_raw_histogram(
        &hist, make_const_view(image_data, make_vector<unsigned>(3, 3)));
    REQUIRE(hist.size() == 0x10000);
    REQUIRE(hist[17] == 3);
    REQUIRE(hist[19] == 1);
    REQUIRE(hist[20] == 0);
    REQUIRE(hist[43] == 2);
}
