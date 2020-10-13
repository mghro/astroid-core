#include <boost/shared_array.hpp>
#include <cradle/common.hpp>

#include <cradle/test.hpp>

using namespace cradle;

TEST_CASE("any_test")
{
    any a;
    a = int(1);
    REQUIRE(unsafe_any_cast<int>(a) == 1);
    REQUIRE(!any_cast<double>(a));

    any b(a);
    REQUIRE(unsafe_any_cast<int>(b) == 1);
    REQUIRE(!any_cast<double>(b));

    any c(1.5);
    REQUIRE(unsafe_any_cast<double>(c) == 1.5);
    REQUIRE(any_cast<double>(c));
    REQUIRE(*any_cast<double>(c) == 1.5);

    b = c;
    REQUIRE(unsafe_any_cast<double>(b) == 1.5);
    REQUIRE(any_cast<double>(b));
    REQUIRE(*any_cast<double>(b) == 1.5);
}

TEST_CASE("optional_test")
{
    optional<int> a;
    REQUIRE(!a);
    a = 1;
    REQUIRE(a);
    REQUIRE(get(a) == 1);

    optional<int> b = a;
    REQUIRE(b);
    REQUIRE(a == b);
    REQUIRE(get(b) == 1);

    a = none;
    REQUIRE(a != b);
    REQUIRE(a < b);
    REQUIRE(!(b < a));
    REQUIRE(!a);
    REQUIRE(b);
    b = 2;
    a = 3;
    REQUIRE(b);
    REQUIRE(get(b) == 2);
    REQUIRE(a != b);
    REQUIRE(b < a);
    REQUIRE(!(a < b));
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

TEST_CASE("value_equality_tests")
{
    value t(true);
    value f(false);
    REQUIRE(t == value(true));
    REQUIRE(t != value(false));
    REQUIRE(f == value(false));
    REQUIRE(t != f);

    value x(0.1);
    value y(0.2);
    value z(0.1);
    REQUIRE(x == value(0.1));
    REQUIRE(x != y);
    REQUIRE(y != z);
    REQUIRE(x == z);
    REQUIRE(z == x);

    REQUIRE(t != x);
    REQUIRE(f != y);

    value s("foo");
    REQUIRE(s == value("foo"));
    REQUIRE(s != value("bar"));

    blob blob = make_blob(10);
    value b1(blob);
    value b2(blob);
    REQUIRE(b1 == b2);
    blob = make_blob(1);
    value b3(blob);
    REQUIRE(b1 != b3);

    value_map r;
    r[value("x")] = x;
    REQUIRE(get_field(r, "x") == x);

    value_map q;
    q[value("r")] = value(r);
    REQUIRE(get_field(q, "r") == value(r));
    REQUIRE(get_field(q, "r") != value(q));
    REQUIRE(get_field(q, "r") != x);

    q.clear();
    q[value("y")] = x;
    REQUIRE(q != r);

    q.clear();
    q[value("x")] = x;
    REQUIRE(q == r);

    q.clear();
    q[value("x")] = y;
    REQUIRE(q != r);

    value_list l;
    l.push_back(value(r));
    l.push_back(x);

    value_list m;
    REQUIRE(l != m);
    m.push_back(value(r));
    REQUIRE(l != m);
    m.push_back(x);
    REQUIRE(l == m);
    m.push_back(y);
    REQUIRE(l != m);
}
