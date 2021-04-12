#include <cstddef>
#include <cstdint>
#include <chrono>
#include <memory>
#include <map>
#include <iostream>
#include <iomanip>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#include "cl-matrix-mult.hh"
#include "cl-double-pendulum.hh"

using std::size_t;
using std::intptr_t;
using std::unique_ptr;
using std::map;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::cout;
using std::clog;
using std::cerr;
using std::endl;
using std::setw;

using cl::Error;
using cl::Device;
using cl::Context;
using cl::Kernel;
using cl::Buffer;
using cl::EnqueueArgs;
using cl::Event;
using cl::NDRange;

extern void CL_CALLBACK context_error_notification(char const *error_info, void const *private_info, size_t private_info_size, void *user_data)
{
    cerr << "OpenCL Context error: " << error_info << endl;
}

static char const benchmark_file_name[] = "./cl-double-pendulum.cl";

static cl_command_queue create_command_queue(Context &context, Device &device)
{
    cl_int cmd_queue_error = 0;
    cl_command_queue cmd_queue = ::clCreateCommandQueue(context(), device(), CL_QUEUE_PROFILING_ENABLE, &cmd_queue_error);

    if (cmd_queue == 0)
	throw Error(cmd_queue_error, "Create command queue failed");

    return cmd_queue;
}

DoublePendulumSimulation::DoublePendulumSimulation(Device &device)
    : device(device),
	context_prop
	{
	    CL_CONTEXT_PLATFORM,
	    static_cast<cl_context_properties>(reinterpret_cast<intptr_t>(device.getInfo<CL_DEVICE_PLATFORM>())),
	    0
	},
	context(device, context_prop.data(), context_error_notification),
	cmdQueue(create_command_queue(context, device)),
	program(context, readSourceFile(benchmark_file_name), false)
{
}

void DoublePendulumSimulation::build(size_t result_buffer_size)
{
    try
    {
	program.build();
    }
    catch(Error const &error)
    {
	if (error.err() == CL_BUILD_PROGRAM_FAILURE)
	{
#if defined(CL_HPP_PARAM_NAME_INFO_1_0_)
	    auto const buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();

	    if (!buildLog.empty())
		for (auto const &output_msg: buildLog)
		    cerr << "Build output from device " << output_msg.first.getInfo<CL_DEVICE_NAME>() << ":\n\t" << output_msg.second << std::endl;
#else
	    string deviceBuildLog;

	    program.getInfo(CL_PROGRAM_BUILD_LOG, &deviceBuildLog);
	    cerr << "Build output:\n" << deviceBuildLog;
#endif
	}
	else
	    cerr << "OpenCL error: " << error.what() << endl;

	throw;
    }

    doublePendulumKernel = Kernel(program, "doublePendulumSimulation");
    simulationFn.reset(new KernelFunction<Buffer, cl_ulong>(doublePendulumKernel));
    result = Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, result_buffer_size);
}

static size_t itemCount(NDRange const &range)
{
    cl::size_type const *dimensions = range;

    return dimensions[0] * dimensions[1] * dimensions[2];
}

unsigned long DoublePendulumSimulation::runSimulation(NDRange const &globalSize, NDRange const &localSize)
{
    static unsigned simulation_count = 0;

    Event fn_event = (*simulationFn)(EnqueueArgs(cmdQueue, NDRange(0), globalSize, localSize), result, iterCount);
    cmdQueue.finish();

    unsigned long execTime = static_cast<unsigned long>((fn_event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - fn_event.getProfilingInfo<CL_PROFILING_COMMAND_START>() + 500000UL) / 1000000UL);
    // clog << "Kernel time " << ++simulation_count << " (" << setw(4) << itemCount(globalSize) << '/' << itemCount(localSize) << "): " << execTime << "ms" << endl;

    return execTime;
}

void DoublePendulumSimulation::probeIterationCount(milliseconds targetDuration)
{
    iterCount = 1;
    cl_ulong executionTime;
    size_t groupSizeMultiple = this->groupSizeMultiple();

    auto runSimFn = [this, &groupSizeMultiple]() -> cl_ulong
    {
	Event fn_event = (*simulationFn)(EnqueueArgs(cmdQueue, NDRange(0), NDRange(groupSizeMultiple), NDRange(groupSizeMultiple)), result, iterCount);
	cmdQueue.finish();
	return fn_event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - fn_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    };

    do
    {
	iterCount *= 2;
	executionTime = runSimFn();
    }
    while (nanoseconds(executionTime) < duration_cast<nanoseconds>(targetDuration));

    iterCount = (iterCount * duration_cast<nanoseconds>(targetDuration).count() + executionTime / 2) / executionTime;

    executionTime = duration_cast<nanoseconds>(milliseconds(100)).count() + runSimFn() + runSimFn();

    iterCount = (iterCount * duration_cast<nanoseconds>(targetDuration).count() * 3U + executionTime / 2) / executionTime;
}

static map<cl_platform_id, map<cl_device_id, unique_ptr<DoublePendulumSimulation>>>
    pendulumSimulations;

DoublePendulumSimulation &DoublePendulumSimulation::get(cl::Device &device)
{
    unique_ptr<DoublePendulumSimulation> &ptr = pendulumSimulations[device.getInfo<CL_DEVICE_PLATFORM>()][device()];

    if (!ptr)
    {
	ptr.reset(new DoublePendulumSimulation(device));
	ptr->build(sizeof(cl_char) * 128);
    }

    return *ptr;
}
