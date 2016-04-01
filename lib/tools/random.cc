/* Created on: Apr 1, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/tools/random.h"
# include <chrono>
# include <random>
BEGIN_GO_NAMESPACE namespace tools {

  real unit_random()
  {
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count() );
    static std::uniform_real_distribution<real> distribution( real(0), real(1));
    return distribution(generator);
  }

} END_GO_NAMESPACE
