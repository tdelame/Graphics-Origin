add_executable( 2_bvh
  2_bvh.cc
  bumpy_torus.balls)

target_link_libraries( 2_bvh
  ${GO_GEOMETRY_LIBRARIES})
  
set_go_tutorial_properties( 2_bvh )
  
add_executable( 2_mesh
  2_mesh.cc)
  
target_link_libraries( 2_mesh
  ${GO_GEOMETRY_LIBRARIES})
set_go_tutorial_properties( 2_mesh )