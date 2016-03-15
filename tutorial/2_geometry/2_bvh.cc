/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/geometry/bvh.h"

# include <omp.h>

# include <list>
# include <fstream>
# include <sstream>

#include <valgrind/callgrind.h>

# include <chrono>
# include <random>



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

  class old_ball
    : public bounding_box_computer,
      public vec4 {
  public:
    virtual ~old_ball(){}
      /**Creates unit ball*/
    old_ball() noexcept : vec4{}
    {}

    old_ball( const vec3& center, const real& radius ) noexcept
        : vec4{ center, radius }
    {}
    old_ball( const old_ball&& other ) noexcept: vec4{ other }
    {}

    old_ball( const old_ball& other ) noexcept: vec4{ other }
    {}
    old_ball( const vec4& b ) noexcept: vec4{ b }
    {}

    old_ball&
    operator=( const old_ball&& other ) noexcept{
      vec4::operator=( std::move( other ) );
      return *this;
    }

    bool intersect( const ball& b ) const {
      auto diff = vec4{ b.x - x, b.y - y, b.z - z, b.w + b.w };
      return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z < diff.w * diff.w;
    }
    bool intersect( const aabox& b ) const
    {
      auto ball_interiority = w * w;
      auto diff = glm::abs( vec3{*this} - b.get_center()) - b.get_half_sides();
      for( int i = 0; i < 3; ++ i )
        if( diff[i] > 0 )
          {
            ball_interiority -= diff[i] * diff[i];
          }
      return ball_interiority >= 0;
    }
  private:
    void do_compute_bounding_box( aabox& b ) const{
      b = aabox{ vec3{*this}, vec3{w,w,w} };
    }
  };

  real unit_random()
  {
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count() );
    static std::uniform_real_distribution<real> distribution( real(0), real(1));
    return distribution(generator);
  }

  static int execute()
  {
    std::vector< ball > balls;
//    read_balls_file( balls, "tutorial/2_geometry/bumpy_torus.balls");
    {
      static const size_t nbballs = 10000000;
      balls.resize( nbballs );
      for( auto& b : balls )
        {
          b = vec4{ unit_random(), unit_random(), unit_random(), unit_random() };
        }
    }

    std::vector< old_ball > old_balls( balls.size(), old_ball{} );
    for( size_t i = 0; i < balls.size(); ++ i )
      {
        old_balls[i] = std::move(vec4{ balls[i] });
      }

    LOG( debug, "NEW ONE:");
    auto start = omp_get_wtime();
    CALLGRIND_START_INSTRUMENTATION;
    bvh<aabox> box_bvh_of_balls( balls.data(), balls.size() );
    CALLGRIND_STOP_INSTRUMENTATION;
    auto new_perf = omp_get_wtime() - start;
    LOG( info, box_bvh_of_balls.get_number_of_nodes() << " nodes in " << new_perf << " second");
    LOG( debug, "OLD ONE:");
    start = omp_get_wtime();
    CALLGRIND_START_INSTRUMENTATION;
    box_bvh old_box_bvh_of_balls( old_balls.data(), old_balls.size());
    CALLGRIND_STOP_INSTRUMENTATION;
    auto old_perf = omp_get_wtime() - start;
    LOG( info, old_box_bvh_of_balls.get_number_of_nodes() << " nodes in " << old_perf << " second");


    LOG( debug, "sizeof new ball = " << sizeof( ball ) );
    LOG( debug, "sizeof old ball = " << sizeof( old_ball ));

    return 0;
  }
}}

int main( int argc, char* argv[] )
{
  return graphics_origin::geometry::execute();
}


