# Pranav's Archicad JSON exporter plugin

This repository contains a plugin for Archicad that provides facilities for exporting elements in JSON format.

## Prerequisites

- [Visual Studio](https://visualstudio.microsoft.com/downloads/)
- [CMake](https://cmake.org) (3.16 minimum version is needed).
- [Python](https://www.python.org) for resource compilation and build script usage (3.8+).

Additionally for Visual Studio, ensure you have the correct platform toolset installed:
- Archicad 23 - 24: Platform toolset v141
- Archicad 25 - 27: Platform toolset v142

Note: This plugin was built using the [Archicad addon template](https://github.com/GRAPHISOFT/archicad-addon-cmake). While the template can build for both
Windows and Mac OS, the plugin in its current state only builds for Windows at the moment, so please use Windows for building this.

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
- Checkboxes for property definition filters. These limit the property types that will be collected and exported for each element, and multiple
  types can be selected. If all properties checkbox is checked or none of them are, then all property types will be included.
- Text box containing comma-separated values of all available element types. You can limit the element types available by removing unwanted
  element types.
- Text box requesting a file path. Enter a file path (e.g. `C:\Users\Username\Output.json`) to provide a location to output JSON data.

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
