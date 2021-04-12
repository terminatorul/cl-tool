
CC=gcc
CXX=g++
OBJCOPY=objcopy
NM=nm

SRC_DIR=..
OBJ_DIR=.
OPENCL_LIB_NAME=OpenCL

include $(SRC_DIR)/System$(OS).mak

CPPFLAGS:=$(CPPFALGS) -DCL_HPP_ENABLE_EXCEPTIONS -DCL_HPP_CL_1_2_DEFAULT_BUILD -Wno-ignored-attributes
CPPFLAGS:=$(CPPFLAGS) -DCL_HPP_TARGET_OPENCL_VERSION=200 -DCL_HPP_MINIMUM_OPENCL_VERSION=110 -DCL_HPP_CL_1_2_DEFAULT_BUILD
CPPFLAGS:=$(CPPFLAGS) -DCL_TARGET_OPENCL_VERSION=220
CPPFLAGS:=$(CPPFLAGS) $(OPENCL_CPP_FLAGS)
CXXFLAGS:=$(CXXFLAGS) -std=c++14
LIBS=-l$(OPENCL_LIB_NAME)
LDFLAGS:=$(LDFLAGS) $(LIBS) $(OPENCL_LD_FLAGS)

CL_TOOL_HEADERS= \
	${SRC_DIR}/cl-matrix-mult.hh \
	${SRC_DIR}/cl-double-pendulum.hh \
	${SRC_DIR}/cl-platform-info.hh \
	${SRC_DIR}/cl-platform-probe.hh \
	${SRC_DIR}/cl-user-selection.hh \
	${SRC_DIR}/parse-cmd-line.hh

CL_TOOL_SOURCES= \
	$(CL_TOOL_HEADERS:.hh=.cc) \
	${SRC_DIR}/cl-tool.cc

