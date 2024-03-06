#ifndef ASTROID_IO_CRC_HPP
#define ASTROID_IO_CRC_HPP

#include <astroid/common.hpp>

namespace astroid {

uint32_t
compute_crc32(uint32_t crc, void const* data, size_t size);

}

#endif
