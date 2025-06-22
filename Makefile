###############################################################################
# 	MAKEFILE V2.
#
#     	STUDENT:      Collin A. Bond---No additional group members.
#  	  PROFESSOR:      Dr. David Ely and Kira Klingenberg.
#      	 COURSE:      CS-350 Algorithms and Complexity.
#
###############################################################################



###############################################################################
# 1. MACROS / USER-DEFINED FUNCTIONS...
###############################################################################

# RECURSIVE WILDCARD.
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))



###############################################################################
#
#
#
#  2. MAKEFILE VARIABLES...
###############################################################################
###############################################################################

# 2.1. Program Directory Prefix-Names.
###############################################################################
prefix:=$(CURDIR)
exec_prefix:=#$(prefix)
datarootdir:=#$(prefix)/share


# 2.2. PROGRAM BUILD AND COMPILATION SETTINGS...
###############################################################################

# Auxiliary Compiler Flags.
I_FLAGS:=$(addprefix -I,$(INCLUDE_DIRS))
CPP_FLAGS:=$(I_FLAGS) -MMD -MP 
VALGRIND_FLAGS:=--track-origins=yes --tool=memcheck --leak-check=full

# Compilation Utilities.
CXX:=g++
DEFINES=-D _CBLIB_LOG
DEFINES_ALT=-D _CBLIB_DEBUG -D _CBLIB_GRAPH_UNIT_TEST
DEBUG_FLAGS=-g
OPT_FLAGS=-O0
OPT_LEVEL=-O2
WERROR_FLAGS=-Wall -Wextra -Wpedantic -Werror -Wshadow \
			 -Wconversion -Wsign-conversion -Wsign-promo \
			 -Wnon-virtual-dtor -Woverloaded-virtual -Wctor-dtor-privacy
			 
WNOERROR_FLAGS=-Wno-error=unused-variable -Wno-error=unused-function \
			   -Wno-error=unused-but-set-variable -Wno-error=unused-parameter \
			   -Wno-error=sign-conversion -Wno-error=unused-local-typedef \
			   -Wno-error=shorten-64-to-32
			   
FORMAT_FLAGS=-fmessage-length=0 -fdiagnostics-show-location=once \
	     -fdiagnostics-show-template-tree -fsanitize=undefined \
	     -fsanitize=float-divide-by-zero


CXX_FLAGS=$(DEFINES) $(DEBUG_FLAGS) $(OPT_FLAGS) $(WERROR_FLAGS) \
	  $(WNOERROR_FLAGS) $(FORMAT_FLAGS)

OTHER_FLAGS=-target x86_64-apple-darwin -std=c++20


# 2.3. Automatic Variables / Project Source Files.
###############################################################################
# Project Files.
SRC_FILES:=$(wildcard *.cpp)
OBJ_FILES:=$(SRC_FILES:.cpp=.o)
D_FILES:=$(OBJ_FILES:.o=.d)


# 2.4. Project Names.
###############################################################################
DATE:=$(shell date -u +"%Y-%m-%d")
TIME:=$(shell date -u +"TH:%M:%SZ")
DATETIME:=$(shell date -u +"%Y-%m-%dT%H:%M:%SZ")
AUTHOR_NAME:=COLLIN_BOND
PROJECT_NAME:=CBApp
EXEC_NAME:=a.out
TARGET_EXEC:=$(EXEC_NAME)
TEST_FILE:=test.py
TREE_NAME:=project_structure


# Misc Names.
TAR_NAME:=$(LOGNAME)-$(PROJECT_NAME)-$(DATE)
ZIP_NAME:=$(AUTHOR_NAME)_$(PROJECT_NAME)_$(DATE)
TREE_FILE:=$(DATE)_$(TREE_NAME).txt
#ZIP_NAME:=$(LOGNAME)-$(PROJECT_NAME)-$(DATE)

ARCHIVE_DIR:=$(prefix)/../.$(PROJECT_NAME)-Archives
ARCHIVE_GROUP_NAME:=$(LOGNAME)-$(PROJECT_NAME)-ARCHIVE-V0
ARCHIVE_GROUP:=$(ARCHIVE_DIR)/$(ARCHIVE_GROUP_NAME)
ARCHIVE_NAME:=$(ARCHIVE_GROUP)/$(PROJECT_NAME)-ARCHIVE


###############################################################################
#
#
#
###############################################################################
# 3. ALL MAKEFILE RULES...
###############################################################################
.PHONY: all xcode windows tree \
	opt clean cls \
	tar zip save archive



# "all"
#  Default Build Rule.
#
all: $(EXEC_NAME)

	
# Linking each of the Object Files (.o) into a Binary Executable.
$(EXEC_NAME): $(OBJ_FILES)
	@echo "linking binaries..."; echo ""
	@$(CXX) $(OTHER_FLAGS) $(CXX_FLAGS) $(OBJ_FILES) -o $@
	@echo "Build Complete."


# Compiling each of the Source Files (.cpp) into their respective Object Files (.o).
%.o: %.cpp
	@echo "building file [$(<)]..."; echo ""
	@$(CXX) $(OTHER_FLAGS) $(CPP_FLAGS) $(CXX_FLAGS) -c "$<" -o "$@"
	
-include $(D_FILES)


###############################################################################
#
#
#
# 3A. PRIMARY MAKEFILE RULES...
###############################################################################
###############################################################################

# "xcode"
#
xcode:
	@(cd build && cmake -G "Xcode" ..)
	

# "windows"
#
windows:
	./scripts/build/build_windows.sh
	
	
# "tree"
#
tree:
	tree -a -I 'build|.git|*.o|*.log|*.ttf' -o $(TREE_FILE)


###############################################################################
#
#
#
# 3B. ADDITIONAL ".PHONY" MAKEFILE RULES...
###############################################################################
###############################################################################
	
# "opt"
# Rule to Build Program With Compiler Optimizations Enabled.
#
opt: clean
	$(MAKE) OPT_FLAGS="$(OPT_LEVEL)"
	
	
# "clean" "cls"
#
clean cls:
	@rm -f $(EXEC_NAME) $(OBJ_FILES) $(D_FILES)
	

# "tar"
#
tar:
	tar cvfz $(TAR_NAME).tar.gz *.[ch] ?akefile

# "zip"
#
zip:
	bash -c 'zip -v $(ZIP_NAME).zip *.{c,cpp,h,hpp,py,pdf,txt,json,ini} ?akefile'


###############################################################################
#
#
#
#	...
###############################################################################
###############################################################################

# "save" "archive"
#  Rules for Saving / Archiving (making a copy of the contents of this Project).
#
save archive: $(ARCHIVE_NAME)
	@cp -a . $(ARCHIVE_NAME)/.

$(ARCHIVE_NAME): $(ARCHIVE_GROUP)
	@mkdir -p $@
$(ARCHIVE_GROUP): $(ARCHIVE_DIR)
	@mkdir -p $@
$(ARCHIVE_DIR):
	@mkdir -p $@
	
	
###############################################################################
#
#
#
###############################################################################
###############################################################################
#
# END MAKEFILE.
