#ifndef ASTROID_IO_MSGPACK_HPP
#define ASTROID_IO_MSGPACK_HPP

#include <msgpack.hpp>

#include <cradle/inner/encodings/msgpack_adaptors_main.h>

#include <astroid/array.hpp>

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

} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

#endif
