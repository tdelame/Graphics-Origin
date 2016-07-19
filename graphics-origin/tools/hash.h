# ifndef GRAPHICS_ORIGIN_TOOLS_HASH_H_
# define GRAPHICS_ORIGIN_TOOLS_HASH_H_
# include <stdint.h>
# include <string>
namespace graphics_origin {
  namespace tools {
    namespace detail {
      /**Offset of the FNV-1a hash algorithm.*/
      constexpr uint32_t fnv_1a_offset = 2166136261u;
      /**Prime number of the FNV-1a hash algorithm.*/
      constexpr uint32_t fnv_1a_prime  =   16777619u;
      /**@brief Implementation of the FNV-1a hash algorithm
       *
       * This function is the implementation of the FNV-1a hash algorithm for
       * string literals. As long as you call it with compile time constant,
       * the result will be available at compile time.
       * @param res Current result of the hash algorithm.
       * @param input Pointer to the remaining string to hash. */
      constexpr uint32_t hash_impl( uint32_t res, const char* input )
      {
        return *input ? hash_impl( (res ^ *input) * fnv_1a_prime, input + 1 ) : res;
      }
    }

    /**@brief Compile time hash of a string literal.
     *
     * This function computes at compile time (in release mode) the hash value
     * of a string literal. If you intend to pass a string \c s to this function
     * by calling <tt>hash( s.c_str())</tt>, the computations are done at runtime,
     * because a string object require an allocation.
     * @param input The string literal to hash.
     */
    constexpr uint32_t hash( const char* input )
    {
      return detail::hash_impl( detail::fnv_1a_offset, input );
    }
    /**@brief Runtime hash of a string.
     *
     * This function computes at runtime the hash value of a string. Try to
     * avoid as much as possible this function as there is one that performs
     * computation at compile time on string literals.
     * @param input The string to hash.
     */
    uint32_t hash( const std::string& input )
    {
      return detail::hash_impl( detail::fnv_1a_offset, input.c_str() );
    }
  }
}
# endif
