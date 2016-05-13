add_executable( 3_simple_gl_application
  3_simple_gl_application.cc 3_simple_gl_application.h
  bumpy_torus.balls
  armadillo.off)

target_link_libraries( 3_simple_gl_application
  ${GO_APPLICATION_LIBRARIES})

set_go_tutorial_properties( 3_simple_gl_application )

add_custom_command( 
  TARGET 3_simple_gl_application POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/3_simple_gl_application.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
  ${CMAKE_CURRENT_SOURCE_DIR}/SceneWindow.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/Style.qml 
  ${CMAKE_CURRENT_SOURCE_DIR}/armadillo.off 
  $<TARGET_FILE_DIR:3_simple_gl_application>)