#ifndef ASTROID_SHARED_MEMORY_HPP
#define ASTROID_SHARED_MEMORY_HPP

#include <astroid/allocators.hpp>

#include <cradle/inner/requests/generic.h>

namespace astroid {

template<class T>
struct shared_memory_allocator : allocator_interface<T>
{
    cradle::local_context_intf* context;

    std::shared_ptr<cradle::data_owner>
    allocate(size_t n) override
    {
        // Allocate a shared memory block and assign it.
        // Note that owner->data() is going to point to the beginning of a
        // memory-mapped allocation, which should be page-aligned, so there
        // shouldn't be any issues with alignment here.
        auto owner = context->make_data_owner(sizeof(T) * n, true);
        // Invoke the placement new operator to construct the elements.
        (void) new (owner->data()) T[n];
        return owner;
    }
};

struct shared_memory_allocation_policy
{
    cradle::local_context_intf* context;

    template<class T>
    shared_memory_allocator<T>
    make_allocator()
    {
        shared_memory_allocator<T> allocator;
        allocator.context = context;
        return allocator;
    }
};

} // namespace astroid

#endif
