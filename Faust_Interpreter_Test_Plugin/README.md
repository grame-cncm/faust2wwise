# Faust Interpreter Test Plugin

This project integrates the Faust IDE as a plugin directly into Wwise Authoring SDK, providing an environment to edit, preview, and build Faust DSP code inside Wwise. 

## Supported Features

- Code editor
- Integrated Faust interpreter
- Build option. This utilizes `faust2wwise` for building dsp code as a Wwise plugin.
- Preview option.
- Source plugin integration support
- Effect plugin integration support with multiple Audio Input generators for testing effects.
- Parameter integration including buttons, checkboxes, sliders, bargraphs. 
- Export options
- Project Filelist (internal / UI / filesystem) that allows to:
    - create new project files,
    - rename project files,
    - delete project files,
- Double Precision support for building and exporting.
- `in place` and `out-of-place` option for building effect plugins.
- Admin rights handling (prompting via Windows elevation)
- Integrated local Faust documentation

## Future-list

- Improve Code Editor:
    - additional shortcuts
    - resizable window
- Integrated output console
- Fix scrolling issues in the plugin window (particularly with many parameters)
- [x] Fix audio duration (numLoops)
    - numLoops is disabled.
- `Windows`-only support
- integrate within `faust2wwise`

## Build Instructions

The template project can be built with the following steps:

1. Enter the `Faust_Interpreter_Test_Plugin` directory
2. Run `wp.py premake Authoring` to generate the Visual Studio solution
3. Open the solution in Visual Studio
4. Add the Faust include directory to the project:
   - Right-click the project → Properties → C/C++ → General → Additional Include Directories
   - Add the path to your Faust installation (e.g., `C:\Program Files\Faust\include`)
5. Build the `Release` configuration


## Usage

Use the code editor to write Faust DSP code.
Pressing Preview checks the compilation status and, if successful, opens a preview instance of the plugin.
The `Build` option compiles the DSP code into a Wwise plugin using `faust2wwise`. After a successful build, **Wwise must be restarted** for the new plugin to appear in the Wwise plugin menu.

For effect plugins, multiple audio input choices are available to provide a test signal for evaluating the effect.

The `Project Files` menu supports creating, renaming, and deleting DSP project files.

The `Export` option stores the compiled DSP code on disk.
Selecting `Save DSP Code` also saves the source `.dsp` file to disk.

The `Double Precision` option enables double-precision internal DSP computation in the exported file.
When building, enabling double precision compiles the plugin using the same precision mode.

Enabling `Out of place` makes the processing use separate input and output buffers in the Execute and compute functions.

Finally, `Docs` redirects to the online Faust documentation.

## Troubleshooting


<details>
<summary> LINK : fatal error LNK1181: cannot open input file 'libfaustwithllvm.lib'</summary>
<br>
Faust is configured without `llvm` backend. Comment out the following line in the [PremakePlugin.lua](PremakePlugin.lua) script.
```
Plugin.sdk.static.links =
{
    "libfaust",
    "libfaustwithllvm",       <---- comment out this line.
}
```
</details>

<br>

<details>
<summary>Scrolling along the plugin window in **Preview** has visual artifacts.</summary>
<br>
This is a known issue. Compiling a DSP implementation that contains a considerable number of parameters (more than can fit on a user's screen) enables scrolling, but it is currently broken and needs to be fixed.
</details>

<br>

<details>
<summary>Can' t loop audio in Wwise.</summary>
<br>
This feature is currently deactivated to eliminate audio artifacts. Check the [Future List](#future-list).
</details>

<br>