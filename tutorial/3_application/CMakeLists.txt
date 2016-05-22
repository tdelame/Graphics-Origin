add_executable( 3_simple_gl_application
  simple_camera.h  simple_qml_application.h  simple_gl_renderer.h  
  simple_camera.cc simple_qml_application.cc 3_simple_gl_application
  3_simple_gl_application.qml)

target_link_libraries( 3_simple_gl_application
  ${GO_APPLICATION_LIBRARIES})

set_go_tutorial_properties( 3_simple_gl_application )

set( SHADERS 
	${GO_SHADERS_DIR}/aabox.vert ${GO_SHADERS_DIR}/aabox.geom ${GO_SHADERS_DIR}/aabox.frag 
	${GO_SHADERS_DIR}/mesh.vert ${GO_SHADERS_DIR}/mesh_wireframe.geom ${GO_SHADERS_DIR}/mesh_wireframe.frag)

add_custom_command( TARGET 3_simple_gl_application 
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:3_simple_gl_application>/qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GO_QML_DIR} $<TARGET_FILE_DIR:3_simple_gl_application>/qml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/3_simple_gl_application.qml $<TARGET_FILE_DIR:3_simple_gl_application>/qml

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:3_simple_gl_application>/shaders
  	COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:3_simple_gl_application>/shaders

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:3_simple_gl_application>/meshes
    COMMAND ${CMAKE_COMMAND} -E copy ${GO_MESHES_DIR}/armadillo.off $<TARGET_FILE_DIR:3_simple_gl_application>/meshes
)
  