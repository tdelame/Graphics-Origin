add_executable( 3_simple_gl_application
  3_simple_gl_application.cc 3_simple_gl_application.h
  bumpy_torus.balls
  armadillo.off)

target_link_libraries( 3_simple_gl_application
  ${GO_APPLICATION_LIBRARIES})
  
set_property( TARGET 3_simple_gl_application
  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${GO_TUTORIAL_OUT_DIR})