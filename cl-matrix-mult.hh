#include <cstddef>
#include <functional>
#include <vector>
#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

class Matrix
{
    protected:
	cl::CommandQueue cmdQueue;
	cl::Program program;

#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
	cl::KernelFunctor<cl::Buffer, cl_ulong, cl_ulong, cl_float, cl_float> random_fill_float_block;
	// cl::KernelFunctor<cl::Buffer, cl_ulong, cl_ulong, cl_double, cl_double> random_fill_double_block;
#else
	cl::make_kernel<cl::Buffer, cl_ulong, cl_ulong, cl_float, cl_float> random_fill_float_block;
	// cl::make_kernel<cl::Buffer, cl_ulong, cl_ulong, cl_double, cl_double> random_fill_double_block;
#endif
	std::vector<cl::Event> waitEvents, mulEvents;

    public:
	Matrix(cl::Context &context);
	~Matrix() = default;

	template<typename FloatType>
	    void random_fill(cl::Buffer &outputBuffer, cl_ulong M, cl_ulong N, FloatType min_value, FloatType max_value);

	template<typename FloatType>
	    void multiply(cl::Context &clContext, cl::Buffer const &m, cl::Buffer const &n, cl_ulong M, cl_ulong N, cl_ulong P, cl::Buffer &nresult);

	template<typename FloatType, cl_ulong M, cl_ulong N>
	    void show_sub_buffer(cl::Buffer const &mat, std::size_t show_m, std::size_t show_n, std::ostream &os = std::cout);
};

template<>
    inline void Matrix::random_fill<cl_float>(cl::Buffer &outputBuffer, cl_ulong M, cl_ulong N, cl_float min_value, cl_float max_value)
{
    waitEvents.push_back(random_fill_float_block(cl::EnqueueArgs(cmdQueue, cl::NDRange(M, N), cl::NDRange(20, 20)), outputBuffer, M, N, min_value, max_value));
};

/*
template<>
    inline void Matrix::random_fill<cl_double>(cl::Buffer &outputBuffer, cl_ulong M, cl_ulong N, cl_double min_value, cl_double max_value)
{
    waitEvents.push_back(random_fill_double_block(cl::EnqueueArgs(cmdQueue, cl::NDRange(M, N)), outputBuffer, M, N, min_value, max_value));
};
*/
