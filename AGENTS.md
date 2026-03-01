# Repository Guidelines

## Project Structure & Module Organization
- `engine/` contains core runtime systems (`include/` public headers, `src/` implementation).
- `editor/` contains Qt-based editor code (`ui/`, `themes/`, `templates/`, `icons/`, `GameBuild/`).
- `common/` holds shared headers and precompiled-header inputs (for example `common/pch.h`).
- `content/` stores runtime assets (materials, models, scenes, shaders, textures) copied post-build.
- `libs/` includes third-party dependencies (Tracy, Qt helpers, physics, rendering, scripting, etc.).
- Build outputs are generated under `build/<PresetName>/`.

## Build, Test, and Development Commands
- Clone dependencies first: `.\libs\clone_dependencies.bat`
- Build with presets (recommended):
  - `cmake --build --preset Debug_Editor_Tracy_NoD3DDebug`
  - `cmake --build --preset Release_Editor_NoTracy_NoD3DDebug`
- If you need a fresh configure:
  - `cmake --preset Debug_Editor_Tracy_NoD3DDebug`
- Run from the preset build folder (binary name is `BlainnflareEngine` on CMake target).
- Tracy profiler (optional):
  - `cd libs/tracy`
  - `cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build profiler/build --config Release --parallel`

## Coding Style & Naming Conventions
- Language: C++20 (`CMAKE_CXX_STANDARD 20`), MSVC toolchain, Ninja generator.
- Formatting: use `.clang-format` (Microsoft base, Allman braces, 4-space indentation, no tabs).
- Static analysis: `.clang-tidy` enables `bugprone-*`, `concurrency-*`, `cppcoreguidelines-*`, `google-*`, `performance-*`, and `readability-*`.
- Keep includes unsorted unless needed (`SortIncludes: false`).

## Testing Guidelines
- Top-level project currently disables tests (`BUILD_TESTING OFF`), and several bundled deps have tests explicitly disabled.
- For changes, validate by building affected presets and running the executable/editor path you modified.
- If adding tests, prefer CTest-integrated targets and place them near the owning module.

## Commit & Pull Request Guidelines
- Recent history favors short, imperative commit subjects (for example: `fix timeline crash`, `warnings as errors`).
- Keep subject lines concise and scoped to one change.
- PRs should include:
  - what changed and why,
  - affected preset(s) used for verification,
  - screenshots/video for editor UI changes,
  - linked issue/task when available.
