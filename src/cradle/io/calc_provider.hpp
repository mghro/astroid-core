#ifndef CRADLE_IO_CALC_PROVIDER_HPP
#define CRADLE_IO_CALC_PROVIDER_HPP

#include <cradle/api.hpp>
#include <cradle/common.hpp>

namespace cradle {

// Implement a calculation provider for an API.
void
provide_calculations(
    int argc, char const* const* argv, api_implementation const& api);

} // namespace cradle

#endif
