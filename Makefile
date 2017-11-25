
CC=g++
OBJCOPY=objcopy
NM=nm

SRC_DIR=..
OBJ_DIR=.
OPENCL_LIB_NAME=OpenCL

include $(SRC_DIR)/System$(OS).mak

CPPFLAGS:=$(CPPFALGS) -DCL_HPP_ENABLE_EXCEPTIONS -DCL_HPP_CL_1_2_DEFAULT_BUILD -Wno-ignored-attributes
CPPFLAGS:=$(CPPFLAGS) -DCL_HPP_TARGET_OPENCL_VERSION=200 -DCL_HPP_MINIMUM_OPENCL_VERSION=110 -DCL_HPP_CL_1_2_DEFAULT_BUILD
CPPFLAGS:=$(CPPFLAGS) $(OPENCL_CPP_FLAGS)
CXXFLAGS:=$(CXXFLAGS) -std=c++14
LIBS=-l$(OPENCL_LIB_NAME)
LDFLAGS:=$(LDFLAGS) $(LIBS) $(OPENCL_LD_FLAGS)


CL_TOOL_HEADERS= \
	${SRC_DIR}/cl-matrix-mult.hh \
	${SRC_DIR}/cl-platform-info.hh

CL_TOOL_SOURCES= \
	$(CL_TOOL_HEADERS:.hh=.cc) \
	${SRC_DIR}/cl-tool.cc

CL_TOOL_OBJECTS= \
	${OBJ_DIR}/cl-matrix-mult${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-platform-info${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-tool${OBJ_SUFFIX}

all: ${OBJ_DIR}/cl-tool${EXE_SUFFIX} ${OBJ_DIR}/cl-matrix-rand.cl

${OBJ_DIR}/cl-tool$(OBJ_SUFFIX): ${SRC_DIR}/cl-tool.cc $(CL_TOOL_HEADERS)
	$(NIX_CMD) $(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-tool.cc
	$(WIN_CMD) $(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@.broken ${SRC_DIR}/cl-tool.cc
	$(WIN_CMD) $(ComSpec) /V:ON /C For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^^^| findstr .weak.`) Do @Echo --weaken-symbol=%%i >"${OBJ_DIR}\weakSym_$(@F).txt"
	$(WIN_CMD) $(OBJCOPY) @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-matrix-mult$(OBJ_SUFFIX): ${SRC_DIR}/cl-matrix-mult.cc ${SRC_DIR}/cl-matrix-mult.hh
	$(NIX_CMD) $(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-matrix-mult.cc
	$(WIN_CMD) $(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@.broken ${SRC_DIR}/cl-matrix-mult.cc
	$(WIN_CMD) $(ComSpec) /V:ON /C For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^^^| findstr .weak.`) Do @Echo --weaken-symbol=%%i >"${OBJ_DIR}\weakSym_$(@F).txt"
	$(WIN_CMD) $(OBJCOPY) @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-platform-info$(OBJ_SUFFIX): ${SRC_DIR}/cl-platform-info.cc ${SRC_DIR}/cl-platform-info.hh
	$(NIX_CMD) $(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-platform-info.cc
	$(WIN_CMD) "$(CC)" $(CPPFLAGS) $(CXXFLAGS) -c -o "$@.broken" "${SRC_DIR}/cl-platform-info.cc"
	$(WIN_CMD) "$(ComSpec)" /V:ON /C For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^^^| findstr .weak.`) Do @Echo --weaken-symbol=%%i >"${OBJ_DIR}\weakSym_$(@F).txt"
	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-tool$(EXE_SUFFIX): $(CL_TOOL_OBJECTS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(CL_TOOL_OBJECTS) $(LDFLAGS)

${OBJ_DIR}/cl-matrix-rand.cl: ${SRC_DIR}/cl-matrix-rand.cl
	${WIN_CMD} If Not Exist "$(@D)\$(@F)" ($(MKLINK_CMD) "$(@D)\$(@F)" "$(?D)\$(?F)")
	${NIX_CMD} $(LN_CMD) "$?" "$@"

clean:
	$(WIN_CMD) For %%i in ("${OBJ_DIR}\*.exe" "${OBJ_DIR}\*.obj" "${OBJ_DIR}\*.cl" "$(OBJ_DIR)\*.obj.broken" "${OBJ_DIR}\weakSym_*.txt") Do (If Exist "%%~i" ($(RM_CMD) "%%~i"))
	$(NIX_CMD) $(RM_CMD) "${OBJ_DIR}/cl-tool$(EXE_SUFFIX)" ${CL_TOOL_OBJECTS} "${OBJ_DIR}/cl-matrix-rand.cl"
