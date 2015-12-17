/* Created on: Dec 17, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_HASH_TUPLE_H_
# define GRAPHICS_ORIGIN_HASH_TUPLE_H_

# include <graphics_origin.h>

/**
 * These structures are helpful to use unordered_map or unordered_set on tuples.
 * Indeed, to be used, those containers require a hash function, which is
 * defined here for any tuple.
 */

# ifndef __CUDACC__
#   include <tuple>
#   include <stddef.h>
namespace hash_tuple {
  template <typename TT>
  struct hash
  {
    size_t
    operator()(TT const& tt) const
    {
      return std::hash<TT>()(tt);
    }
  };
  namespace {
    template <class T>
    inline void hash_combine(std::size_t& seed, T const& v)
    {
        seed ^= hash_tuple::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
  }

  namespace {
    // Recursive template code derived from Matthieu M.
    template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
    struct HashValueImpl
    {
      static void apply(size_t& seed, Tuple const& tuple)
      {
        HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
      }
    };
    template <class Tuple>
    struct HashValueImpl<Tuple,0>
    {
      static void apply(size_t& seed, Tuple const& tuple)
      {
        hash_combine(seed, std::get<0>(tuple));
      }
    };
  }

  template <typename ... TT>
  struct hash<std::tuple<TT...>>
  {
    size_t
    operator()(std::tuple<TT...> const& tt) const
    {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
        return seed;
    }
  };
}
# endif
# endif 
