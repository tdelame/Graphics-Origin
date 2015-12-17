/*  Created on: Dec 17, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_LOG_H_
# define GRAPHICS_ORIGIN_LOG_H_

# include <graphics_origin.h>
# include <tools/filesystem.h>

# include <string>

# include <boost/log/sources/global_logger_storage.hpp>
# include <boost/log/sources/severity_logger.hpp>
# include <boost/log/sources/severity_feature.hpp>
# include <boost/log/trivial.hpp>
# include <boost/log/sources/record_ostream.hpp>

BEGIN_GO_NAMESPACE
namespace tools
{
  typedef boost::log::trivial::severity_level severity_level;

  typedef boost::log::sources::severity_logger_mt< severity_level > logger_mt_type;

  BOOST_LOG_GLOBAL_LOGGER(logger, logger_mt_type)

  extern void
  init_log( const std::string& log_filename );

  /**
   * This function will flush all log entry to its destination. It could be used
   * when an exception has been caught, just before exiting the application.
   */
  extern void
  flush_log();
}

# define LOG(level,msg)                                                       \
  {                                                                           \
    if( tools::severity_level::level > tools::severity_level::warning )       \
    {                                                                         \
      BOOST_LOG_STREAM_SEV(tools::logger::get(), tools::severity_level::level)\
      <<msg<<" (in "<<tools::get_basename(__FILE__)<<":"<< __LINE__ <<")";    \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      BOOST_LOG_STREAM_SEV(tools::logger::get(),tools::severity_level::level) \
      << msg;                                                                 \
    }                                                                         \
  }

# define LOG_WITH_LINE_FILE(level,msg,line,file)                              \
  {                                                                           \
      BOOST_LOG_STREAM_SEV(tools::logger::get(), tools::severity_level::level)\
      <<msg<<" (in "<<tools::get_basename( file )<<":"<< line <<")";          \
  }

END_GO_NAMESPACE

# endif /* PROJECT_LOG_H_ */
