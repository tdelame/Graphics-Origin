/* Created on: Dec 17, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/tools/log.h>
# include <omp.h>
# include <exception>
BEGIN_GO_NAMESPACE
static int
do_tutorial()
{
  LOG( trace, "this is a trace" );
  LOG( debug, "this is some debug info" );
  LOG( info, "this is an information");
  LOG( warning, "this is a warning" );
  LOG( error, "this is an error, the line and the file of the error are automatically displayed");
  LOG( fatal, "this is a fatal error, i.e. causing a program exit ")

  LOG( trace, "");
  LOG( info, "We demonstrate now that the log functionality can be used in different thread without problem")
  # pragma omp parallel
  for( int i = 0; i < 200; ++ i )
    {
      // You can see here the way to concatenate variable values and strings in the log entry
      LOG( trace, "thread #" << omp_get_thread_num() << " is at i = " << i );
    }

  LOG( trace, "");
  LOG( info, "We will now redirect the log entries to the file \"1_log.log\".");
  LOG( info, "Usually, this is done at the beginning of the application.")
  LOG( info, "Otherwise, the log entries are displayed in the standard output.");
  LOG( warning, "Last log entry in the standard output.");

  tools::init_log("1_log.log");
  LOG( info, "The functionalities are exactly the same as before.");

  try
    {
      LOG( info, "When the application stops, the last log entries could be lost.");
      LOG( info, "This could be an issue is the last entries explain why the program stops.");
      LOG( info, "To avoid this issue, you can catch any exception and flush the log entries before quitting.")
      LOG( fatal, "We simulate a problem by throwing an exception");
      throw std::runtime_error("an error");
    }
  catch( std::exception& e )
    {
      LOG( error, "the program stops because of: " << e.what() );
      LOG( info, "If you remove the try{}catch(){} structure, you will see that some log entries are lost")
      tools::flush_log();
      return EXIT_SUCCESS;
    }

  return EXIT_SUCCESS;
}

END_GO_NAMESPACE

int main( int argc, char* argv[] )
{
  (void)argc;
  (void)argv;
  return GO_NAMESPACE::do_tutorial();
}
