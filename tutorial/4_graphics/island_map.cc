/* Created on: May 23, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/shader_program.h"
# include "../../graphics-origin/application/points_renderable.h"
# include "../../graphics-origin/application/lines_renderable.h"
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/tools/random.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

# include "common/simple_camera.h"
# include "common/simple_qml_application.h"
# include "common/simple_gl_renderer.h"

# include "island_map/island_map_builder.h"
# include "island_map/island_map.h"

# include <chrono>

# include <QGuiApplication>

//# define CGAL_LINKED_WITH_TBB
//# include <tbb/task_scheduler_init.h>
//# include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//# include <CGAL/Triangulation_vertex_base_with_info_2.h>
//# include <CGAL/Triangulation_face_base_with_info_2.h>
//# include <CGAL/Delaunay_triangulation_2.h>


namespace graphics_origin {
  namespace application {

//    class island_map {
//      struct vertex_info {
//        vertex_info()
//          : elevation{ 0 }, index{ 0 }, land{ false }
//        {}
//        vertex_info( size_t i )
//          : elevation{ 0 }, index{ i }, land{ false }
//        {}
//        gpu_real elevation;
//        size_t index;
//        bool land;
//      };
//
//      struct face_info {
//
//      };
//
//      typedef CGAL::Epick kernel;
//      typedef CGAL::Triangulation_vertex_base_with_info_2< vertex_info, kernel > dt_vertex_base;
//      typedef CGAL::Triangulation_face_base_with_info_2< face_info, kernel > dt_face_base;
//      typedef CGAL::Triangulation_data_structure_2< dt_vertex_base, dt_face_base > dt_datastructure;
//      typedef CGAL::Delaunay_triangulation_2< kernel, dt_datastructure > dt;
//
//
//    public:
//      struct generation_parameters {
//        generation_parameters()
//          : m_map_radius{ 6 },m_land_density{ 0.1 },
//            m_number_of_input_samples{100000},
//            m_lloyds_relaxations{ 2 }
//        {}
//        gpu_real m_map_radius;
//        gpu_real m_land_density; // should be positive, and less than 1.0
//        size_t m_number_of_input_samples;
//        uint8_t m_lloyds_relaxations;
//      };
//
//      island_map( const generation_parameters& params )
//      {
//        std::vector< dt::Point > input_points( params.m_number_of_input_samples, dt::Point{} );
//        std::vector< vertex_info > vertex_infos( params.m_number_of_input_samples, vertex_info{} );
//        # pragma omp parallel for
//        for( size_t i = 0; i < params.m_number_of_input_samples; ++ i )
//          {
//            input_points[ i ] = dt::Point{
//                (tools::unit_random() * 2 - 1) * params.m_map_radius,
//                (tools::unit_random() * 2 - 1) * params.m_map_radius };
//            vertex_infos[ i ] = vertex_info{ i };
//          }
//        m_delaunay.insert(
//            boost::make_zip_iterator(boost::make_tuple( input_points.begin(), vertex_infos.begin())),
//            boost::make_zip_iterator(boost::make_tuple( input_points.end(), vertex_infos.end())));
//
//        for( uint8_t k = 0; k < params.m_lloyds_relaxations; ++ k )
//          {
//            # pragma omp parallel
//            # pragma omp single
//            {
//              for( auto it = m_delaunay.finite_vertices_begin(), end = m_delaunay.finite_vertices_end();
//                  it != end; ++ it )
//                {
//                  # pragma omp task firstprivate(it)
//                  {
//                    size_t i = it->info().index;
//                    dt::Point& new_point = input_points[ i ];
//                    vec2 temp_point;
//                    size_t count = 0;
//
//                    auto circulator = m_delaunay.incident_faces( it ), start = circulator;
//                    do
//                      {
//                        if( !m_delaunay.is_infinite( circulator ) )
//                          {
//                            dt::Point dual = m_delaunay.dual( circulator );
//                            temp_point.x += CGAL::to_double(dual.x());
//                            temp_point.y += CGAL::to_double(dual.y());
//                             ++count;
//                          }
//                      }
//                    while( ++circulator != start );
//
//                    temp_point *= real(1.0) / real(count);
//                    new_point = dt::Point( temp_point.x, temp_point.y );
//                  }
//                }
//              # pragma omp taskwait
//            }
//            for( auto it = m_delaunay.finite_vertices_begin(), end = m_delaunay.finite_vertices_end();
//                it != end; ++ it )
//              {
//                m_delaunay.move_if_no_collision( it, input_points[ it->info().index ] );
//              }
//          }
//
//        noise::module::Perlin land_generator;
//        land_generator.SetFrequency( 1.0 );
//        land_generator.SetOctaveCount( 4 );
//        land_generator.SetSeed( std::chrono::system_clock::now().time_since_epoch().count() );
//        land_generator.SetNoiseQuality( noise::NoiseQuality::QUALITY_STD );
//
//        const float invr2 = float(1.0) / (params.m_map_radius * params.m_map_radius);
//        # pragma omp parallel
//        # pragma omp single
//        {
//          for( auto it = m_delaunay.finite_vertices_begin(), end = m_delaunay.finite_vertices_end();
//             it != end; ++ it )
//           {
//             # pragma omp task firstprivate(it)
//             {
//               dt::Point p = it->point();
//               float x = p.x();
//               float y = p.y();
//               float threshold = params.m_land_density + (x*x + y*y) * invr2;
//               it->info().land = land_generator.GetValue( x, y, 0 ) * 0.5f + 0.5f > threshold;
//             }
//           }
//        }
//      }
//
//      void load( points_renderable& points, lines_renderable& lines )
//      {
//        dt::Segment s;
//        for( auto eit = m_delaunay.finite_edges_begin(), end = m_delaunay.finite_edges_end();
//            eit != end; ++ eit )
//          {
//            bool take = false;
//            for( int i = 0; i < 3; ++ i )
//              {
//                if( i != eit->second && eit->first->vertex( i )->info().land )
//                  {
//                    take = true;
//                    break;
//                  }
//              }
//            if( take )
//              {
//                CGAL::Object o = m_delaunay.dual( eit );
//                if( CGAL::assign( s, o ) )
//                  {
//                    auto source = s.source(), target = s.target();
//                    lines.add( gpu_vec3{ source.x(), source.y(), 0 }, gpu_vec3{0,1,0},
//                               gpu_vec3{ target.x(), target.y(), 0 }, gpu_vec3{0,1,0} );
//                  }
//              }
//          }
//        for( auto it = m_delaunay.finite_vertices_begin(), end = m_delaunay.finite_vertices_end();
//            it != end; ++ it )
//          {
//            if( it->info().land )
//              {
//                auto point = it->point();
//                points.add( gpu_vec3{ point.x(), point.y(), 0 }, gpu_vec3{1,0,0});
//              }
//          }
//
//      }
//
//    private:
//      dt m_delaunay;
//    };

    class simple_gl_window
      : public gl_window {
    public:
      simple_gl_window( QQuickItem* parent = nullptr )
        : gl_window{ parent }
      {
        initialize_renderer( new simple_gl_renderer );
        std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
        shader_program_ptr samples_program =
            std::make_shared<shader_program>( std::list<std::string>{
              shader_directory + "flat.vert",
              shader_directory + "flat.frag"});

        island_map map;
        island_map_builder( island_map_builder::parameters{}, map );

//        island_map map( island_map::generation_parameters{} );
//        points_renderable* points = new points_renderable( samples_program, island_map::generation_parameters{}.m_number_of_input_samples );
//        lines_renderable* lines = new lines_renderable( samples_program );
//        map.load( *points, *lines );
//        add_renderable( points );
//        add_renderable( lines );
      }
    };

}}

int main( int argc, char* argv[] )
{
  // This is typically the place where you will analyze command-line arguments
  // such as to set a resources root directory.

  // Initialize the GUI application.
  QGuiApplication qgui( argc, argv );

  // Register C++ types to the QML engine: we would then be able to use those types in qml scripts.
  qmlRegisterType<graphics_origin::application::simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
  qmlRegisterType<graphics_origin::application::simple_camera   >( "GraphicsOrigin", 1, 0, "GLCamera" );

  // Load the main QML describing the main window into the simple QML application.
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "4_island_map.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
