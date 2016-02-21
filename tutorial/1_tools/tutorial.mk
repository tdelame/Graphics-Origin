tools_tut_source_stems = $(notdir $(wildcard $(TUT_SRC_DIR)/1_tools/*.cc))
tools_tut_objects      = $(tools_tut_source_stems:%.cc=$(TUT_OBJ_DIR)/tools/%.o)
tools_tut_rules        = $(tools_tut_source_stems:%.cc=$(TUT_RULES_DIR)/tools/%.d)
tools_tut_cu_stems     = $(notdir $(wildcard $(TUT_SRC_DIR)/1_tools/*.cu))
tools_tut_cu_objects   = $(tools_tut_cu_stems:%.cu=$(TUT_OBJ_DIR)/tools/%.o)
tools_tut_cu_rules     = $(tools_tut_cu_stems:%.cu=$(TUT_RULES_DIR)/tools/%.d)

# update global products variables with produced files
TUT_NAMES += 1_log
TUT_OBJ   += $(tools_tut_objects) $(tools_tut_cu_objects)
TUT_RULES += $(tools_tut_rules) $(tools_tut_cu_rules)
TUT_RES   += $(TUT_RES_DIR)/1_log

# define variable to use this library
TOOLS_TUT_LD  = $(TOOLS_LD)
TOOLS_TUT_INC = $(TOOLS_INC)

# rules to generate dependencies 
$(TUT_RULES_DIR)/1_tools/%.d: $(TUT_SRC_DIR)/1_tools/%.cc
	@echo -e "\033[1;30m[: > host recipe tools tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_RULES_DIR)/1_tools
	@$(HOSTRECIPER) $(TOOLS_TUT_INC) $< -o $(TUT_RULES_DIR)/1_tools/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(TUT_RULES_DIR)/1_tools/$(*F).temp \
		> $@;
	@rm $(TUT_RULES_DIR)/1_tools/$(*F).temp
	
$(TUT_RULES_DIR)/1_tools/%.d: $(TUT_SRC_DIR)/1_tools/%.cu
	@echo -e "\033[1;30m[: > device recipe tools tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_RULES_DIR)/1_tools
	@$(DEVICERECIPER) $(TOOLS_TUT_INC) $< -o $(TUT_RULES_DIR)/1_tools/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(TUT_RULES_DIR)/1_tools/$(*F).temp \
		> $@;
	@rm $(TUT_RULES_DIR)/1_tools/$(*F).temp		
	
-include $(tools_tut_rules) $(tools_tut_cu_rules)

# rules	to generate objects
$(TUT_OBJ_DIR)/1_tools/%.o: $(TUT_SRC_DIR)/1_tools/%.cc $(TUT_RULES_DIR)/1_tools/%.d 
	@echo -e "\033[1;38m[: > host compiling tools tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_OBJ_DIR)/1_tools
	@$(HOSTCOMPILER) $(TOOLS_TUT_INC) -o $@ $< 

$(TUT_OBJ_DIR)/1_tools/%.o: $(TUT_SRC_DIR)/1_tools/%.cu $(TUT_RULES_DIR)/1_tools/%.d 
	@echo -e "\033[1;38m[: > device compiling tools tutorial ] \033[0m$$(basename $<)"
	@mkdir -p $(TUT_OBJ_DIR)/1_tools
	@$(DEVICECOMPILER) $(TOOLS_TUT_INC) -o $@ $< 
	
1_log: $(TUT_RES_DIR)/1_log 

$(TUT_RES_DIR)/1_log: $(TUT_OBJ_DIR)/1_tools/1_log.o tools
	@echo -e "\033[1;38m[: > building tutorial ] \033[0m1_log"
	@$(LINKER) $(TUT_OBJ_DIR)/1_tools/1_log.o $(TOOLS_LD) -o 1_log
	@mkdir -p $(TUT_RES_DIR)
	@$(STRIP) 1_log
	@mv 1_log $(TUT_RES_DIR)/
	