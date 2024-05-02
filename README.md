# Pranav's Archicad JSON exporter plugin

This repository contains a plugin for Archicad that provides facilities for exporting elements in JSON format.

## Prerequisites

- [Visual Studio](https://visualstudio.microsoft.com/downloads/)
- [CMake](https://cmake.org) (3.16 minimum version is needed).
- [Python](https://www.python.org) for resource compilation and build script usage (3.8+).

Additionally for Visual Studio, ensure you have the correct platform toolset installed:
- Archicad 23 - 24: Platform toolset v141
- Archicad 25 - 27: Platform toolset v142

Note 1: This plugin was built using the [Archicad addon template](https://github.com/GRAPHISOFT/archicad-addon-cmake). While the template can build for both
Windows and Mac OS, the plugin in its current state only builds for Windows at the moment, so please use Windows for building this.

Note 2: This project also makes use of [cpp-httplib](https://github.com/yhirose/cpp-httplib) library for HTTP/HTTPS functions. By default, the project
currently only allows HTTP requests to be sent. To send HTTPS requests, you need to do the following:
- Install OpenSSL (version 3+). I was able to do this with [vcpkg](https://github.com/microsoft/vcpkg) like so:
  ```
  .\vcpkg install openssl[core,tools]
  .\vcpkg integrate install
  ```
- Navigate to [Src/DataExporter.cpp](https://github.com/PranavBahuguna/Archicad-Plugin-Pranav/blob/master/Src/DataExporter.cpp) and uncomment the line `#define CPPHTTPLIB_OPENSSL_SUPPORT`

If these steps for installing OpenSSL don't work for you, then keep the above line commented as it may cause compilation errors due to not finding the required headers.  

## Setup

The plugin can be easily built using the provided build script. The project and the binaries will be created in the `Build` folder.

This repository uses [archicad-addon-cmake-tools](https://github.com/GRAPHISOFT/archicad-addon-cmake-tools) as a submodule, so in order to use this repository you need to be sure that all submodules are cloned properly.

1. Open a command prompt and clone this repository:
```
git clone https://github.com/PranavBahuguna/Archicad-Plugin-Pranav.git --recurse-submodules
```
2. Run the build script from the root of the repository:
```
python Tools/BuildAddOn.py --configFile config.json --acVersion 26 27
```
3. Open the project in Visual Studio and build it. This will produce a compiled .apx file for use in Archicad.

## Detailed instructions

I was able to build this normally with the python build script. However if the provided script doesn't work, you can set up your environment manually.
See [this](https://github.com/GRAPHISOFT/archicad-addon-cmake?tab=readme-ov-file#detailed-instructions) on more details to perform manual setup.

## Adding plugin to Archicad

Follow the given steps to use this plugin in Archicad:
- The plugin only works in demo mode on Archicad. On Windows, you can start in demo by running the command line command `ARCHICAD.exe -DEMO`.
- Navigate to the Add-On Manager under Options.
- Add an Add-On and select the .apx file in the Build directory of this plugin.

The plugin is available via Options -> Export to JSON.

## Plugin operation

There are several options provided in the plugin:
- Checkboxes for using selection vs using all elements. If no elements are selected in Archicad, then it will default to the latter option. If
  a selection is made (e.g. selecting elements with the arrow tool), it gives the option of parsing data for that selection only, or obtaining
  data from all project elements instead.
- Checkboxes for property definition filters. These limit the property types that will be collected and exported for each element, and multiple
  types can be selected. If all properties checkbox is checked, then all property types will be included.
- Text box containing comma-separated values of all available element types. This automatically updates based on whether a selection or all
  elements are selected. You can limit the elements whose data is extracted by removing element types.
- File path export option. Enter a file path (e.g. `C:\Users\Username\Output.json`) to provide a location to write JSON data to. Unchecking this
  option will disable file exports.
- Url path export option. Enter a base url (e.g. `http://httpbin.org`) to provide a location to upload JSON data to. Data will be sent via the
  HTTP POST method, (so full endpoint would be `http://httpbin.org/post`). Unchecking this option will disable url exports.
- Export button to run the export process for file, url or both. On completion, this will produce a dialog notifying the success or failure of
  the export operations for file and url respectively. This button is disabled if no property definition filters are selected or both file and
  url exports are disabled.

The exported JSON will have this format:

```
Layer Name {
  Element Type {
    Element Guid {
      Property Name : value
    }
  }
}
```

## Limitations / known issues

- This is currently unable to collect data for non-standard elements such as MEP element types. It may well be possible to obtain and parse JSON
  data from them, but I have not included this in the scope of this project for now.
- Certain element types such as 'Object' fail to get data collected for them where it exists. Although the Archicad API can obtain the property
  definitions for that type just fine, it produces an error when attempting to obtain values for those definitions. Its not possible to determine
  which properties have this issue short of checking every single one for each type so I opted not to fix this. This might happen with other types,
  but I have not tested every one to determine if this happens to any others.
- File writing / url upload isn't done concurrently, so this blocks the application until complete. I would have preferred to make this asynchronous
  but chose to limit the scope of the project not to include this for now. 
