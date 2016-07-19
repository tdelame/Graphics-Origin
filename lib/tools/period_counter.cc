# include "../../graphics-origin/tools/period_counter.h"
# include <omp.h>
namespace graphics_origin {
  namespace tools {

    period_counter::period_counter( real refresh_interval ) :
        refresh_interval( refresh_interval > real(0) ? refresh_interval : real(2.0) ),
        last_refresh_time( omp_get_wtime() ), last_tick_time( last_refresh_time ),
        accumulated_period( 0 ), averaged_period( 0 ),
        number_of_samples( 0 )
    {}

    void period_counter::tick()
    {
      last_tick_time = omp_get_wtime();
    }

    void period_counter::tock()
    {
      auto current_time = omp_get_wtime();
      accumulated_period += current_time - last_tick_time;
      ++number_of_samples;
    }

    real period_counter::get_period()
    {
      auto current_time = omp_get_wtime();
      if( current_time > last_refresh_time + refresh_interval )
        {
          averaged_period = accumulated_period / real(number_of_samples);
          accumulated_period = real(0);
          number_of_samples = 0;
          last_refresh_time = current_time;
        }
      return averaged_period;
    }
  }
}
