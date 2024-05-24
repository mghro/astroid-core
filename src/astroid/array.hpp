#ifndef ASTROID_ARRAY_HPP
#define ASTROID_ARRAY_HPP

#include <memory>

#include <astroid/allocators.hpp>
#include <astroid/common.hpp>

namespace astroid {

// `array` stores an immutable array of values with unspecified ownership.
// Note that arrays are externalized as blobs, so only POD types are allowed.
// (Well, nearly POD, since we want to allow the constructors that the
// preprocessor generates.)
template<class T>
struct array
{
    // This doesn't seem to work on GCC, at least not 4.6.
#ifdef _MSC_VER
    static_assert(
        std::is_trivially_destructible<T>::value
            && std::is_standard_layout<T>::value,
        "array elements must be POD types.");
#endif

    array() : elements(0), n_elements(0)
    {
    }

    T const* elements;
    size_t n_elements;
    ownership_holder ownership;

    // STL-like interface
    typedef T value_type;
    typedef T const* const_iterator;
    typedef const_iterator iterator;
    const_iterator
    begin() const
    {
        return elements;
    }
    const_iterator
    end() const
    {
        return elements + n_elements;
    }
    typedef size_t size_type;
    size_t
    size() const
    {
        return n_elements;
    }
    T const&
    operator[](size_t i) const
    {
        return elements[i];
    }
};

template<class T, class Allocator>
T*
allocate(array<T>* array, size_t n_elements, Allocator& allocator)
{
    std::shared_ptr<cradle::data_owner> owner = allocator.allocate(n_elements);
    T* raw_ptr = reinterpret_cast<T*>(owner->data());
    array->elements = raw_ptr;
    array->ownership = std::move(owner);
    array->n_elements = n_elements;
    return raw_ptr;
}

template<class T>
T*
allocate(array<T>* array, size_t n_elements)
{
    return allocate(array, n_elements, default_allocator<T>());
}

template<class T>
void
clear(array<T>* array)
{
    array->elements = 0;
    array->n_elements = 0;
    array->ownership = ownership_holder();
}

template<class T>
void
initialize(array<T>* array, std::vector<T> const& vector)
{
    size_t n_elements = vector.size();
    auto ptr = std::make_unique<T[]>(n_elements);
    T* elements = ptr.get();
    array->ownership = make_unique_ptr_owner(std::move(ptr));
    array->elements = elements;
    array->n_elements = n_elements;
    for (size_t i = 0; i != n_elements; ++i)
        elements[i] = vector[i];
}

} // namespace astroid
template<class T>
struct cradle::type_info_query<astroid::array<T>>
{
    static void
    get(api_type_info* info)
    {
        *info = cradle::make_api_type_info_with_blob_type(
            cradle::api_blob_type{});
    }
};
namespace astroid {

template<class T>
size_t
deep_sizeof(array<T> const& x)
{
    using cradle::deep_sizeof;
    // We assume that array is only used with types that have a static
    // deep_sizeof.
    return deep_sizeof(T()) * x.size();
}

template<class T>
void
swap(array<T>& a, array<T>& b)
{
    swap(a.elements, b.elements);
    swap(a.n_elements, b.n_elements);
    swap(a.ownership, b.ownership);
}

template<class T>
bool
operator==(array<T> const& a, array<T> const& b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i != a.size(); ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}
template<class T>
bool
operator!=(array<T> const& a, array<T> const& b)
{
    return !(a == b);
}
template<class T>
bool
operator<(array<T> const& a, array<T> const& b)
{
    if (a.size() < b.size())
        return true;
    if (b.size() < a.size())
        return false;
    for (size_t i = 0; i != a.size(); ++i)
    {
        if (a[i] < b[i])
            return true;
        if (b[i] < a[i])
            return false;
    }
    return false;
}

template<class T>
void
from_blob(astroid::array<T>* x, cradle::blob const& b)
{
    x->n_elements = b.size() / sizeof(T);
    x->elements = reinterpret_cast<T const*>(b.data());
    x->ownership = b.shared_owner();
}

template<class T>
cradle::blob
to_blob(astroid::array<T> x)
{
    return cradle::blob(
        std::move(x.ownership),
        reinterpret_cast<std::byte const*>(x.elements),
        x.n_elements * sizeof(T));
}

template<class T>
void
from_dynamic(array<T>* x, cradle::dynamic const& v)
{
    from_blob(x, cast<cradle::blob>(v));
}

template<class T>
void
to_dynamic(cradle::dynamic* v, array<T> x)
{
    *v = to_blob(std::move(x));
}

template<class T>
std::ostream&
operator<<(std::ostream& s, array<T> const& x)
{
    s << "{";
    for (unsigned i = 0; i != x.n_elements; ++i)
    {
        if (i != 0)
            s << ",";
        s << x.elements[i];
    }
    s << "}";
    return s;
}

template<class T>
size_t
hash_value(astroid::array<T> const& x)
{
    size_t h = 0;
    for (auto const& i : x)
        h = cradle::combine_hashes(h, cradle::invoke_hash(i));
    return h;
}

} // namespace astroid

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
    namespace adaptor {

    template<class Element>
    struct convert<astroid::array<Element>>
    {
        msgpack::object const&
        operator()(msgpack::object const& o, astroid::array<Element>& v) const
        {
            cradle::blob blob;
            o.convert(blob);
            from_blob(&v, blob);
            return o;
        }
    };

    template<class Element>
    struct pack<astroid::array<Element>>
    {
        template<typename Stream>
        msgpack::packer<Stream>&
        operator()(
            msgpack::packer<Stream>& p, astroid::array<Element> const& v) const
        {
            p.pack(to_blob(v));
            return p;
        }
    };

    template<class Element>
    struct object_with_zone<astroid::array<Element>>
    {
        void
        operator()(
            msgpack::object::with_zone& o,
            astroid::array<Element> const& v) const
        {
            auto blob = to_blob(v);
            adaptor::object_with_zone<cradle::blob>()(o, blob);
        }
    };

    } // namespace adaptor
}
} // namespace msgpack

#endif
