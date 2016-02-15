application_tut_source_stems = $(notdir $(wildcard $(TUT_SRC_DIR)/3_application/*.cc))
application_tut_objects      = $(application_tut_source_stems:%.cc=$(TUT_OBJ_DIR)/application/%.o)
application_tut_rules        = $(application_tut_source_stems:%.cc=$(TUT_RULES_DIR)/application/%.d)
application_tut_cu_stems     = $(notdir $(wildcard $(TUT_SRC_DIR)/3_application/*.cu))
application_tut_cu_objects   = $(application_tut_cu_stems:%.cu=$(TUT_OBJ_DIR)/application/%.o)
application_tut_cu_rules     = $(application_tut_cu_stems:%.cu=$(TUT_RULES_DIR)/application/%.d)

# update global products variables with produced files
TUT_NAMES += application
TUT_OBJ   += $(application_tut_objects) $(application_tut_cu_objects)
TUT_RULES += $(application_tut_rules) $(application_tut_cu_rules)
TUT_RES   += 3_simple_gl_application

# define variable to use this library
APPLICATION_TUT_LD  = $(APPLICATION_LD)
APPLICATION_TUT_INC = $(APPLICATION_INC)

# rules to generate dependencies 
$(TUT_RULES_DIR)/3_application/%.d: $(TUT_SRC_DIR)/3_application/%.cc
	@echo -e "\033[1;30m[: > host recipe application tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_RULES_DIR)/3_application
	@$(HOSTRECIPER) $(APPLICATION_TUT_INC) $< -o $(TUT_RULES_DIR)/3_application/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(TUT_RULES_DIR)/3_application/$(*F).temp \
		> $@;
	@rm $(TUT_RULES_DIR)/3_application/$(*F).temp
	
$(TUT_RULES_DIR)/3_application/%.d: $(TUT_SRC_DIR)/3_application/%.cu
	@echo -e "\033[1;30m[: > device recipe application tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_RULES_DIR)/3_application
	@$(DEVICERECIPER) $(APPLICATION_TUT_INC) $< -o $(TUT_RULES_DIR)/3_application/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(TUT_RULES_DIR)/3_application/$(*F).temp \
		> $@;
	@rm $(TUT_RULES_DIR)/3_application/$(*F).temp		
	
-include $(application_tut_rules) $(application_tut_cu_rules)

# rules	to generate objects
$(TUT_OBJ_DIR)/3_application/%.o: $(TUT_SRC_DIR)/3_application/%.cc $(TUT_RULES_DIR)/3_application/%.d 
	@echo -e "\033[1;38m[: > host compiling application tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_OBJ_DIR)/3_application
	@$(HOSTCOMPILER) $(APPLICATION_TUT_INC) -o $@ $< 

$(TUT_OBJ_DIR)/3_application/%.o: $(TUT_SRC_DIR)/3_application/%.cu $(TUT_RULES_DIR)/3_application/%.d 
	@echo -e "\033[1;38m[: > device compiling application tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_OBJ_DIR)/3_application
	@$(DEVICECOMPILER) $(APPLICATION_TUT_INC) -o $@ $< 
	
3_simple_gl_application: $(TUT_RES_DIR)/3_simple_gl_application

$(TUT_RES_DIR)/3_simple_gl_application: $(TUT_OBJ_DIR)/3_application/3_simple_gl_application.o
	@echo -e "\033[1;38m[: > building tutorial ] \033[0m3_application"
	@$(LINKER) $(TUT_OBJ_DIR)/3_application/3_application.o $(APPLICATION_LD) -o 3_simple_gl_application
	@mkdir -p $(TUT_RES_DIR)
	@$(STRIP) 3_simple_gl_application
	@mv 3_log $(TUT_RES_DIR)/
	