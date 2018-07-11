#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "cl-matrix-mult.hh"

static std::string readSourceFile(char const *file_name)
{
    std::ifstream sourceFile(file_name);
    std::ostringstream sourceText;

    sourceFile.exceptions(sourceFile.exceptions() | sourceFile.badbit | sourceFile.failbit);
    sourceText << sourceFile.rdbuf();
    
    return sourceText.str();
}

static char const program_file_name[] = "./cl-matrix-rand.cl";

enum class FloatType { Half, Single, Double, Quad };

char const *float_type_name(FloatType floatType)
{
    switch (floatType)
    {
	case FloatType::Half:
	    return "half";
	case FloatType::Single:
	    return "float";
	case FloatType::Double:
	    return "double";
	case FloatType::Quad:
	    return "quad";
    }

    return "-";
}

static cl::Program &build_program(cl::Program &program, FloatType floatType)
try
{
    program.build((std::string("-cl-std=CL1.1 -DFLOAT_TYPE=") + float_type_name(floatType)).c_str());

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
		std::cerr << "Build output from device " << output_msg.first.getInfo<CL_DEVICE_NAME>() << ":\n\t" << output_msg.second << std::endl;
#else
	std::string deviceBuildLog;

	program.getInfo(CL_PROGRAM_BUILD_LOG, &deviceBuildLog);
	std::cerr << "Build output:\n" << deviceBuildLog;
#endif
    }


    throw;
}

Matrix::Matrix(cl::Context &context)
    : cmdQueue(context /* , cl::QueueProperties::OutOfOrder */), program(context, readSourceFile(program_file_name), false),
      random_fill_float_block(build_program(program, FloatType::Single), "random_fill_float_block"),
      multiply_float_matrix_block(program, "multiply_float_matrix_block") //,
      // random_fill_double_block(program, "random_fill_double_block")
{
}
