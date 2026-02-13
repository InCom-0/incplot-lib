/*
 * LLVM 21's libc++ moved std::__hash_memory from an inline function to an
 * external symbol exported from libc++.dylib (llvm/llvm-project#127040).
 * On macOS, the system /usr/lib/libc++.1.dylib doesn't have this symbol,
 * causing link failures and runtime crashes.
 *
 * This shim provides the missing symbol using libc++'s own cityhash
 * implementation (which is still available in the headers).
 *
 * See: https://github.com/llvm/llvm-project/issues/77653
 * TODO: Remove this when the system libc++ on our minimum macOS version
 *       includes __hash_memory, or when LLVM fixes the driver (issue #77653).
 */

#if defined(__APPLE__)

#include <__config>
#include <__functional/hash.h>
#include <cstddef>

#if _LIBCPP_VERSION >= 220000
#error "LLVM 22+ detected. Check if __hash_memory is still missing from the system libc++.dylib. " \
       "If the symbol is now available (or LLVM fixed issue #77653), remove this shim."
#endif

#if _LIBCPP_AVAILABILITY_HAS_HASH_MEMORY

_LIBCPP_BEGIN_NAMESPACE_STD

[[__gnu__::__pure__]]
_LIBCPP_EXPORTED_FROM_ABI
size_t __hash_memory(_LIBCPP_NOESCAPE const void* __ptr, size_t __size) _NOEXCEPT {
    return __murmur2_or_cityhash<size_t>()(__ptr, __size);
}

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP_AVAILABILITY_HAS_HASH_MEMORY

#endif // __APPLE__