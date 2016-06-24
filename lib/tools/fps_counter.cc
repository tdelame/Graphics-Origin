# include "../../graphics-origin/tools/fps_counter.h"
# include <omp.h>

BEGIN_GO_NAMESPACE namespace tools {

  fps_counter::fps_counter( real display_interval )
    : m_refresh_interval{ display_interval > 0.0 ? display_interval : real(2.0)},
      m_last_refresh_time{ omp_get_wtime() },  m_last_call_time{ omp_get_wtime() },
      m_accumulated_fps{0}, m_fps(0.0f), m_nb_samples{0}
  {}

  real fps_counter::get_fps()
  {
    auto current_time = omp_get_wtime();
    auto duration = current_time - m_last_call_time;
    real currentFPS = real(1.0) / real(duration);
    m_accumulated_fps += currentFPS;
    ++m_nb_samples;

    // It's time to update the value
    if( current_time > m_last_refresh_time + m_refresh_interval )
    {
        m_fps = m_accumulated_fps / real(m_nb_samples);
        m_accumulated_fps = real(0);
        m_nb_samples = 0;
        m_last_refresh_time = current_time;
    }

    m_last_call_time = current_time;
    return m_fps;
  }

} END_GO_NAMESPACE
