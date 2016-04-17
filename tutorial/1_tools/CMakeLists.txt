add_executable( 1_log 1_log.cc )

target_link_libraries( 1_log ${GO_TOOLS_LIBRARIES} )
  #IMPORTED_LOCATION_DEBUGDebug ${GO_TOOLS_LIBRARIES_DEBUG}
  #Release ${GO_TOOLS_LIBRARIES_RELEASE}
  #RelWithDebInfo ${GO_TOOLS_LIBRARIES_RELWITHDEBINFO}  )

set_go_tutorial_properties( 1_log )