CL_TOOL_OBJECTS= \
	${OBJ_DIR}/cl-matrix-mult${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-double-pendulum${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-platform-info${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-platform-probe${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-user-selection${OBJ_SUFFIX} \
	${OBJ_DIR}/parse-cmd-line${OBJ_SUFFIX} \
	${OBJ_DIR}/cl-tool${OBJ_SUFFIX}

all: ${OBJ_DIR}/cl-tool${EXE_SUFFIX} ${OBJ_DIR}/cl-matrix-rand.cl ${OBJ_DIR}/cl-double-pendulum.cl

icd_headers:=$(SRC_DIR)/OpenCL-Headers $(SRC_DIR)/OpenCL-CLHPP

$(SRC_DIR)/OpenCL-Headers:
	git -C $(SRC_DIR) submodule update --init OpenCL-Headers

$(SRC_DIR)/OpenCL-CLHPP:
	git -C $(SRC_DIR) submodule update --init OpenCL-CLHPP

${OBJ_DIR}/cl-tool$(OBJ_SUFFIX): ${SRC_DIR}/cl-tool.cc $(SRC_DIR)/cl-platform-info.hh $(SRC_DIR)/cl-platform-probe.hh $(SRC_DIR)/cl-user-selection.hh $(SRC_DIR)/parse-cmd-line.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-tool.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-tool.cc
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-user-selection$(OBJ_SUFFIX): ${SRC_DIR}/cl-user-selection.cc ${SRC_DIR}/cl-user-selection.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-user-selection.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o "$@" "${SRC_DIR}/cl-user-selection.cc"
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/parse-cmd-line$(OBJ_SUFFIX): ${SRC_DIR}/parse-cmd-line.cc ${SRC_DIR}/parse-cmd-line.hh $(SRC_DIR)/cl-platform-info.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/parse-cmd-line.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o "$@" "${SRC_DIR}/parse-cmd-line.cc"
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-matrix-mult$(OBJ_SUFFIX): ${SRC_DIR}/cl-matrix-mult.cc ${SRC_DIR}/cl-matrix-mult.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-matrix-mult.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-matrix-mult.cc
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i )>"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) $(OBJCOPY) @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-double-pendulum$(OBJ_SUFFIX): ${SRC_DIR}/cl-double-pendulum.cc ${SRC_DIR}/cl-double-pendulum.hh $(SRC_DIR)/cl-matrix-mult.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-double-pendulum.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-double-pendulum.cc
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-platform-info$(OBJ_SUFFIX): ${SRC_DIR}/cl-platform-info.cc ${SRC_DIR}/cl-platform-info.hh $(SRC_DIR)/cl-double-pendulum.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-platform-info.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o "$@" "${SRC_DIR}/cl-platform-info.cc"
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) "$(OBJCOPY)" @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

${OBJ_DIR}/cl-platform-probe$(OBJ_SUFFIX): ${SRC_DIR}/cl-platform-probe.cc $(SRC_DIR)/cl-platform-probe.hh $(SRC_DIR)/cl-matrix-mult.hh $(SRC_DIR)/cl-double-pendulum.hh $(icd_headers)
	$(NIX_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ ${SRC_DIR}/cl-platform-probe.cc
	$(WIN_CMD) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o "$@" "${SRC_DIR}/cl-platform-probe.cc"
# 	$(WIN_CMD) (For /F "usebackq tokens=1,*" %%i In (`$(NM) "$@.broken" --format POSIX ^| findstr .weak.`) Do @Echo --weaken-symbol=%%i) >"${OBJ_DIR}\weakSym_$(@F).txt"
# 	$(WIN_CMD) $(OBJCOPY) @"${OBJ_DIR}\weakSym_$(@F).txt" "$@.broken" "$@"
# 	$(WIN_CMD) $(RM_CMD) "$@.broken" "${OBJ_DIR}\weakSym_$(@F).txt"

$(SRC_DIR)/OpenCL-ICD-Loader:
	git -C $(SRC_DIR) submodule update --init OpenCL-ICD-Loader

.PHONY: build_icd

OpenCL-ICD-Loader/bin/$(DLL_PREFIX)OpenCL$(DLL_SUFFIX): build_icd

build_icd: $(SRC_DIR)/OpenCL-ICD-Loader
	$(NIX_CMD) mkdir -parents OpenCL-ICD-Loader
	$(WIN_CMD) If Not Exist OpenCL-ICD-Loader (MkDir OpenCL-ICD-Loader)
	$(NIX_CMD) if ! test -e OpenCL-ICD-Loader/CMakeCache.txt; then  cmake -S $(SRC_DIR)/OpenCL-ICD-Loader -B OpenCL-ICD-Loader $(ICD_LOADER_GENERATOR) -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_SHARED_LIBRARY_PREFIX=$(DLL_PREFIX) -DCMAKE_SHARED_LIBRARY_PREFIX_C=$(DLL_PREFIX) -DCMAKE_C_FLAGS="-DCL_TARGET_OPENCL_VERSION=220 -I$(SRC_DIR)/OpenCL-Headers -I$$(PWD)/$(SRC_DIR)/OpenCL-Headers"
	$(WIN_CMD) If Not Exist OpenCL-ICD-Loader\CMakeCache.txt cmd /C cmake -S $(SRC_DIR)/OpenCL-ICD-Loader -B OpenCL-ICD-Loader $(ICD_LOADER_GENERATOR) -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_SHARED_LIBRARY_PREFIX=$(DLL_PREFIX) -DCMAKE_SHARED_LIBRARY_PREFIX_C=$(DLL_PREFIX) -DCMAKE_C_FLAGS="-DCL_TARGET_OPENCL_VERSION=220 -I$(SRC_DIR)/OpenCL-Headers -I%CD%/$(SRC_DIR)/OpenCL-Headers"
	$(NIX_CMD) cmake --build OpenCL-ICD-Loader
	$(WIN_CMD) cmake --build OpenCL-ICD-Loader --target OpenCL

${OBJ_DIR}/cl-tool$(EXE_SUFFIX): $(CL_TOOL_OBJECTS) OpenCL-ICD-Loader/bin/$(DLL_PREFIX)OpenCL$(DLL_SUFFIX)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(CL_TOOL_OBJECTS) $(LDFLAGS)

${OBJ_DIR}/cl-matrix-rand.cl: ${SRC_DIR}/cl-matrix-rand.cl
	${WIN_CMD} If Not Exist "$(@D)\$(@F)" ($(MKLINK_CMD) /H "$(@D)\$(@F)" "$(?D)\$(?F)")
	${NIX_CMD} $(LN_CMD) "$?" "$@"

${OBJ_DIR}/cl-double-pendulum.cl: ${SRC_DIR}/cl-double-pendulum.cl
	${WIN_CMD} If Not Exist "$(@D)\$(@F)" ($(MKLINK_CMD) /H "$(@D)\$(@F)" "$(?D)\$(?F)")
	${NIX_CMD} $(LN_CMD) "$?" "$@"

clean:
	$(WIN_CMD) If Exist OpenCL-ICD-Loader\CMakeCache.txt cmake --build OpenCL-ICD-Loader --target clean
	$(WIN_CMD) For %%i in ("${OBJ_DIR}\*.exe" "${OBJ_DIR}\*.obj" "${OBJ_DIR}\*.cl" "$(OBJ_DIR)\*.obj.broken" "${OBJ_DIR}\weakSym_*.txt") Do (If Exist "%%~i" ($(RM_CMD) "%%~i"))
	$(NIX_CMD) $(RM_CMD) "${OBJ_DIR}/cl-tool$(EXE_SUFFIX)" ${CL_TOOL_OBJECTS} "${OBJ_DIR}/cl-matrix-rand.cl" "${OBJ_DIR}/cl-double-pendulum.cl"
