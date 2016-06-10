add_executable( log 1_log.cc )

target_link_libraries( log ${GO_TOOLS_LIBRARIES} )
  #IMPORTED_LOCATION_DEBUGDebug ${GO_TOOLS_LIBRARIES_DEBUG}
  #Release ${GO_TOOLS_LIBRARIES_RELEASE}
  #RelWithDebInfo ${GO_TOOLS_LIBRARIES_RELWITHDEBINFO}  )

set_go_tutorial_properties( 
  NAME log 
  MODULE 1_tools )
