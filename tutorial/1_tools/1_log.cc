/* Created on: Dec 17, 2015
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <tools/log.h>
BEGIN_GO_NAMESPACE
static int
do_tutorial()
{
  LOG( trace, "this is a trace" );
  LOG( debug, "this is some debug info" );
  LOG( info, "this is an information")

  return EXIT_SUCCESS;
}

END_GO_NAMESPACE

int main( int argc, char* argv[] )
{
  (void)argc;
  (void)argv;
  return GO_NAMESPACE::do_tutorial();
}
