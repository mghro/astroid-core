#ifndef ASTROID_IMMUTABLE_HPP
#define ASTROID_IMMUTABLE_HPP

#include <astroid/common.hpp>

namespace astroid {

// immutable<T> holds (by shared_ptr) an immutable value of type T.
// immutable also provides the ability to efficiently store values in a
// dynamically-typed way. Any immutable<T> can be cast to and from
// untyped_immutable.

struct untyped_immutable_value : noncopyable
{
    virtual ~untyped_immutable_value()
    {
    }
    virtual cradle::api_type_info
    type_info() const
        = 0;
    virtual size_t
    deep_size() const
        = 0;
    virtual size_t
    hash() const
        = 0;
    virtual cradle::dynamic
    as_dynamic() const
        = 0;
    virtual bool
    equals(untyped_immutable_value const* other) const
        = 0;
};

struct untyped_immutable
{
    std::shared_ptr<untyped_immutable_value> ptr;
};

bool static inline is_initialized(untyped_immutable const& x)
{
    return x.ptr ? true : false;
}

static inline untyped_immutable_value const*
get_value_pointer(untyped_immutable const& x)
{
    return x.ptr.get();
}

void static inline reset(untyped_immutable& x)
{
    x.ptr.reset();
}

template<class T>
struct immutable_value : untyped_immutable_value
{
    T value;
    cradle::api_type_info
    type_info() const override
    {
        return cradle::get_type_info<T>();
    }
    size_t
    deep_size() const override
    {
        return deep_sizeof(this->value);
    }
    size_t
    hash() const override
    {
        return invoke_hash(this->value);
    }
    cradle::dynamic
    as_dynamic() const override
    {
        return to_value(this->value);
    }
    bool
    equals(untyped_immutable_value const* other) const override
    {
        auto const* typed_other
            = dynamic_cast<immutable_value<T> const*>(other);
        return typed_other && this->value == typed_other->value;
    }
};

template<class T>
struct immutable
{
    std::shared_ptr<immutable_value<T>> ptr;
};

template<class T>
bool
is_initialized(immutable<T> const& x)
{
    return x.ptr ? true : false;
}

template<class T>
void
reset(immutable<T>& x)
{
    x.ptr.reset();
}

template<class T>
void
initialize(immutable<T>& x, T value)
{
    x.ptr.reset(new immutable_value<T>);
    x.ptr->value = std::move(value);
}

template<class T>
immutable<T>
make_immutable(T value)
{
    immutable<T> x;
    initialize(x, std::move(value));
    return x;
}

template<class T>
size_t
invoke_hash(immutable<T> const& x)
{
    using cradle::invoke_hash;
    return is_initialized(x) ? cradle::invoke_hash(get(x)) : 0;
}

template<class T>
void
swap_in(immutable<T>& x, T& value)
{
    x.ptr.reset(new immutable_value<T>);
    swap(x.ptr->value, value);
}

template<class T>
T const&
get(immutable<T> const& x)
{
    return x.ptr->value;
}

} // namespace astroid
template<class T>
struct cradle::type_info_query<astroid::immutable<T>>
    : cradle::type_info_query<T>
{
};
namespace astroid {

template<class T>
size_t
deep_sizeof(immutable<T> const& x)
{
    return x.ptr->deep_size();
}

template<class T>
void
swap(immutable<T>& a, immutable<T>& b)
{
    swap(a.ptr, b.ptr);
}

template<class T>
bool
operator==(immutable<T> const& a, immutable<T> const& b)
{
    // First test if the two immutables are actually pointing to the same
    // thing, which avoids having to do a deep comparison for this case.
    return a.ptr == b.ptr || (a.ptr ? (b.ptr && get(a) == get(b)) : !b.ptr);
}
template<class T>
bool
operator!=(immutable<T> const& a, immutable<T> const& b)
{
    return !(a == b);
}
template<class T>
bool
operator<(immutable<T> const& a, immutable<T> const& b)
{
    return b.ptr && (a.ptr ? get(a) < get(b) : true);
}

template<class T>
void
from_dynamic(immutable<T>* x, cradle::dynamic const& v)
{
    using cradle::from_dynamic;
    x->ptr.reset(new immutable_value<T>);
    from_dynamic(&x->ptr->value, v);
}
template<class T>
void
to_dynamic(cradle::dynamic* v, immutable<T> const& x)
{
    using cradle::to_dynamic;
    if (x.ptr)
        to_dynamic(v, get(x));
    else
        to_dynamic(v, T());
}

template<class T>
std::ostream&
operator<<(std::ostream& s, immutable<T> const& x)
{
    if (x.ptr)
        s << get(x);
    else
        s << T();
    return s;
}

} // namespace astroid

#endif
