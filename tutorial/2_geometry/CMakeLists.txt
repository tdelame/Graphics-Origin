add_executable( 2_bvh
  2_bvh.cc
  bumpy_torus.balls)

target_link_libraries( 2_bvh
  ${GO_GEOMETRY_LIBRARIES})
  
set_property( TARGET 2_bvh
  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${GO_TUTORIAL_OUT_DIR})
  
  
add_executable( 2_mesh
  2_mesh.cc)
  
target_link_libraries( 2_mesh
  ${GO_GEOMETRY_LIBRARIES})
set_property( TARGET 2_mesh
  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${GO_TUTORIAL_OUT_DIR} )
  