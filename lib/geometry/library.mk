geometry_source_stems = $(notdir $(wildcard $(LIB_SRC_DIR)/geometry/*.cc))
geometry_objects      = $(geometry_source_stems:%.cc=$(LIB_OBJ_DIR)/geometry/%.o)
geometry_rules        = $(geometry_source_stems:%.cc=$(LIB_RULES_DIR)/geometry/%.d)
geometry_cu_stems     = $(notdir $(wildcard $(LIB_SRC_DIR)/geometry/*.cu))
geometry_cu_objects   = $(geometry_cu_stems:%.cu=$(LIB_OBJ_DIR)/geometry/%.o)
geometry_cu_rules     = $(geometry_cu_stems:%.cu=$(LIB_RULES_DIR)/geometry/%.d)

# update global products variables with produced files
LIB_NAMES += geometry
LIB_OBJ   += $(geometry_objects) $(geometry_cu_objects)
LIB_RULES += $(geometry_rules) $(geometry_cu_rules)
LIB_RES   += libgeometry.so

# define variable to use this library
GEOMETRY_LD  = -L$(LIB_RES_DIR) -lgeometry $(BOOST_LD) $(TOOLS_LD) $(CUDA_LD)
GEOMETRY_INC = -I$(INC_DIR) $(BOOST_INC) $(TOOLS_INC) $(CUDA_INC)

# rules to generate dependencies 
$(LIB_RULES_DIR)/geometry/%.d: $(LIB_SRC_DIR)/geometry/%.cc
	@echo -e "\033[1;30m[: > host recipe geometry ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_RULES_DIR)/geometry
	@$(HOSTRECIPER) $(GEOMETRY_INC) $< -o $(LIB_RULES_DIR)/geometry/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(LIB_RULES_DIR)/geometry/$(*F).temp \
		> $@;
	@rm $(LIB_RULES_DIR)/geometry/$(*F).temp
	
$(LIB_RULES_DIR)/geometry/%.d: $(LIB_SRC_DIR)/geometry/%.cu
	@echo -e "\033[1;30m[: > device recipe geometry ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_RULES_DIR)/geometry
	@$(DEVICERECIPER) $(GEOMETRY_INC) $< -o $(LIB_RULES_DIR)/geometry/$(*F).temp
	@sed -e 's,\($$*\)\.o[ :]*,\1.o $@ : ,g' \
		< $(LIB_RULES_DIR)/geometry/$(*F).temp \
		> $@;
	@rm $(LIB_RULES_DIR)/geometry/$(*F).temp		
	
-include $(geometry_rules) $(geometry_cu_rules)

# rules	to generate objects
$(LIB_OBJ_DIR)/geometry/%.o: $(LIB_SRC_DIR)/geometry/%.cc $(LIB_RULES_DIR)/geometry/%.d 
	@echo -e "\033[1;38m[: > host compiling geometry ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_OBJ_DIR)/geometry
	@$(HOSTCOMPILER) $(GEOMETRY_INC) -o $@ $< 

$(LIB_OBJ_DIR)/geometry/%.o: $(LIB_SRC_DIR)/geometry/%.cu $(LIB_RULES_DIR)/geometry/%.d 
	@echo -e "\033[1;38m[: > device compiling geometry ] \033[0m$$(basename $<)"
	@mkdir -p $(LIB_OBJ_DIR)/geometry
	@$(DEVICECOMPILER) $(GEOMETRY_INC) -o $@ $< 
	
geometry: $(LIB_RES_DIR)/libgeometry.so

$(LIB_RES_DIR)/libgeometry.so: $(geometry_objects) $(geometry_cu_objects)
	@echo -e "\033[1;38m[: > building library ] \033[0mgeometry"
	@$(LINKER) -shared $(geometry_objects) $(geometry_cu_objects) -o libgeometry.so
	@mkdir -p $(LIB_RES_DIR)
	@$(STRIP) libgeometry.so
	@mv libgeometry.so $(LIB_RES_DIR)/
	