#include <astroid/io/compression.hpp>
#include <astroid/io/file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/scoped_array.hpp>
#include <cstdlib>

#include <astroid/test.hpp>

using namespace astroid;

TEST_CASE("to_file")
{
    unsigned const data_size = 0x20301;
    boost::scoped_array<astroid::uint8_t> data(
        new astroid::uint8_t[data_size]);
    for (unsigned i = 0; i != data_size; ++i)
        data[i] = (std::rand() & 0x7f) + 0x70;

    {
        c_file f("compressed", "wb");
        compress(f, data.get(), data_size);
    }
    boost::scoped_array<astroid::uint8_t> decompressed_data(
        new astroid::uint8_t[data_size]);
    {
        c_file f("compressed", "rb");
        decompress(decompressed_data.get(), data_size, f);
    }

    REQUIRE(std::equal(
        data.get(), data.get() + data_size, decompressed_data.get()));

    remove(boost::filesystem::path("compressed"));
}

TEST_CASE("to_memory")
{
    std::size_t const data_size = 0x3020401;
    boost::scoped_array<astroid::uint8_t> data(
        new astroid::uint8_t[data_size]);
    for (std::size_t i = 0; i < data_size; ++i)
        data[i] = (std::rand() & 0x7f) + 0x70;

    boost::scoped_array<astroid::uint8_t> compressed_data;
    std::size_t compressed_data_size;
    compress(&compressed_data, &compressed_data_size, data.get(), data_size);

    boost::scoped_array<astroid::uint8_t> decompressed_data(
        new astroid::uint8_t[data_size]);
    decompress(
        decompressed_data.get(),
        data_size,
        compressed_data.get(),
        compressed_data_size);

    REQUIRE(std::equal(
        data.get(), data.get() + data_size, decompressed_data.get()));
}
