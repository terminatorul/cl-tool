#if !defined(CL_DOUBLE_PENDULUM_HH)
#define CL_DOUBLE_PENDULUM_HH

#include <cstddef>
#include <chrono>
#include <memory>
#include <array>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
    template <typename... Ts>
	using KernelFunction = cl::KernelFunctor<Ts...>;
#else
    template <typename... Ts>
	using KernelFunction = cl::make_kernel<Ts...>;
#endif

extern void CL_CALLBACK context_error_notification(char const *error_info, void const *private_info, size_t private_info_size, void *user_data);

class DoublePendulumSimulation
{
protected:
    using ClockT = std::chrono::high_resolution_clock;

    std::array<cl_context_properties, 3> context_prop;

    cl::Device	       &device;
    cl::Context		context;
    cl::CommandQueue    cmdQueue;
    cl::Program		program;
    cl::Kernel	    	doublePendulumKernel;

    std::unique_ptr<KernelFunction<cl::Buffer, cl_ulong>>
			simulationFn;

    cl::Buffer		result;
    cl_ulong	    	iterCount = 0;

    void build(std::size_t result_buffer_size);
    DoublePendulumSimulation(cl::Device &device);

public:
    void probeIterationCount(std::chrono::milliseconds targetDurationMs);
    unsigned long runSimulation(cl::NDRange const &globalSize, cl::NDRange const &localSize);

    std::size_t groupSizeMultiple() const;
    std::size_t workGroupSize() const;
    cl_ulong iterationCount() const;

    static DoublePendulumSimulation &get(cl::Device &device);
};

inline std::size_t DoublePendulumSimulation::groupSizeMultiple() const
{
    return doublePendulumKernel.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>(device);
}

inline std::size_t DoublePendulumSimulation::workGroupSize() const
{
    return doublePendulumKernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
}

inline cl_ulong DoublePendulumSimulation::iterationCount() const
{
    return iterCount;
}

#endif // !defined(CL_DOUBLE_PENDULUM_HH)
