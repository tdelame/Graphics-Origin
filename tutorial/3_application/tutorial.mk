application_tut_source_stems = $(notdir $(wildcard $(TUT_SRC_DIR)/3_application/*.cc))
application_tut_objects      = $(application_tut_source_stems:%.cc=$(TUT_OBJ_DIR)/application/%.o)
application_tut_rules        = $(application_tut_source_stems:%.cc=$(TUT_RULES_DIR)/application/%.d)
application_tut_cu_stems     = $(notdir $(wildcard $(TUT_SRC_DIR)/3_application/*.cu))
application_tut_cu_objects   = $(application_tut_cu_stems:%.cu=$(TUT_OBJ_DIR)/application/%.o)
application_tut_cu_rules     = $(application_tut_cu_stems:%.cu=$(TUT_RULES_DIR)/application/%.d)

# update global products variables with produced files
TUT_NAMES += 3_application
TUT_OBJ   += $(application_tut_objects) $(application_tut_cu_objects)
TUT_RULES += $(application_tut_rules) $(application_tut_cu_rules)
TUT_RES   += 3_simple_gl_application

# define variable to use this library
APPLICATION_TUT_LD  = $(APPLICATION_LD) 
APPLICATION_TUT_INC = $(APPLICATION_INC)

# rules to generate dependencies 
#$(TUT_RULES_DIR)/3_application/%.d: $(TUT_SRC_DIR)/3_application/%.cc
#	@echo -e "\033[1;30m[: > host recipe application tutorial ] \033[0m$$(basename $<)"
#	@mkdir -p $(TUT_RULES_DIR)/3_application
#	@$(HOSTRECIPER) $(APPLICATION_TUT_INC) $< -o $(TUT_RULES_DIR)/3_application/$(*F).temp
#	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
#		< $(TUT_RULES_DIR)/3_application/$(*F).temp \
#		> $@;
#	@rm $(TUT_RULES_DIR)/3_application/$(*F).temp
#	
#$(TUT_RULES_DIR)/3_application/%.d: $(TUT_SRC_DIR)/3_application/%.cu
#	@echo -e "\033[1;30m[: > device recipe application tutorial ] \033[0m$$(basename $<)"
#	@mkdir -p $(TUT_RULES_DIR)/3_application
#	@$(DEVICERECIPER) $(APPLICATION_TUT_INC) $< -o $(TUT_RULES_DIR)/3_application/$(*F).temp
#	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
#		< $(TUT_RULES_DIR)/3_application/$(*F).temp \
#		> $@;
#	@rm $(TUT_RULES_DIR)/3_application/$(*F).temp		
#	
#-include $(application_tut_rules) $(application_tut_cu_rules)
#
## rules	to generate objects
#$(TUT_OBJ_DIR)/3_application/%.o: $(TUT_SRC_DIR)/3_application/%.cc $(TUT_RULES_DIR)/3_application/%.d 
#	@echo -e "\033[1;38m[: > host compiling application tutorial ] \033[0m$$(basename $<)"
#	@mkdir -p $(TUT_OBJ_DIR)/3_application
#	@$(HOSTCOMPILER) $(APPLICATION_TUT_INC) -o $@ $< 
#
#$(TUT_OBJ_DIR)/3_application/%.o: $(TUT_SRC_DIR)/3_application/%.cu $(TUT_RULES_DIR)/3_application/%.d 
#	@echo -e "\033[1;38m[: > device compiling application tutorial ] \033[0m$$(basename $<)"
#	@mkdir -p $(TUT_OBJ_DIR)/3_application
#	@$(DEVICECOMPILER) $(APPLICATION_TUT_INC) -o $@ $< 
	
3_application: application 3_simple_gl_application	
	
3_simple_gl_application: $(TUT_RES_DIR)/3_simple_gl_application tools geometry application

$(TUT_SRC_DIR)/3_application/3_simple_gl_application_qtmakefile.mk: $(TUT_SRC_DIR)/3_application/tutorial.mk $(BIN_DIR)/build_conf.mk $(TUT_SRC_DIR)/3_application/3_simple_gl_application.cc $(TUT_SRC_DIR)/3_application/3_simple_gl_application.h $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pro
	@echo -e "\033[1;30m[: > host recipe test ] \033[0mqt makefile"
	@echo "QMAKE_CXXFLAGS += $(QMAKECXXFLAGS) $(APPLICATION_TUT_INC)" > $(TUT_SRC_DIR)/3_application/3_simple_gl_application.pri
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

$(TUT_RES_DIR)/3_simple_gl_application: $(TUT_SRC_DIR)/3_application/3_simple_gl_application_qtmakefile.mk application
	@echo -e "\033[1;38m[: > building tutorial ] \033[0m3_application"
	@mkdir -p $(TUT_RES_DIR)
	@here=$$(pwd); cd $(TUT_SRC_DIR)/3_application; \
		make -f 3_simple_gl_application_qtmakefile.mk;\
		cd $$here
	@$(STRIP) 3_simple_gl_application
	@mv 3_simple_gl_application $(TUT_RES_DIR)/
	