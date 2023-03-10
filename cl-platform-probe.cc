#include <cstddef>
#include <chrono>
#include <thread>
#include <iterator>
#include <iostream>
#include <memory>

#if defined(__APPLE__) || defined(__MACOSX__)
# include <OpenCL/cl2.hpp>
#else
# include <CL/cl2.hpp>
#endif

#include "cl-platform-info.hh"
#include "cl-double-pendulum.hh"
#include "cl-platform-probe.hh"

using std::size_t;
using std::chrono::milliseconds;
using std::cout;
using std::clog;
using std::endl;
using std::flush;
using std::size;
using std::unique_ptr;

using cl::Platform;
using cl::Device;
using cl::NDRange;

static const auto
    SIMULATION_STEP_PROBE_TIME = milliseconds(75),
    SIMULATION_PROBE_TIME_MAX  = milliseconds(450);	    // Allow for at least 3 full time steps during probe

extern void probe_cl_platform(Platform &platform)
{
    cout << trim_name(platform.getInfo<CL_PLATFORM_NAME>()) << endl;
}

static size_t probe_global_simulation_size(DoublePendulumSimulation &sim, size_t base_size, size_t size_multiple)
{
    unsigned n = 1u;

    while
	(
	    milliseconds(sim.runSimulation(NDRange((base_size + n)     * size_multiple), NDRange(size_multiple))) < SIMULATION_PROBE_TIME_MAX
		||
	    milliseconds(sim.runSimulation(NDRange((base_size + n + 1) * size_multiple), NDRange(size_multiple))) < SIMULATION_PROBE_TIME_MAX
		||
	    milliseconds(sim.runSimulation(NDRange((base_size + n + 2) * size_multiple), NDRange(size_multiple))) < SIMULATION_PROBE_TIME_MAX
	)
    {
	n *= 2u;
    }

    if (n > 2u)
	return probe_global_simulation_size(sim, base_size + n / 2, size_multiple);

    return base_size;
}

static void show_simulation_times(Device &device, size_t simulation_size, size_t size_multiple, size_t step_size, unsigned long const times[], unsigned int pass_count)
{
#if !defined(DISABLE_LOGGING)

    cout << '\n';

    cout << "counts = [ 0";
    for (size_t n = 1u; n <= simulation_size; n++)
	cout << ", " << n * step_size * size_multiple;
    cout << " ]" << endl;

    for (unsigned pass = 0; pass < pass_count; pass++)
    {
	cout << "times_" << pass << " = [ 0";
	for (unsigned n = 1u; n <= simulation_size; n++)
	    cout << ", " << times[(n - 1u) * pass_count + pass];
	cout << " ]" << endl;
	cout << "figure" << endl;
	cout << "plot(counts, times_" << pass <<", '.')" << endl;
	cout << "title(\"" << trim_name(device.getInfo<CL_DEVICE_VENDOR>()) << " - "
	     << trim_name(Platform(device.getInfo<CL_DEVICE_PLATFORM>()).getInfo<CL_PLATFORM_NAME>()) << "\\n"
	     << trim_name(device.getInfo<CL_DEVICE_NAME>()) << "\\n"
	     << trim_name(device.getInfo<CL_DEVICE_VERSION>()) << "\")" << endl;
	cout << "xlabel('Work group size (work items count)')" << endl;
	cout << "ylabel('Simulation time (ms)')" << endl;
	cout << "grid on" << endl;
	cout << "grid minor on" << endl;
	cout << endl;
    }

#endif
}

extern bool probe_cl_device(Device &device, unsigned int pass_count, unsigned long simulation_count, unsigned delay_ms)
{
    cout << "\tDevice:                " << trim_name(device.getInfo<CL_DEVICE_NAME>()) << endl;

    cl_bool has_linker = false;

    if
	(
	    device.getInfo<CL_DEVICE_AVAILABLE>()
		&&
	    device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>()
		&&
	    (device.getInfo(CL_DEVICE_LINKER_AVAILABLE, &has_linker), has_linker)
	)
    {
	DoublePendulumSimulation &sim = DoublePendulumSimulation::get(device);

	sim.probeIterationCount(SIMULATION_STEP_PROBE_TIME);
	clog << "\tSimulation step count: " << sim.iterationCount() << endl;

	// clog << endl; return true;

	auto
	    size_multiple = sim.groupSizeMultiple(),
	    simulation_size = probe_global_simulation_size(sim, 1u, size_multiple),
	    step_size = simulation_size > simulation_count ? (simulation_size + simulation_count - 1u) / simulation_count : 1u;

	simulation_size = (simulation_size + step_size - 1) / step_size;

	clog << "\tGroup size multiple:   " << size_multiple << " work items" << endl;
	clog << "\tMax workgroup count:   " << step_size * simulation_size << " workgroups" << endl;
	clog << "\tUsing granularity:     " << step_size << " workgroups" << endl;
	clog << "\tReruns:                " << pass_count << " runs" << endl;

	unique_ptr<unsigned long[]> times(new unsigned long[simulation_size * pass_count]);

	for (unsigned pass = 0u; pass < pass_count; pass++)
	{
	    cout << "\rMultiple: " << pass << "/            " << flush;

	    for (size_t n = 1u; n <= simulation_size; n++)
	    {
		cout << "\rMultiple: " << pass + 1u << '/' << n * step_size << flush;
		times[(n - 1) * pass_count + pass] = static_cast<unsigned long>(sim.runSimulation(NDRange(n * step_size * size_multiple), NDRange(size_multiple)));

		if (delay_ms)
		    std::this_thread::sleep_for(milliseconds(delay_ms));
	    }
	}

	show_simulation_times(device, simulation_size, size_multiple, step_size, times.get(), pass_count);
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
