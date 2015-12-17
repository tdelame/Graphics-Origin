#
# 1. Directories
################################################################################
# sources
INC_DIR				= include
LIB_SRC_DIR		= lib
TUT_SRC_DIR		= tutorial
TEST_SRC_DIR	= test

# build type configuration
BIN_DIR				= bin
BUILD_CONF		= ""
-include $(BIN_DIR)/build_conf.mk
ifeq ($(BUILD_CONF),"")
BUILD_CONF 	= release
else 
ifneq ($(BUILD_CONF),release)
BUILD_CONF  = debug 
endif
endif 

# library output directories
LIB_OBJ_DIR		= $(BIN_DIR)/$(BUILD_CONF)/obj
LIB_RULES_DIR	= $(BIN_DIR)/$(BUILD_CONF)/rules
LIB_RES_DIR		= $(BIN_DIR)/$(BUILD_CONF)/lib

# tutorial output directories
TUT_OBJ_DIR		= $(BIN_DIR)/$(BUILD_CONF)/tutorial/obj
TUT_RULES_DIR	= $(BIN_DIR)/$(BUILD_CONF)/tutorial/rules
TUT_RES_DIR		= $(BIN_DIR)/$(BUILD_CONF)/tutorial/bin

# test output directories
TEST_OBJ_DIR		= $(BIN_DIR)/$(BUILD_CONF)/test/obj
TEST_RULES_DIR	= $(BIN_DIR)/$(BUILD_CONF)/test/rules
TEST_RES_DIR		= $(BIN_DIR)/$(BUILD_CONF)/test/bin

#
# 2. Commands
################################################################################
AR		= ar
CXX		= g++
LD		= $(CXX)
NVCC	= nvcc
QMAKE	= qmake
ifeq ($(BUILD_CONF), debug)
STRIP = touch
else
STRIP	= strip --strip-debug --strip-unneeded
endif
#
# 3. Global command options
################################################################################
ARFLAGS		= rs
CXXFLAGS	= -frounding-math -std=c++14 -fPIC -fopenmp
LDFLAGS		= -L/usr/lib -L/usr/local/lib -L$(LIB_RES_DIR)
NVCCFLAGS = -Xcompiler -fopenmp -Xcompiler '-fPIC' --use_fast_math -m64 \
						-arch sm_21 --ptxas-options="-v"
ifeq ($(BUILD_CONF),release)
CXXFLAGS += -O3 -msse2
NVCCFLAGS+= -Xcompiler -O3 --optimize 3
else
CXXFLAGS += -ggdb -Wall -Wextra -Wctor-dtor-privacy -Wwrite-strings -fno-inline \
            -fno-inline-functions -fno-inline-small-functions
NVCCFLAGS+= -O0 --debug -Xlinker -ggdb -Xlinker -O0 -G -g -Xcompiler -fno-inline \
            -Xcompiler -fno-inline-functions -Xcompiler -fno-inline-small-functions            
endif					

#
# 4. External libraries
################################################################################
# openmesh
OPENMESH_INC = -isystem /usr/local/include/OpenMesh
OPENMESH_LD  = -L/usr/local/lib -L/usr/local/lib/OpenMesh -lOpenMeshCore

# boost
BOOST_INC		 = 
BOOST_LD		 = -lboost_serialization -lboost_filesystem -lpthread \
							 -lboost_log -lboost_log_setup -lboost_locale -lboost_random \
							 -lboost_thread -lboost_system

# qt
QT_INC			 = -isystem /usr/include/qt -isystem /usr/include/qt/QtQuick \
							 -isystem /usr/include/qt/QtGui -isystem /usr/include/qt/QtQml \
							 -isystem /usr/include/qt/QtNetwork -isystem /usr/include/qt/QtCore \
							 -I/usr/lib/qt/mkspecs/linux-g++
QT_LD				 = -lQt5Quick -lQt4Gui -lQt5Qml -lQt5Network -lQt5Core

#
# 5. Compilation flags
################################################################################
CPPFLAGS = $(CUDA_INCLUDES) \
           -DBOOST_LOG_DYN_LINK -DBOOST_ALL_DYN_LINK \
				   -DCGAL_DISABLE_ROUNDING_MATH_CHECK \
				   -DGLM_FORCE_SSE2 -DGLM_FORCE_RADIANS  -DGLM_PRECISION_HIGHP_FLOAT

ifeq ($(BUILD_CONF), release)
CPPFLAGS          += -UDEBUG -DNDEBUG -DCGAL_NDEBUG -DNO_DEBUG -DEIGEN_NO_DEBUG
else
CPPFLAGS          += -DDEBUG -O0 
endif

#
# 6. Shortcuts
################################################################################
HOSTCOMPILER   = $(CXX) $(CXXFLAGS) -c $(CPPFLAGS)
DEVICECOMPILER = $(NVCC) $(NVCCFLAGS) -dc
LINKER         = $(NVCC) $(NVCCFLAGS) $(LDFLAGS)
HOSTRECIPER    = $(CXX) $(CXXFLAGS) -M $(CPPFLAGS)
DEVICERECIPER  = $(NVCC) $(NVCCFLAGS) -M
define uniq =
  $(eval seen :=)
  $(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))
  ${seen}
endef

#
# 7. Products
################################################################################
LIB_OBJ    =
LIB_RULES  =
LIB_RES    =
LIB_NAMES  =
LIB_MK     = $(wildcard $(LIB_SRC_DIR)/*/library.mk)

TUT_OBJ    =
TUT_RULES  =
TUT_RES    =
TUT_NAMES  =
TUT_MK     = $(wildcard $(TUT_SRC_DIR)/*/tutorial.mk)

TEST_OBJ   =
TEST_RULES =
TEST_RES   =
TEST_NAMES =
TEST_MK    = $(wildcard $(TEST_SRC_DIR)/*/test.mk)

#
# 8. Rules
################################################################################
-include $(LIB_MK)
-include $(TUT_MK)
-include $(TEST_MK)

.SUFFIXES:
.SUFFIXES: .cc .d .h .cu .cuh .pro
.PHONY: all strap clean $(LIB_NAMES) $(TUT_NAMES) $(TEST_NAMES)
.SECONDARY:
.DEFAULT_GOAL :=
.DEFAULT_GOAL := all


all: libraries tutorial test

libraries: $(LIB_NAMES)

tutorial: $(TUT_NAMES)

test: $(TEST_NAMES)

strap:
	@echo Build configuration \[$(BUILD_CONF)\] activated and ready
	
clean:
	@echo "To clean, remove bin/ manually"
