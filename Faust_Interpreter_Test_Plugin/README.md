# Faust Interpreter Test Plugin

This project integrates the Faust IDE as a plugin directly into Wwise Authoring SDK, providing an environment to edit, preview, and build Faust DSP code inside Wwise. Currently, it is supported only for Windows.

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

- [ ] Improve Code Editor:
    - [ ] additional shortcuts
    - [ ] resizable window
- [ ] Integrated output console
- [ ] Fix scrolling issues in the plugin window (particularly with many parameters)
- [x] Fix audio duration (numLoops)
    - numLoops is disabled.
- [ ] add support for macOS
- [ ] integrate within `faust2wwise`
- [ ] replace faust interpreter with faust compiler
- [x] automate Faust include and lib directories detection at premake time

## Build Instructions

At first, make sure all dependencies are installed:
- Install **Faust** either [from source](https://github.com/grame-cncm/faust/wiki) or using the [installer](https://github.com/grame-cncm/faust/releases).
- Install **Wwise 2024**
    - **WWISEROOT** must be exposed system-wide. On Windows, this should be done by opening the AudioKinetic Launcher, clicking **Install options** (tool icon) and selecting **Set environment variables**, as shown [here](extras/set_wwise_root_img.png).
    - make sure the correct `Windows SDK` is installed (for instance, for the Wwise 2024.1.12 version, see [Windows Build requirements](https://www.audiokinetic.com/en/public-library/2024.1.12_9034/?source=SDK&id=windows_releasenotes_2024_1_12.html). For other versions see [Windows Release Notes](https://www.audiokinetic.com/en/public-library/2024.1.12_9034/?source=SDK&id=windows_releasenotes.html)).

The template project can be built with the following steps:

```
cd faust2wwise/Faust_Interpreter_Test_Plugin
python path/to/wp.py premake Authoring
python path/to/wp.py build Authoring -x <arch> -t <toolset> -c Release
```

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

<br>

<details>
<summary>Scrolling along the plugin window in <strong>Preview</strong> has visual artifacts.</summary>
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