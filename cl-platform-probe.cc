#include <chrono>
#include <iterator>
#include <iostream>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#include "cl-platform-info.hh"
#include "cl-double-pendulum.hh"
#include "cl-platform-probe.hh"

using std::chrono::milliseconds;
using std::cout;
using std::clog;
using std::endl;
using std::size;

using cl::Platform;
using cl::Device;
using cl::NDRange;

extern void probe_cl_platform(Platform &platform)
{
    cout << trim_name(platform.getInfo<CL_PLATFORM_NAME>()) << endl;
}

extern bool probe_cl_device(Device &device)
{
    cout << "\tDevice:                 " << trim_name(device.getInfo<CL_DEVICE_NAME>()) << endl;

    if
	(
	    device.getInfo<CL_DEVICE_AVAILABLE>()
		&&
	    device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>()
		&&
	    [&device] () -> bool
	    {
		cl_bool has_linker;
		device.getInfo(CL_DEVICE_LINKER_AVAILABLE, &has_linker);

		return has_linker;
	    }()
	)
    {
	DoublePendulumSimulation &sim = DoublePendulumSimulation::get(device);

	sim.probeIterationCount(milliseconds(100));
	clog << "\tCandidate step count:   " << sim.iterationCount() << endl;

	// clog << endl; return true;

	auto size_multiple = sim.groupSizeMultiple();

	unsigned long counts[512], times[512][6];
	unsigned long prev_sim_time = sim.runSimulation(NDRange(1 * size_multiple), NDRange(size_multiple));

	for (unsigned n = 1; n <= size(counts); n++)
	{
	    cout << "Multiple: " << n << '\r';

	    counts[n - 1] = static_cast<unsigned long>(n * size_multiple);

	    for (unsigned it = 0; it < size(times[n - 1]); it++)
	    {
		unsigned long sim_time = sim.runSimulation(NDRange(n * size_multiple), NDRange(size_multiple));

		prev_sim_time = sim_time;
		times[n - 1][it] = static_cast<unsigned long>(sim_time);
	    }
	}

	cout << '\n';

	// cout << endl;

	cout << "Counts: [ 0";
	for (auto val: counts)
	    cout << ", " << val;
	cout << " ]" << endl;

	for (unsigned it = 0; it < sizeof times[0] / sizeof times[0][0]; it++)
	{
	    cout << "Times: [ 0";
	    for (auto val: times)
		cout << ", " << val[it];
	    cout << " ]" << endl;
	}
    }
    else
    {
	clog << "\t                        " << "Linker, compiler or device are not available!\n" << endl;
	return false;
    }

    return true;
}

// void list_context_devices(Context &context)
// {
//     vector<Device> devices(context.getInfo<CL_CONTEXT_DEVICES>());
//
//     for (auto &device: devices)
// 	show_cl_device(device);
// }
//
// extern void test_matrix_multiply()
// {
//     cl_platform_id platform;
//     std::array<cl_context_properties, 3> context_props = { 0, 0, 0 };
//
//     if (!args.select_platform_name.empty())
//     {
//         std::vector<cl::Platform> clPlatforms;
//         bool platform_found = false;
//
//         cl::Platform::get(&clPlatforms);
//
//         for (auto it = clPlatforms.cbegin(); !platform_found && it != clPlatforms.cend(); it++)
//             if (!strncmp(it->getInfo<CL_PLATFORM_NAME>().data(), args.select_platform_name.data(),
//         		std::min(it->getInfo<CL_PLATFORM_NAME>().size(), args.select_platform_name.length())))
//             {
//         	platform = it->Wrapper<cl_platform_id>::get();
//         	platform_found = true;
//             }
//
//         if (!platform_found)
//             for (auto it = clPlatforms.cbegin(); !platform_found && it != clPlatforms.cend(); it++)
//         	if(!strncmp(it->getInfo<CL_PLATFORM_VENDOR>().data(), args.select_platform_name.data(),
//         		std::min(it->getInfo<CL_PLATFORM_VENDOR>().size(), args.select_platform_name.length())))
//         	{
//         	    platform = it->Wrapper<cl_platform_id>::get();
//         	    platform_found = true;
//         	}
//
//         if (!platform_found)
//             throw std::runtime_error("No such platform: " + args.select_platform_name);
//
//         context_props[0] = CL_CONTEXT_PLATFORM;
//         context_props[1] = static_cast<cl_context_properties>(reinterpret_cast<intptr_t>(platform));
//         context_props[2] = static_cast<cl_context_properties>(0);
//     }
//
//     cl::Context context(CL_DEVICE_TYPE_DEFAULT, context_props[0] ? context_props.data() : nullptr, context_error_notification);
//
//     if (args.list_all_devices)
//         list_context_devices(context);
//
//     if (args.probe)
//     {
//         cout << "Probing device" << (context.getInfo<CL_CONTEXT_DEVICES>().size() > 1 ? "s" : "") << ": \n";
//
//         for (auto const &device: context.getInfo<CL_CONTEXT_DEVICES>())
//             cout << '\t' << device.getInfo<CL_DEVICE_NAME>() << endl;
//
//         cout << endl;
//
//         cl_ulong const lines = 1024, cols = 1024, internal_size = 1024;
//
//         cl::Buffer
//             m(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * lines * internal_size),
//             n(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY /* CL_MEM_HOST_NO_ACCESS */, sizeof(cl_double) * internal_size * cols),
//             result(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_double)* lines * cols);
//
//         Matrix mat(context);
//         std::vector<cl_float> sub_buffer;
//
//         mat.random_fill<cl_float>(m, lines, internal_size, -100.0f, 100.0f);
//         mat.random_fill<cl_float>(n, internal_size, cols, -100.0f, 100.0f);
//         //mat.zero_fill<cl_float>(result, lines, cols);
//         mat.random_fill<cl_float>(result, internal_size, cols, 0.0f, 0.0001f);
//         mat.waitForCompletion();
//
//
//         std::time_t startTime, endTime;
//         time(&startTime);
//         mat.multiply<cl_float>(m, lines, internal_size, n, internal_size, cols, result, lines, cols);
//         mat.waitForCompletion();
//         time(&endTime);
//
//         cout << std::setprecision(5) << std::fixed << (static_cast<double>(lines) * internal_size * cols * 2 / 1000/1000/1000) / static_cast<double>(endTime - startTime) << " GFLOPS" << endl;
//
//
//         // mat.readBufferRectAsync(result, lines, cols, 852, 718, 20, 20, sub_buffer);
//         // mat.waitForCompletion();
//
//         mat.readBufferRect(result, lines, cols, 852, 718, 20, 20, sub_buffer);
//
//         cout << "Sample sub-matrix:\n";
//         for (unsigned i = 0; i < 10; i++)
//         {
//             for (unsigned j = 0; j < 10; j++)
//         	cout << std::setw(14) << std::setprecision(5) << std::fixed << std::setfill(' ') << std::setiosflags(cout.right) << std::showpos << sub_buffer[i * 10 + j] << ' ';
//
//             cout << endl;
//         }
//     }
// }
