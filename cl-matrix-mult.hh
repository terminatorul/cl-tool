#ifndef CL_MATRIX_MULT_HH
#define CL_MATRIX_MULT_HH

#include <cstddef>
#include <functional>
#include <vector>
#include <string>

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
	cl::KernelFunctor<cl::Buffer, cl_ulong, cl_ulong, cl::Buffer, cl_ulong, cl_ulong, cl::Buffer, cl_ulong, cl_ulong> multiply_float_matrix_block;
	// cl::KernelFunctor<cl::Buffer, cl_ulong, cl_ulong, cl_double, cl_double> random_fill_double_block;
#else
	cl::make_kernel<cl::Buffer, cl_ulong, cl_ulong, cl_float, cl_float> random_fill_float_block;
	cl::make_kernel<cl::Buffer, cl_ulong, cl_ulong, cl::Buffer, cl_ulong, cl_ulong, cl::Buffer, cl_ulong, cl_ulong> multiply_float_matrix_block;
	// cl::make_kernel<cl::Buffer, cl_ulong, cl_ulong, cl_double, cl_double> random_fill_double_block;
#endif
	std::vector<cl::Event> waitEvents, mulEvents;

	Matrix(Matrix const &other) = delete;
	Matrix &operator =(Matrix const &other) = delete;

    public:
	Matrix(cl::Context &context);
	~Matrix() = default;

	template<typename FloatType>
	    void random_fill(cl::Buffer &outputBuffer, cl::size_type M, cl::size_type N, FloatType min_value, FloatType max_value);

	template<typename FloatType>
	    void zero_fill(cl::Buffer &outputBuffer, cl::size_type M, cl::size_type N);

	template<typename FloatType>
	    void multiply(cl::Buffer const &m, cl::size_type m_lines, cl::size_type m_cols, cl::Buffer const &n, cl::size_type n_lines, cl::size_type n_cols, cl::Buffer &result, cl::size_type lines, cl::size_type cols);

	template<typename FloatType>
	    void readBufferRectAsync(cl::Buffer const &inputBuff, cl::size_type buffLines, cl::size_type buffCols, cl::size_type startLn, cl::size_type startCol, cl::size_type lines, cl::size_type cols, std::vector<FloatType> &region);

	template<typename FloatType>
	    void readBufferRect(cl::Buffer const &inputBuff, cl::size_type buffLines, cl::size_type buffCols, cl::size_type startLn, cl::size_type startCol, cl::size_type lines, cl::size_type cols, std::vector<FloatType> &region);

	void waitForCompletion();
};

void CL_CALLBACK context_error_notification(char const *error_info, void const *private_info, size_t private_info_size, void *user_data);
std::string readSourceFile(char const *file_name);

template<>
    inline void Matrix::random_fill<cl_float>(cl::Buffer &outputBuffer, cl::size_type M, cl::size_type N, cl_float min_value, cl_float max_value)
{
    waitEvents.push_back(random_fill_float_block(cl::EnqueueArgs(cmdQueue, cl::NDRange(M, N), cl::NDRange(16, 16)), outputBuffer, M, N, min_value, max_value));
};

template<>
    inline void Matrix::zero_fill<cl_float>(cl::Buffer &outputBuffer, cl::size_type M, cl::size_type N)
{
    waitEvents.emplace_back();
    cmdQueue.enqueueFillBuffer<cl_float>(outputBuffer, cl_float(0.0f), 0, M * N * sizeof(cl_float), nullptr, &(*waitEvents.rbegin()));
}

template<typename FloatType>
    void Matrix::readBufferRectAsync(cl::Buffer const &inputBuff, cl::size_type buffLines, cl::size_type buffCols, cl::size_type startLn, cl::size_type startCol, cl::size_type lines, cl::size_type cols, std::vector<FloatType> &region)
{
    cols = std::min(cols, buffCols - startCol);
    lines = std::min(lines, buffLines - startLn);

    region.resize(cols * lines);

    cl::Event event;
    cmdQueue.enqueueReadBufferRect(inputBuff, CL_FALSE, { startCol * sizeof (FloatType), startLn, 0 }, { 0, 0, 0 }, { cols * sizeof(FloatType), lines, 1 },
	    buffCols * sizeof(FloatType), buffCols * buffLines * sizeof(FloatType), cols * sizeof(FloatType), cols * lines * sizeof(FloatType),
	    region.data(), (mulEvents.empty() ? &waitEvents : &mulEvents), &event);
    mulEvents.push_back(event);
}

template<typename FloatType>
    void Matrix::readBufferRect(cl::Buffer const &inputBuff, cl::size_type buffLines, cl::size_type buffCols, cl::size_type startLn, cl::size_type startCol, cl::size_type lines, cl::size_type cols, std::vector<FloatType> &region)
{
    cols = std::min(cols, buffCols - startCol);
    lines = std::min(lines, buffLines - startLn);

    region.resize(cols * lines);

    cmdQueue.enqueueReadBufferRect(inputBuff, CL_TRUE, { startCol * sizeof (FloatType), startLn, 0 }, { 0, 0, 0 }, { cols * sizeof(FloatType), lines, 1 },
	    buffCols * sizeof(FloatType), buffCols * buffLines * sizeof(FloatType), cols * sizeof(FloatType), cols * lines * sizeof(FloatType),
	    region.data(), (mulEvents.empty() ? &waitEvents : &mulEvents));
}

inline void Matrix::waitForCompletion()
{
    if (mulEvents.empty())
    {
	cl::Event::waitForEvents(waitEvents);
	waitEvents.clear();
    }
    else
    {	cl::Event::waitForEvents(mulEvents);
	mulEvents.clear();
    }
}

template<>
    inline void Matrix::multiply<cl_float>(cl::Buffer const &m, cl::size_type m_lines, cl::size_type m_cols, cl::Buffer const &n, cl::size_type n_lines, cl::size_type n_cols, cl::Buffer &result, cl::size_type lines, cl::size_type cols)
{
    mulEvents.push_back(multiply_float_matrix_block(cl::EnqueueArgs(cmdQueue, waitEvents, cl::NDRange(lines, cols), cl::NDRange(16, 16)),  m, m_lines, m_cols, n, n_lines, n_cols, result, lines, cols));
}

#endif // CL_MATRIX_MULT_HH
