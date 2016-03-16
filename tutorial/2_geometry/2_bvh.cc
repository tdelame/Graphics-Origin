/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/geometry/bvh.h"

# include <list>
# include <fstream>
# include <sstream>

namespace graphics_origin { namespace geometry {
  std::string get_next_line( std::ifstream& file, size_t& cline )
  {
    std::string result = "";
    do
      {
        if (file.eof() || file.fail())
          return "";
        getline(file, result);
        ++cline;

        std::string::size_type pos = 0;
        for( auto c : result )
          {
            if( c == ' ' || c == '\t' )
              ++pos;
            else break;
          }
        result = result.substr( pos );

      }
    while (result == "" || result[0] == '#');
    size_t found = result.find('#');
    if (found != std::string::npos)
      result = result.substr(0, found);
    return result;
  }
  static void read_balls_file( std::vector< ball >& balls, const std::string& filename )
  {
    size_t nb_balls = 0;
    size_t nline = 0;
    std::ifstream input(filename);

    {
      std::istringstream tokenizer( get_next_line( input, nline ) );
      tokenizer >> nb_balls;
    }
    balls.resize( nb_balls );
    for( size_t i = 0; i < nb_balls; ++ i )
      {
        std::string line_string = get_next_line( input, nline );
        std::istringstream tokenizer( line_string );
        vec4 b;
        tokenizer >> b.x >> b.y >> b.z >> b.w;
        if( tokenizer.fail() )
          {
            LOG( error, "incorrect data at line " << nline << " [" << line_string << "]");
          }
        else
          {
            balls[i] =  std::move(b);
          }
      }
    input.close();
  }

  static int execute()
  {
    std::vector< ball > balls;
    read_balls_file( balls, "tutorial/2_geometry/bumpy_torus.balls");
    bvh<aabox> box_bvh_of_balls( balls.data(), balls.size() );

    return 0;
  }
}}

int main( int argc, char* argv[] )
{
  return graphics_origin::geometry::execute();
}


