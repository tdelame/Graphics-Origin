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
# OPENMESH_INCLUDE_DIR  OpenMesh include directory (to add to include_directories())
# OPENMESH_LIBRARIES    OpenMesh libraries (to add to target_link_libraries())

find_path( OPENMESH_INCLUDE_DIR OpenMesh/Core/Mesh/PolyMeshT.hh
  PATHS
    /usr/local/include
    /usr/include
    /opt/local/include
    /opt/include
    $ENV{OPENMESH_DIR}/src
    $ENV{OPENMESH_DIR}/include
    ${OPENMESH_DIR}
    ${OPENMESH_DIR}/include
    ${OPENMESH_DIR}/src
)

IF (OPENMESH_INCLUDE_DIR)
	IF (WIN32)
	   SET(OPENMESH_LIBRARY_DIR "${OPENMESH_INCLUDE_DIR}/../lib")
	ELSE (WIN32)
	   SET(OPENMESH_LIBRARY_DIR "${OPENMESH_INCLUDE_DIR}/../lib/OpenMesh")
	ENDIF (WIN32)

	FIND_LIBRARY(OPENMESH_CORE_LIBRARY_RELEASE NAMES OpenMeshCore libOpenMeshCore PATHS ${OPENMESH_LIBRARY_DIR})
	FIND_LIBRARY(OPENMESH_TOOLS_LIBRARY_RELEASE NAMES OpenMeshTools libOpenMeshTools PATHS ${OPENMESH_LIBRARY_DIR})
	SET(OPENMESH_LIBRARY_RELEASE
		${OPENMESH_CORE_LIBRARY_RELEASE}
		${OPENMESH_TOOLS_LIBRARY_RELEASE})
	    
	FIND_LIBRARY(OPENMESH_CORE_LIBRARY_DEBUG NAMES OpenMeshCored libOpenMeshCored PATHS ${OPENMESH_LIBRARY_DIR})
	FIND_LIBRARY(OPENMESH_TOOLS_LIBRARY_DEBUG NAMES OpenMeshToolsd libOpenMeshToolsd PATHS ${OPENMESH_LIBRARY_DIR})
	SET(OPENMESH_LIBRARY_DEBUG
		${OPENMESH_CORE_LIBRARY_DEBUG}
		${OPENMESH_TOOLS_LIBRARY_DEBUG})
ENDIF (OPENMESH_INCLUDE_DIR)

if(OPENMESH_LIBRARY_RELEASE)
  if(OPENMESH_LIBRARY_DEBUG)
    set(OPENMESH_LIBRARIES_ optimized ${OPENMESH_LIBRARY_RELEASE} debug ${OPENMESH_LIBRARY_DEBUG})
  else()
    set(OPENMESH_LIBRARIES_ ${OPENMESH_LIBRARY_RELEASE})
  endif()

  set(OPENMESH_LIBRARIES ${OPENMESH_LIBRARIES_} CACHE FILEPATH "The OpenMesh library")
endif()

IF(OPENMESH_INCLUDE_DIR AND OPENMESH_LIBRARIES)
	SET(OPENMESH_FOUND TRUE)
	if( NOT OpenMesh_FIND_QUIETLY )
	  MESSAGE(STATUS "Found OpenMesh: ${OPENMESH_LIBRARIES}")
	endif()
else()
  if( OpenMesh_FIND_REQUIRED )
    message( SEND_ERROR "Cannot find OpenMesh. Make sure it is installed and set OPENMESH_DIR variable to give a hint about its location")
  endif()
ENDIF(OPENMESH_INCLUDE_DIR AND OPENMESH_LIBRARIES)
