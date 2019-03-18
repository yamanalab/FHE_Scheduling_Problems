# 
# makefile for C++
# 

#------ user settings -----

## output program name
PROGRAM_NAME := run

## output program directory
OUTPUT_DIR := .

## object(.o) file and dependency(.d) file directory
TMP_DIR := $(OUTPUT_DIR)/.build_obj

## source file directories
SRC_DIRS := .

## include search directories
INCLUDE_DIRS := ..

## debug or release
BUILD_TYPE := debug
#BUILD_TYPE := release

## c++ compiler
CXX := g++

## compiler options (-Wall etc.)
CXXFLAGS := -Wall -Wextra -pedantic-errors -std=c++98 

## extra options when debug mode
CXX_DEBUG_FLAGS := -O0 -g3

## extra options when release mode
CXX_RELEASE_FLAGS := -O3

## link options
LINK_FLAGS := 

CLEAN_TARGET := $(TMP_DIR) 

#------ end user settings -----




#----- automatic settings -----
TARGET   := $(OUTPUT_DIR)/$(PROGRAM_NAME)
SRCS     := $(foreach directory, $(SRC_DIRS), $(wildcard $(directory)/*.cpp))
VPATH    := $(SRC_DIRS)
ifeq ($(BUILD_TYPE), debug)
    CXXFLAGS  += $(CXX_DEBUG_FLAGS)
else ifeq ($(BUILD_TYPE), release)
    CXXFLAGS  += $(CXX_RELEASE_FLAGS)
else
    $(error BUILD_TYPE must be release or debug)
endif
OBJS     := $(foreach src, $(SRCS), $(TMP_DIR)/$(src).o)
DEPS     := $(OBJS:%.o=%.d)
INCLUDE  := $(foreach directory, $(INCLUDE_DIRS), -I$(directory))

CLEAN_TARGET += $(TARGET).dSYM

#----- recipes -----
.PHONY: all clean destroy scan debugrun test
all: $(TARGET)

## link object files
$(TARGET): $(OBJS) 
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(LINK_FLAGS) -o $@ $(OBJS)
	@printf "\e[35m=== build success! ===\e[m\n"

## compile c++ source file
$(TMP_DIR)/%.cpp.o: %.cpp 
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
		
## create dependency
$(TMP_DIR)/%.cpp.d: %.cpp 
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -MM -MP $(INCLUDE) $< | \
    sed -e '1s:\(.*\)\.o:$(@:%.d=%.o):' > $@


## remove .o .d and execute file
clean: 
	rm -rf $(TARGET) $(CLEAN_TARGET)

## scan project
scan:
	@scan-build make

## for debugging
debugrun:
	valgrind --leak-check=full --show-reachable=no ./$(TARGET) $(FLAG)

## test of makefile
test:
	@echo "[make debug]"
	@echo "   [TEST = " $(TEST) "]"
	@echo "   [SRCS = " $(SRCS) "]"
	@echo "   [OBJS = " $(OBJS) "]"
	@echo "   [DEPS = " $(DEPS) "]"

-include $(DEPS)

