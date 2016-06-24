# ifndef GRAPHICS_ORIGIN_PERIOD_COUNTER_H_
# define GRAPHICS_ORIGIN_PERIOD_COUNTER_H_
# include "../graphics_origin.h"
namespace graphics_origin {
  namespace tools {

    /**@brief Compute an averaged period.
     *
     * This helper class provides a way to compute an averaged period
     * for a cyclic event. The period is averaged, because in case the
     * period is under 1 or 2 seconds, the changes of period will be
     * too fast to be read by a user. Also, averaging allows to smooth
     * out variations.
     *
     * To use this class, you must call tick() every time the cyclic
     * even is started, and tock() when the cycle just ended. Then,
     * when you want to get the averaged period, simply call get_period().
     */
    class GO_API period_counter {
    public:

      /**@brief Build an instance of a period counter.
       *
       * Build a new instance of a period counter. The constructor takes as
       * argument the refresh interval to use for the averaging operation.
       * @param refresh_interval Should be positive, otherwise, a, interval of 2 seconds is used.
       */
      period_counter( real refresh_interval = real(2.0 ) );

      /**@brief Notify the start of a cyclic event.
       *
       */
      void tick();

      /**@brief Notify the end of a cyclic event.
       *
       */
      void tock();

      /**@brief Get the average period.
       *
       * Get the averaged period in seconds. This function automatically
       * update the value at the refresh time specified in the constructor.
       * @return The averaged period.
       */
      real get_period();

    private:
      const real refresh_interval;
      real last_refresh_time;
      real last_tick_time;
      real accumulated_period;
      real averaged_period;
      unsigned int number_of_samples;
    };
  }
}


# endif 
