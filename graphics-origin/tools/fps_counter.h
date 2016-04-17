/* Created on: Feb 25, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_FPS_COUNTER_H_
# define GRAPHICS_ORIGIN_FPS_COUNTER_H_

# include "../graphics_origin.h"

BEGIN_GO_NAMESPACE namespace tools {

/** @brief Compute an averaged FPS.
 *
 * This helper class provides an automatically updated and averaged FPS count.
 * The value is averaged at each interval specified in the constructor. A typical
 * use of this class:
 * \code{.cpp}
 * fps_counter counter( 1.5 ); // value refreshed every 1.5 second
 * while( running ) // main rendering loop
 * {
 *   float averageFPS = counter.get_fps();
 *   // averageFPS is guaranteed to remain the same for each interval
 *   // of 1.5 second. We can then display this value on screen: it will
 *   // slowly change such that we can still read it correctly.
 * }
 * \endcode
 */
class GO_API fps_counter
{
public:
  /** @brief Build a FPS counter with a specific refresh interval.
   *
   * Create a FPS counter that will be refreshed every refresh_interval
   * second(s).
   *
   * @param refresh_interval refresh interval, i.e. minimum duration for which
   * the value returned by this won't change. At the end of the interval, a
   * new FPS value is computed and stored to be returned.
   */
  fps_counter( real refresh_interval = real(2.0) );

  /** @brief Get the averaged FPS count.
   *
   * This function updates the internal members of the instance, taking into
   * account the last time it was called. If the last refresh was enough time
   * ago, a new averaged fps count is computed. This is this averaged fps count
   * that is returned in all cases.
   *
   * @return The averaged FPS count.
   */
  real get_fps();

private:

  /** Duration between two refresh of fps_counter::m_fps. */
  real m_refresh_interval;
  /** Last time the FPSCounter::m_fps was refreshed. */
  real m_last_refresh_time;
  /** Last time getFPS() was called. */
  real m_last_call_time;
  /** The sum of the FPS computed at each call to get_fps() since last refresh. */
  real m_accumulated_fps;
  /** Averaged FPS count stored such that get_fps() returns the same value between two refreshes.
   * When a refresh occurs, this value receives fps_counter::m_accumulated_fps / fps_counter::m_nb_samples.*/
  float m_fps;
  /** The number of FPS computed by each call to get_fps() since last refresh. */
  unsigned int m_nb_samples;
};

} END_GO_NAMESPACE
# endif 
