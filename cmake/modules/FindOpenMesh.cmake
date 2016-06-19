# FindOpenMesh.cmake
# ==================
# Find the OpenMesh library.
#
# Filepath
# --------
# OPENMESH_DIR hint to find the OpenMesh installation path, i.e. where to find
#   either OpenMesh/Core/Mesh/PolyMeshT.hh
# Variables defined
# -----------------
# OPENMESH_FOUND        true if OpenMesh was found
# OPENMESH_INCLUDE_DIRS OpenMesh include directory (to add to include_directories())
# OPENMESH_LIBRARIES    OpenMesh libraries (to add to target_link_libraries())

find_path( OPENMESH_INCLUDE_DIRS OpenMesh/Core/Mesh/PolyMeshT.hh
  PATHS
    $ENV{OPENMESH_DIR}/src
    $ENV{OPENMESH_DIR}/include
    ${OPENMESH_DIR}
    ${OPENMESH_DIR}/include
    ${OPENMESH_DIR}/src
    /usr/local/include
    /usr/include
    /opt/local/include
    /opt/include
)

if (OPENMESH_INCLUDE_DIRS)
  find_library( OPENMESH_CORE_LIBRARY_RELEASE
    NAMES OpenMeshCore libOpenMeshCore
    PATHS
      ${OPENMESH_INCLUDE_DIRS}/../lib 
      ${OPENMESH_INCLUDE_DIRS}/../lib/OpenMesh
  )
  find_library( OPENMESH_TOOLS_LIBRARY_RELEASE
    NAMES OpenMeshTools libOpenMeshTools
    PATHS
      ${OPENMESH_INCLUDE_DIRS}/../lib 
      ${OPENMESH_INCLUDE_DIRS}/../lib/OpenMesh
  )
  find_library( OPENMESH_CORE_LIBRARY_DEBUG
    NAMES OpenMeshCored libOpenMeshCored
    PATHS
      ${OPENMESH_INCLUDE_DIRS}/../lib 
      ${OPENMESH_INCLUDE_DIRS}/../lib/OpenMesh
  )
  find_library( OPENMESH_TOOLS_LIBRARY_DEBUG
    NAMES OpenMeshTools libOpenMeshToolsd
    PATHS
      ${OPENMESH_INCLUDE_DIRS}/../lib 
      ${OPENMESH_INCLUDE_DIRS}/../lib/OpenMesh
  )  

  if( OPENMESH_CORE_LIBRARY_RELEASE AND OPENMESH_TOOLS_LIBRARY_RELEASE )
    if( OPENMESH_CORE_LIBRARY_DEBUG AND OPENMESH_TOOLS_LIBRARY_DEBUG )
      set( OPENMESH_LIBRARIES_ 
        optimized ${OPENMESH_CORE_LIBRARY_RELEASE} ${OPENMESH_TOOLS_LIBRARY_RELEASE}
        debug ${OPENMESH_CORE_LIBRARY_DEBUG} ${OPENMESH_TOOLS_LIBRARY_DEBUG})
    else()  
      set( OPENMESH_LIBRARIES ${OPENMESH_CORE_LIBRARY_RELEASE} ${OPENMESH_TOOLS_LIBRARY_RELEASE} )
    endif()
    set( OPENMESH_FOUND TRUE )
    if( NOT OpenMesh_FIND_QUIETLY )
      message( STATUS "Found OpenMesh: ${OPENMESH_LIBRARIES}")
    endif()
  else( )
    if( OPENMESH_CORE_LIBRARY_DEBUG AND OPENMESH_TOOLS_LIBRARY_DEBUG )
      set( OPENMESH_LIBRARIES ${OPENMESH_CORE_LIBRARY_DEBUG} ${OPENMESH_TOOLS_LIBRARY_DEBUG})
      set( OPENMESH_FOUND TRUE )
      if( NOT OpenMesh_FIND_QUIETLY )
        message( STATUS "Found OpenMesh: ${OPENMESH_LIBRARIES}")
      endif()
    else()  
      message( SEND_ERROR "Cannot find OpenMesh tools and core libraries in ${OPENMESH_INCLUDE_DIRS}/../lib and ${OPENMESH_INCLUDE_DIRS}/../lib/Openmesh" )
    endif()
  endif( )

else( OPENMESH_INCLUDE_DIRS )
  if( OpenMesh_FIND_REQUIRED )
    message( SEND_ERROR "Cannot find OpenMesh. Make sure it is installed and set OPENMESH_DIR variable to give a hint about its location")
  endif()
endif( OPENMESH_INCLUDE_DIRS )

mark_as_advanced( OPENMESH_CORE_LIBRARY_RELEASE OPENMESH_CORE_LIBRARY_DEBUG OPENMESH_TOOLS_LIBRARY_DEBUG OPENMESH_TOOLS_LIBRARY_RELEASE )
