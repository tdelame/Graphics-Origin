tools_source_stems = $(notdir $(wildcard $(LIB_SRC_DIR)/tools/*.cc))
tools_objects      = $(tools_source_stems:%.cc=$(LIB_OBJ_DIR)/tools/%.o)
tools_rules        = $(tools_source_stems:%.cc=$(LIB_RULES_DIR)/tools/%.d)
tools_cu_stems     = $(notdir $(wildcard $(LIB_SRC_DIR)/tools/*.cu))
tools_cu_objects   = $(tools_cu_stems:%.cu=$(LIB_OBJ_DIR)/tools/%.o)
tools_cu_rules     = $(tools_cu_stems:%.cu=$(LIB_RULES_DIR)/tools/%.d)

# update global products variables with produced files
LIB_NAMES += tools
LIB_OBJ   += $(tools_objects) $(tools_cu_objects)
LIB_RULES += $(tools_rules) $(tools_cu_rules)
LIB_RES   += $(LIB_RES_DIR)/libtools.so

# define variable to use this library
TOOLS_LD  = -L$(LIB_RES_DIR) -ltools $(BOOST_LD)
TOOLS_INC = -I$(INC_DIR) $(BOOST_INC)

# rules to generate dependencies 
$(LIB_RULES_DIR)/tools/%.d: $(LIB_SRC_DIR)/tools/%.cc
	@echo -e "\033[1;30m[: > host recipe tools ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_RULES_DIR)/tools
	@$(HOSTRECIPER) $(TOOLS_INC) $< -o $(LIB_RULES_DIR)/tools/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(LIB_RULES_DIR)/tools/$(*F).temp \
		> $@;
	@rm $(LIB_RULES_DIR)/tools/$(*F).temp
	
$(LIB_RULES_DIR)/tools/%.d: $(LIB_SRC_DIR)/tools/%.cu
	@echo -e "\033[1;30m[: > device recipe tools ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_RULES_DIR)/tools
	@$(DEVICERECIPER) $(TOOLS_INC) $< -o $(LIB_RULES_DIR)/tools/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(LIB_RULES_DIR)/tools/$(*F).temp \
		> $@;
	@rm $(LIB_RULES_DIR)/tools/$(*F).temp		
	
-include $(tools_rules) $(tools_cu_rules)

# rules	to generate objects
$(LIB_OBJ_DIR)/tools/%.o: $(LIB_SRC_DIR)/tools/%.cc $(LIB_RULES_DIR)/tools/%.d 
	@echo -e "\033[1;38m[: > host compiling tools ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_OBJ_DIR)/tools
	@$(HOSTCOMPILER) $(TOOLS_INC) -o $@ $< 

$(LIB_OBJ_DIR)/tools/%.o: $(LIB_SRC_DIR)/tools/%.cu $(LIB_RULES_DIR)/tools/%.d 
	@echo -e "\033[1;38m[: > device compiling tools ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_OBJ_DIR)/tools
	@$(DEVICECOMPILER) $(TOOLS_INC) -o $@ $< 
	
tools: $(LIB_RES_DIR)/libtools.so

$(LIB_RES_DIR)/libtools.so: $(tools_objects) $(tools_cu_objects)
	@echo -e "\033[1;38m[: > building library ] \033[0mtools"
	@$(LINKER) -shared $(tools_objects) $(tools_cu_objects) -o libtools.so
	@mkdir -p $(LIB_RES_DIR)
	@$(STRIP) libtools.so
	@mv libtools.so $(LIB_RES_DIR)/
	