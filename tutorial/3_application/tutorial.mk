application_tut_source_stems = $(notdir $(wildcard $(TUT_SRC_DIR)/3_application/*.cc))

# update global products variables with produced files
TUT_NAMES += 3_application 3_simple_gl_application
TUT_OBJ   += 
TUT_RULES += 
TUT_RES   += $(TUT_RES_DIR)/3_simple_gl_application

# define variable to use this library
APPLICATION_TUT_LD  = $(APPLICATION_LD) 
APPLICATION_TUT_INC = $(APPLICATION_INC)

3_application: application 3_simple_gl_application	
	
3_simple_gl_application: $(TUT_RES_DIR)/3_simple_gl_application 

$(TUT_SRC_DIR)/3_application/3_simple_gl_application_qtmakefile.mk: $(TUT_SRC_DIR)/3_application/tutorial.mk $(BIN_DIR)/build_conf.mk $(TUT_SRC_DIR)/3_application/3_simple_gl_application.cc $(TUT_SRC_DIR)/3_application/3_simple_gl_application.h $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pro
	@echo -e "\033[1;30m[: > host recipe test ] \033[0mqt makefile"
	@echo -e "QMAKE_CXXFLAGS += \\ \n    $(QMAKECXXFLAGS) \\ \n    $(APPLICATION_TUT_INC)" > $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
	@echo "QMAKE_LFLAGS += $(QMAKELFLAGS) $(APPLICATION_TUT_INC) $(APPLICATION_TUT_LD)" >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
ifeq ($(BUILD_CONF),debug)
	@echo "CONFIG         += debug" >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
else
	@echo -e "QMAKE_CXXFLAGS_RELEASE -= -O1\nQMAKE_CXXFLAGS_RELEASE -= -O2\nQMAKE_CXXFLAGS_RELEASE *= -O3" >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
	@echo -e "QMAKE_LFLAGS_RELEASE -= -Wl,-O1\nQMAKE_LFLAGS_RELEASE -= -Wl,-O2" >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
endif
	@echo -e "SOURCES += 3_simple_gl_application.cc" >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
	@echo -e "HEADERS += 3_simple_gl_application.h"  >> $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri	
	@here=$$(pwd); cd $(TUT_SRC_DIR)/3_application; \
		qmake -makefile -o 3_simple_gl_application_qtmakefile.mk 3_simple_gl_application.pro;\
		cd $$here

$(TUT_RES_DIR)/3_simple_gl_application: $(TUT_SRC_DIR)/3_application/3_simple_gl_application_qtmakefile.mk tools geometry application
	@echo -e "\033[1;38m[: > building tutorial ] \033[0m3_simple_gl_application"
	@mkdir -p $(TUT_RES_DIR)
	@here=$$(pwd); cd $(TUT_SRC_DIR)/3_application; \
		make -f 3_simple_gl_application_qtmakefile.mk;\
		cd $$here
	@$(STRIP) $(TUT_RES_DIR)/3_simple_gl_application
	