#include <boost/filesystem/operations.hpp>
#include <boost/shared_array.hpp>
#include <cradle/io/generic_io.hpp>

#include <cradle/test.hpp>

using namespace cradle;

void
test_value_io(value const& v)
{
    {
        write_value_file("value_file", v);
        value u;
        read_value_file(&u, "value_file");
        REQUIRE(u == v);
    }

    {
        std::string s;
        value_to_string(&s, v);
        value u;
        parse_value_string(&u, s);
        REQUIRE(u == v);
    }

    {
        std::string json;
        value_to_json(&json, v);
        value u;
        parse_json_value(&u, json);
        REQUIRE(u == v);
    }

    {
        byte_vector data;
        serialize_value(&data, v);
        value u;
        deserialize_value(&u, &data[0], data.size());
        REQUIRE(u == v);
    }

    {
        std::string s;
        value_to_base64_string(&s, v);
        value u;
        parse_base64_value_string(&u, s);
        REQUIRE(u == v);

        // Corrupt the encoded string and check that the CRC test fails.
        char& c = s[s.length() - 3];
        if (c == '_')
            c = '-';
        else
            c = '_';
        REQUIRE_THROWS_AS(parse_base64_value_string(&u, s), std::exception);
    }
}

static blob
make_blob(size_t size)
{
    blob blob;
    blob.size = size;
    boost::shared_array<cradle::uint8_t> storage(new cradle::uint8_t[size]);
    blob.ownership = storage;
    for (std::size_t i = 0; i != size; ++i)
        storage[i] = cradle::uint8_t(i + 1);
    blob.data = storage.get();
    return blob;
}

TEST_CASE("generic_io_test")
{
    value t(true);
    value f(false);

    value x(0.1);
    value y(0.2);

    value foo("foo");
    value bar("bar");
    value empty_string("");

    value_list primitives;
    primitives.push_back(t);
    primitives.push_back(f);
    primitives.push_back(x);
    primitives.push_back(y);
    primitives.push_back(foo);
    primitives.push_back(bar);
    primitives.push_back(empty_string);
    test_value_io(value(primitives));

    value b(make_blob(1000));
    test_value_io(b);
    set(b, make_blob(10));
    test_value_io(b);

    value_map r;
    test_value_io(value(r));
    r[value("b")] = b;
    test_value_io(value(r));
    r[value("x")] = x;
    test_value_io(value(r));
    r[value("y")] = y;
    test_value_io(value(r));

    value_list l;
    test_value_io(value(l));
    l.push_back(value(r));
    test_value_io(value(l));
    l.push_back(x);
    test_value_io(value(l));
    l.push_back(y);
    test_value_io(value(l));

    // Test the JSON output to make sure that the JSON I/O functions are
    // actually dealing in JSON.
    string json;
    value_to_json(&json, value(l));
    REQUIRE(json ==
        "[\n"
        "  {\n"
        "    \"b\": \n"
        "    {\n"
        "      \"blob\": \"AQIDBAUGBwgJCg==\",\n"
        "      \"type\": \"base64-encoded-blob\"\n"
        "    },\n"
        "    \"x\": 0.1,\n"
        "    \"y\": 0.2\n"
        "  },\n"
        "  0.1,\n"
        "  0.2\n"
        "]");
}
