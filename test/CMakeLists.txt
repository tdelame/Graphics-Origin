add_executable( 3_simple_gl_application
  simple_camera.h  simple_qml_application.h  simple_gl_renderer.h  
  simple_camera.cc simple_qml_application.cc 3_simple_gl_application
  meshes qml shaders)

target_link_libraries( 3_simple_gl_application
  ${GO_APPLICATION_LIBRARIES})

set_go_tutorial_properties( 3_simple_gl_application )

add_custom_command( 
  TARGET 3_simple_gl_application POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/meshes $<TARGET_FILE_DIR:3_simple_gl_application>/meshes
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/qml $<TARGET_FILE_DIR:3_simple_gl_application>/qml
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:3_simple_gl_application>/shaders)