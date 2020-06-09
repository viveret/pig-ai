#####  SURVIVOR BUILD SYSTEM VERSION 8/16/14
#####  Updated 5/12/20 for PigAI

### Start by configuring os commands

RM =
FixPath =

#	Set RM and FixPath

ifdef SystemRoot # Only defined on Windows
   RM = del /Q
   FixPath = $(subst /,\,$1)
else
   ifeq ($(shell uname), Linux)
      RM = rm
      FixPath = $1
   endif
endif


###	Start configuration of build variables
SE_TARG_OS=ubuntu

CXX=
CXX_FLAGS=
CXX_LIBS=
CXX_INC=-Ijson/include -Itiny-dnn/ -Ipng++/
CXX_NOTIF=-Wall -Wextra -MMD -MP -Wno-comment -Wno-class-memaccess -Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter

OUTNAME_EXTENSION_EXE=

ifndef SE_TARG_OS
$(error SE_TARG_OS Not set)
endif
ifeq ($(SE_TARG_OS), win32)
	CXX=/usr/bin/i586-mingw32msvc-g++ -D_WIN32
	OUTNAME_EXTENSION_EXE=.exe
endif
ifeq ($(SE_TARG_OS), ubuntu)
	CXX=g++ -D_UBUNTU
	CXX_LIBS=-ldl -pthread -lsqlite3 -lpng -ljpeg -lz
endif
ifeq ( $(CXX), '' )
	$(error SE_TARG_OS Not set properly)
endif

### Start getting target info

OUTBUILD=obj/
OUTPATH=bin/
OUTNAME_GENERIC=
OUTNAME=
SE_TARG_NAME=pig-ai

### Calculate build variables

CXX_INC+=$(SE_TARG_INC) $(SE_PROJ_INC) 
CXX_FLAGS+=$(SE_PROJ_FLAGS) $(SE_TARG_FLAGS)
CXX_LIBS:=-L$(OUTPATH) $(SE_TARG_LIBS) $(SE_PROJ_LIBS) $(CXX_LIBS)

CXX_FLAGS +=-c -std=c++17 

### Calculate files for building

SRC=$(shell find src/ -name "*.cpp" )
SRC:=$(subst src/,,$(SRC))
OBJS=$(patsubst %.cpp, $(OUTBUILD)%.o, $(SRC))
DEPS=$(patsubst %.o, $(OUTBUILD)%.d, $(OBJS))


default: debug

debug: SE_TARG_NAME:=$(SE_TARG_NAME)-debug
debug: CXX_FLAGS+=-g3 -rdynamic
debug: obj bin exe

release: SE_TARG_NAME:=$(SE_TARG_NAME)-release
release: CXX_FLAGS+=-O3 
release: obj bin exe

obj:
	mkdir obj

bin:
	mkdir bin

exe: OUTNAME=$(OUTPATH)$(SE_TARG_NAME)$(OUTNAME_EXTENSION_EXE)
exe: $(OBJS)
	@mkdir -p $(dir $(OUTNAME))
	$(CXX) $(CXX_NOTIF) $(OBJS) $(CXX_LIBS) -o $(OUTNAME)


clean:
	$(RM) -r obj bin


obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(CXX_NOTIF) $(CXX_INC) $< -o $@
	@echo ' '

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(CXX_NOTIF) $(CXX_INC) $< -o $@
	@echo ' '
