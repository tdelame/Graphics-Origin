add_executable( bvh
  2_bvh.cc
  bumpy_torus.balls)

target_link_libraries( bvh ${GO_GEOMETRY_LIBRARIES})
  
set_go_tutorial_properties( NAME bvh MODULE 2_geometry )
  
add_executable( mesh 2_mesh.cc)
  
target_link_libraries( mesh ${GO_GEOMETRY_LIBRARIES})
set_go_tutorial_properties( NAME mesh MODULE 2_geometry )