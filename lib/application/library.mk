application_headers      = $(wildcard $(INC_DIR)/graphics-origin/application/*.h)
application_sources      = $(wildcard $(LIB_SRC_DIR)/application/*.cc)
application_source_stems = $(notdir $(application_sources))
application_header_stems = $(notdir $(application_headers))
application_objects      = $(application_source_stems:%.cc=$(LIB_OBJ_DIR)/application/%.o)

# update global products variables with produced files
LIB_NAMES  += application
LIB_OBJ    += $(application_objects) 
LIB_RULES  += 
LIB_RES    += libapplication.so
TEMP_FILES += $(LIB_SRC_DIR)/application/qtmakefile.mk $(LIB_SRC_DIR)/application/application.pri

# define variable to use this library
APPLICATION_COMPILATION_LD = $(GEOMETRY_LD) $(QT_LD) $(GL_LD) # $(BOOST_LD) $(TOOLS_LD)
APPLICATION_LD  = -L$(LIB_RES_DIR) -lapplication $(APPLICATION_COMPILATION_LD) $(GL_LD)
APPLICATION_INC = $(GEOMETRY_INC) $(QT_INC) $(GL_INC) #$(TOOLS_INC) $(BOOST_INC) -I$(INC_DIR)


$(LIB_SRC_DIR)/application/qtmakefile.mk: $(LIB_SRC_DIR)/application/library.mk $(LIB_SRC_DIR)/application/application.pro $(BIN_DIR)/build_conf.mk $(application_sources) $(application_headers) 
	@echo -e "\033[1;30m[: > host recipe application ] \033[0mqt makefile"
	@echo -e "QMAKE_CXXFLAGS += \\ \n    $(QMAKECXXFLAGS) \\ \n    $(APPLICATION_INC) -I../../" > $(LIB_SRC_DIR)/application/application.pri
	@echo -e "QMAKE_LFLAGS   += \\ \n    $(QMAKELFLAGS) \\ \n    $(APPLICATION_COMPILATION_LD)" >> $(LIB_SRC_DIR)/application/application.pri
ifeq ($(BUILD_CONF),debug)
	@echo "CONFIG  += debug" >> $(LIB_SRC_DIR)/application/application.pri
else
	@echo -e "QMAKE_CXXFLAGS_RELEASE -= -O1\nQMAKE_CXXFLAGS_RELEASE -= -O2\nQMAKE_CXXFLAGS_RELEASE *= -O3" >> $(LIB_SRC_DIR)/application/application.pri
	@echo -e "QMAKE_LFLAGS_RELEASE -= -Wl,-O1\nQMAKE_LFLAGS_RELEASE -= -Wl,-O2" >> $(LIB_SRC_DIR)/application/application.pri
endif
	@echo "SOURCES += \\" >> $(LIB_SRC_DIR)/application/application.pri
	@for file in $(application_source_stems); do echo "    $$file \\" >> $(LIB_SRC_DIR)/application/application.pri; done
	@echo -e "\nHEADERS += \\" >> $(LIB_SRC_DIR)/application/application.pri
	@for file in $(application_header_stems); do echo "    ../../graphics-origin/application/$$file \\" >> $(LIB_SRC_DIR)/application/application.pri; done
	@echo "" >> $(LIB_SRC_DIR)/application/application.pri
	@here=$$(pwd); cd $(LIB_SRC_DIR)/application; \
		qmake -makefile -o qtmakefile.mk application.pro;\
		cd $$here

application: $(LIB_RES_DIR)/libapplication.so

$(LIB_RES_DIR)/libapplication.so: $(LIB_SRC_DIR)/application/qtmakefile.mk tools geometry
	@echo -e "\033[1;38m[: > building library ] \033[0mapplication"
	@mkdir -p $(LIB_RES_DIR)
	@here=$$(pwd); cd $(LIB_SRC_DIR)/application; \
		make -f qtmakefile.mk;\
		cd $$here; $(STRIP) $(LIB_RES_DIR)/libapplication.so
	