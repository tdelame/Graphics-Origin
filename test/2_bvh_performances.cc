# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/geometry/bvh.h"
# include "2_bvh_performances/bvh_check.h"
# include "2_bvh_performances/bvh_new.h"
# include "../../graphics-origin/tools/resources.h"
# include <omp.h>
# include <iostream>

namespace graphics_origin {
  namespace geometry {
    static const size_t number_of_input_meshes = 3;
    static const std::string mesh_names[] = {
        "armadillo.off", "Bunny.obj", "spot_triangulated.obj"
    };

    struct bvh_results {
      double construction;
      bool is_tree;
      bool one_leaf_by_object;

      bvh_results() :
        construction{0}, is_tree{false}, one_leaf_by_object{false}
      {}

      template< typename bvh_type >
      bvh_results( double time, bvh_type& tree ) :
        construction{ time },
        is_tree{ check_is_tree( tree ) },
        one_leaf_by_object{ check_one_leaf_by_object( tree ) }
      {}

      void print()
      {
        std::cout
          << "  construction       " << construction << "\n"
          << "  is tree            " << (is_tree ? "OK\n" : "FAIL\n")
          << "  one leaf by object " << (one_leaf_by_object ? "OK\n" : "FAIL\n");
      }
    };

    struct test_results {
      std::string mesh_name;
      size_t number_of_triangles;
      bvh_results ground_truth;
      bvh_results candidate;
      bool are_equal;

      test_results( const std::string& name, size_t nb_triangles ) :
        mesh_name{name}, number_of_triangles{nb_triangles}, are_equal{ false }
      {}

      void print()
      {
        std::cout
          << "Results for " << mesh_name << "\n"
          << "- #triangles " << number_of_triangles << "\n"
          << "- equal " << (are_equal ? "YES\n" : "NO\n")
          << "- ground_truth\n";
        ground_truth.print();
        std::cout
          << "- candidate\n";
        candidate.print();
      }
    };

    static int execute()
    {
      const std::string mesh_dir = tools::get_path_manager().get_resource_directory( "meshes" );

      for( size_t i = 0; i < number_of_input_meshes; ++ i )
        {
          mesh input( mesh_dir + mesh_names[i] );

          const size_t nfaces = input.n_faces();
          std::vector< triangle > triangles( nfaces );
          # pragma omp parallel for
          for (size_t i = 0; i < nfaces; ++i)
            {
              mesh::FaceVertexIter it = input.fv_begin( mesh::FaceHandle(i) );
              auto& p1 = input.point( *it ); ++ it;
              auto& p2 = input.point( *it ); ++ it;
              auto& p3 = input.point( *it );

              triangles[ i ] = triangle(
                  vec3{ p1[0], p1[1], p1[2] },
                  vec3{ p2[0], p2[1], p2[2] },
                  vec3{ p3[0], p3[1], p3[2] } );
            }

          test_results results( mesh_names[i], nfaces );

          {
            double start = omp_get_wtime();
            bvh<aabox> candidate( triangles.data(), nfaces );
            double stop = omp_get_wtime();
            results.candidate = bvh_results( stop - start, candidate );
          }

          {
            double start = omp_get_wtime();
            bvh_new<aabox> ground_truth( triangles.data(), nfaces );
            double stop = omp_get_wtime();
            results.ground_truth = bvh_results( stop - start, ground_truth );
          }

          {
            bvh<aabox> candidate( triangles.data(), nfaces );
            bvh_new<aabox> ground_truth( triangles.data(), nfaces );
            results.are_equal = are_equal( candidate, ground_truth );
          }
          results.print();
        }
      return EXIT_SUCCESS;
    }
  }
}
int main( int argc, char* argv[] )
{
  (void)argc;
  (void)argv;
  return graphics_origin::geometry::execute();
}
