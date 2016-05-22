set( SHADERS 
  ${GO_SHADERS_DIR}/mesh.vert ${GO_SHADERS_DIR}/mesh.frag
  ${GO_SHADERS_DIR}/transparent_window.vert ${GO_SHADERS_DIR}/transparent_window.geom ${GO_SHADERS_DIR}/transparent_window.frag
)

add_executable( graphics_transparency
  transparency.cc
  simple_camera.h simple_camera.cc
  simple_qml_application.h simple_qml_application.cc
  ${SHADERS} )

target_link_libraries( graphics_transparency ${GO_APPLICATION_LIBRARIES})

set_go_tutorial_properties( graphics_transparency )

	


add_custom_command( TARGET graphics_transparency 
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:graphics_transparency>/qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GO_QML_DIR} $<TARGET_FILE_DIR:graphics_transparency>/qml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/graphics_transparency.qml $<TARGET_FILE_DIR:graphics_transparency>/qml

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:graphics_transparency>/shaders
  	COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:graphics_transparency>/shaders

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:graphics_transparency>/meshes
    COMMAND ${CMAKE_COMMAND} -E copy ${GO_MESHES_DIR}/armadillo.off $<TARGET_FILE_DIR:graphics_transparency>/meshes
)
  