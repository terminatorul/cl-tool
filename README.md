# cl-tool
List detailed infromation on OpenCL devices (compute devices and GPUs) from the command line.
Both the OpenCL framework and GPU-specific OpenCL drivers must be installed. Git submodule for Khronos reference OpenCL driver manager will be used for building from source.

Similar to the existing `clinfo` (or maybe `cldemo`) tools.

## Syntax

Command line syntax:

```txt
Syntax:
        cl-tool [ --include-defaults ]
        cl-tool [ [--list] [--probe] --platforms [--devices] ]
        cl-tool [ [--list] [--probe] --platform "Name" [--devices | --device "Name" ]... ]...

cl-tool will by default attempt to probe the default OpenCL device(s) using a trivial matrix
multiplication and report the number of floating-point operations per second in GFLOPS.

Options:
        --probe
             Attempt to run a test function for each available device, using floating-point operations,
             and show the resulting speed (usually in GFLOPS). Without other options, this will be the default
             action. With no device given, --probe implies --devices, meaning all platform devices will be
             probed.

        [--list][ [--probe] --platforms [--devices]
             With --list (default) show details on the available OpenCL platforms.
             With --devices also show details on available OpenCL devices in the platforms.
             With --probe report the resulting floating-point speed of the devices.
             To both show details and test all devices in your system use:
                  cl-tool --list --probe --platforms --devices

        [--list] [--probe] --platform "OpenCL Platform or Vendor Name" [--devices]
             With --list (default) show details on selected OpenCL platform or vendor/other info.
             With --devices show details on available OpenCL devices in selected platform.
             With --probe report the resulting floating-point speed of the devices,
             usually in GFLOPS.

        [--list] [--probe] --platform "OpenCL Platform Name" --device "OpenCL Device Name"
             With --list (default) show selected OpenCL device, from the selected platform.
             With --probe show the resulting floating-point speed of the device.

        [--include-defaults]
             Include device details and extensions that are pre-defined for any OpenCL 1.2 device. By default
             pre-defined values are not shown to simplify the output.

        [--opencl-order]
             Keep platform and device order as reported by OpenCL. By default the order from the command line
             is used, as the OpenCL order is not meant to be significant.

        [--exact-match]
             Match platfom and device name exactly. By default names on the command line can be substrings or
             prefixes of the OpenCL reported names.

The platform and device names can also be substrings or prefixes thereof. You can list and probe several
platforms and devices with multiple options. Option order is significant, specify the platform before the
device(s).

OpenCL allows multiple devices with the same name (or multiple platforms with the same name). In this
case there will be no way to distinguish between the devices using cl-tool.

See the OpenCL specification at https:://www.khronos.org/registry/OpenCL/ for more information
about the platform and device details listed by cl-tool.
```
>>>>>>> bleed
