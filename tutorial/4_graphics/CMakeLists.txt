set( SHADERS 
  ${GO_SHADERS_DIR}/mesh.vert ${GO_SHADERS_DIR}/mesh.frag
  ${GO_SHADERS_DIR}/transparent_window.vert ${GO_SHADERS_DIR}/transparent_window.geom ${GO_SHADERS_DIR}/transparent_window.frag
)

add_executable( 4_transparency
  4_transparency.cc
  simple_camera.h simple_camera.cc
  simple_qml_application.h simple_qml_application.cc
  ${SHADERS} )

target_link_libraries( 4_transparency ${GO_APPLICATION_LIBRARIES})

set_go_tutorial_properties( 4_transparency )

add_custom_command( TARGET 4_transparency 
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:4_transparency>/qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GO_QML_DIR} $<TARGET_FILE_DIR:4_transparency>/qml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/4_transparency.qml $<TARGET_FILE_DIR:4_transparency>/qml

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:4_transparency>/shaders
  	COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:4_transparency>/shaders

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:4_transparency>/meshes
    COMMAND ${CMAKE_COMMAND} -E copy ${GO_MESHES_DIR}/armadillo.off $<TARGET_FILE_DIR:4_transparency>/meshes
)
  
  
set( SHADERS
	${GO_SHADERS_DIR}/flat.vert ${GO_SHADERS_DIR}/flat.frag
)
set( QML
	${CMAKE_CURRENT_SOURCE_DIR}/4_island_map.qml
)

add_executable( 4_island_map
	4_island_map.cc
	simple_camera.h simple_camera.cc simple_qml_application.h simple_qml_application.cc simple_gl_renderer.h
	${SHADERS} ${QML}
)

set( CGAL_CORE_PACKAGE_DIR "/usr"
	CACHE FILEPATH "Variable to try to find CGAL_CORE package i.e. the directory where include/CGAL/CORE can be found" )

#TODO: I have done quite a mess to compile with the CGAL libraries. 
#TODO: clean that by using the cmake files given by CGAL or write another find package file for cgal

find_package( GMP REQUIRED )
find_package( MPFR REQUIRED )
find_package( TBB REQUIRED COMPONENTS tbbmalloc )
	
if( NOT GMP_FOUND )
  message( fatal "GMP not found")
endif()
if( NOT MPFR_FOUND )
  message( fatal "MPFR not found")
endif()

include_directories( ${GMP_INCLUDE_DIR} ${MPFR_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCGAL_DISABLE_ROUNDING_MATH_CHECK ${TBB_DEFINITIONS}")
set( GO_CGAL_LIBRARIES ${GMP_LIBRARIES} ${MPFR_LIBRARIES} -lCGAL -lCGAL_Core ${TBB_LIBRARIES} ${CGAL_CORE_LIBRARY})

# get it here: https://github.com/qknight/libnoise
find_package( LibNoise REQUIRED )
include_directories( ${LIBNOISE_INCLUDE_DIR} )
target_link_libraries( 4_island_map ${GO_APPLICATION_LIBRARIES} ${GO_CGAL_LIBRARIES} ${LIBNOISE_LIBRARIES})
set_go_tutorial_properties( 4_island_map )
add_custom_command( TARGET 4_island_map
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:4_island_map>/qml
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${GO_QML_DIR} $<TARGET_FILE_DIR:4_island_map>/qml
    COMMAND ${CMAKE_COMMAND} -E copy ${QML} $<TARGET_FILE_DIR:4_island_map>/qml

    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:4_island_map>/shaders
  	COMMAND ${CMAKE_COMMAND} -E copy ${SHADERS} $<TARGET_FILE_DIR:4_island_map>/shaders
)
  