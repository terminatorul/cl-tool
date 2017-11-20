#include <memory>
#include <string>
#include <iostream>

#include "cl-matrix-mult.hh"

static char const * const program_text = 
"__kernel void random_fill_float_block(__global float *matrix, ulong lines, ulong cols, float minVal, float maxVal)\n"
"{\n"
"    ulong2 const work_item_size = (ulong2)( (lines + get_global_size(0) - 1) / get_global_size(0), (cols + get_global_size(1) - 1) / get_global_size(1));\n"
"    unsigned startLine = work_item_size.s0 * (get_global_id(0) - get_global_offset(0)) , startCol = work_item_size.s1 * (get_global_id(1) - get_global_offset(1));\n"
"    unsigned shift_register = (startLine ? startLine : (unsigned)(copysign(maxVal, (float)1.0f))) * (startCol ? startCol : (unsigned)(copysign(maxVal, (float)1.0f))) + (startLine > startCol ? 8 : 3);\n"
"    unsigned bit = 1;\n"
"\n"
"    shift_register *= (startLine ? startLine : (unsigned)(maxVal - minVal) / 2) * (startCol ? startCol : (unsigned)(maxVal - minVal) / 2) + abs_diff(startCol, startLine) / 3;\n"
"\n"
"    for (unsigned int i = startLine; i < min(lines, startLine + work_item_size.s0); i++)\n"
"	for (unsigned int j = startCol; j < min(cols, startCol + work_item_size.s1); j++)\n"
"	{\n"
"	    bit  = ((shift_register >> 0) ^ (shift_register >> 3) ^ (shift_register >> 5) ^ (shift_register >> 8) ) & 1;\n"
"	    shift_register =  (shift_register >> 1) | (shift_register << 31);\n"
"\n"
"	    matrix[i * cols + j] = minVal + fmod((float)(shift_register) / 100, maxVal - minVal);\n"
"	}\n"
"}\n"
"\n"
"/* __kernel void random_fill_double_block(__global double *matrix, ulong lines, ulong cols, double minVal, double maxVal)\n"
"{\n"
"    ulong2 const work_item_size = (ulong2)( (lines + get_global_size(0) - 1) / get_global_size(0), (cols + get_global_size(1) - 1) / get_global_size(1));\n"
"    unsigned startLine = work_item_size.s0 * (get_global_id(0) - get_global_offset(0)) , startCol = work_item_size.s1 * (get_global_id(1) - get_global_offset(1));\n"
"    double absMaxVal = copysign((float)maxVal, 1);\n"
"    unsigned shift_register = (startLine ? startLine : (unsigned)absMaxVal + 2) * (startCol ? startCol : (unsigned)absMaxVal + 2) + (startLine > startCol ? 8 : 3);\n"
"    unsigned bit = 1;\n"
"\n"
"    shift_register *= (startLine ? startLine : (unsigned)(maxVal - minVal) / 2) * (startCol ? startCol : (unsigned)(maxVal - minVal) / 2) + abs_diff(startCol, startLine) / 3;\n"
"\n"
"    for (unsigned int i = startLine; i < min(lines, startLine + work_item_size.s0); i++)\n"
"	for (unsigned int j = startCol; j < min(cols, startCol + work_item_size.s1); j++)\n"
"	{\n"
"	    bit  = ((shift_register >> 0) ^ (shift_register >> 3) ^ (shift_register >> 5) ^ (shift_register >> 8) ) & 1;\n"
"	    shift_register =  (shift_register >> 1) | (shift_register << 31);\n"
"\n"
"	    matrix[i * cols + j] = minVal + fmod((float)(shift_register) / 100.0, convert_float(maxVal) - convert_float(minVal));\n"
"	}\n"
"} */";

static cl::Program &build_program(cl::Program &program)
try
{
    program.build();

    return program;
}
catch(cl::Error const &error)
{
    if (error.err() == CL_BUILD_PROGRAM_FAILURE)
    {
#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
	auto const buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();

	if (!buildLog.empty())
	    for (auto const &output_msg: buildLog)
		std::cerr << "Build output:\n" << output_msg.second << std::endl;
#else
	std::string deviceBuildLog;

	program.getInfo(CL_PROGRAM_BUILD_LOG, &deviceBuildLog);
	std::cerr << "Build output:\n" << deviceBuildLog;
#endif
    }


    throw;
}

Matrix::Matrix(cl::Context &context)
    : cmdQueue(context), program(context, program_text, false), random_fill_float_block(build_program(program), "random_fill_float_block") //,
      // random_fill_double_block(program, "random_fill_double_block")
{
}
