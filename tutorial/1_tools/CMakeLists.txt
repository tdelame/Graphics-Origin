add_executable( 1_log 1_log.cc )

target_link_libraries( 1_log ${GO_TOOLS_LIBRARIES} )

set_property( TARGET 1_log
  PROPERTY RUNTIME_OUTPUT_DIRECTORY ${GO_TUTORIAL_OUT_DIR})