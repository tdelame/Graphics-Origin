/* Created on: Dec 17, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <tools/log.h>

# include <boost/date_time/posix_time/posix_time_types.hpp>
# include <boost/log/trivial.hpp>
# include <boost/log/expressions.hpp>
# include <boost/log/sources/severity_logger.hpp>
# include <boost/log/sources/record_ostream.hpp>
# include <boost/log/utility/setup/file.hpp>
# include <boost/log/utility/setup/common_attributes.hpp>

BEGIN_GO_NAMESPACE
namespace tools
{
  BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_mt_type ){
    logger_mt_type lg;
    return(lg);
  }

  void init_log( const std::string& log_filename )
  {
    boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
    boost::log::add_file_log(
        boost::log::keywords::file_name = log_filename,
        boost::log::keywords::format = "[%TimeStamp%][%Severity%] %Message%"
    );
    boost::log::add_common_attributes();
  }
  void
  flush()
  {
    boost::log::core::get()->flush();
  }
}
END_GO_NAMESPACE
