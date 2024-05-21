#ifndef ASTROID_IO_MSGPACK_HPP
#define ASTROID_IO_MSGPACK_HPP

#include <msgpack.hpp>

#include <cradle/inner/encodings/msgpack_adaptors_main.h>

#include <astroid/array.hpp>
#include <astroid/immutable.hpp>

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

    namespace adaptor {

    template<class Value>
    struct convert<astroid::immutable<Value>>
    {
        msgpack::object const&
        operator()(
            msgpack::object const& o, astroid::immutable<Value>& v) const
        {
            if (o.type == msgpack::type::NIL)
            {
                v = astroid::immutable<Value>();
            }
            else
            {
                Value value;
                o.convert(value);
                v = astroid::make_immutable(std::move(value));
            }
            return o;
        }
    };

    template<class Value>
    struct pack<astroid::immutable<Value>>
    {
        template<typename Stream>
        msgpack::packer<Stream>&
        operator()(
            msgpack::packer<Stream>& p,
            astroid::immutable<Value> const& v) const
        {
            if (v)
                p.pack(*v);
            else
                p.pack_nil();
            return p;
        }
    };

    template<class Value>
    struct object_with_zone<astroid::immutable<Value>>
    {
        void
        operator()(
            msgpack::object::with_zone& o,
            astroid::immutable<Value> const& v) const
        {
            if (v)
                adaptor::object_with_zone<Value>()(o, *v);
            else
                o.type = msgpack::type::NIL;
        }
    };

    } // namespace adaptor

} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

#endif
