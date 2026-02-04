# BlainnflareEngine
[Documentation](https://github.com/BlainnflareEngine/BlainnflareEngine/wiki)

## Building the engine

To build the engine you need **CMake** and **Qt 6.9.2 msvc2022_64** installed. Cmake can be downloaded from the [official download page](https://cmake.org/download/). Qt can be downloaded from the official [Qt online installer](https://www.qt.io/development/download) or with [aqt installer](https://github.com/miurahr/aqtinstall).

Open the libs folder and run the clone_dependencies.bat.

If you want to build without the editor run with flag
<pre>-DBLAINN_EXCLUDE_EDITOR=ON</pre>

<p>Open the project in your IDE of choice, and when generating the CMake you need to specify the flag  </p>
<pre>
-DCMAKE_PREFIX_PATH="[your path to Qt]/msvc2022_64/lib/cmake" 
</pre>

<p>You may possibly need to also add</p>
<pre>-DCMAKE_POLICY_VERSION_MINIMUM=3.5</pre>

## Profiler
<p>
This project uses Tracy Profiler(https://github.com/wolfpld/tracy).
</p>

<p>
This is a live profiler, meaning you need to launch a separate app that would listen to your application.
To build Tracy you need to run these two commands from tracy directory (libs/tracy, in our case)
</p>
<pre>cd libs/tracy && cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release</pre>
This would generate the build for config for us and
<pre>cmake --build profiler/build --config Release --parallel</pre>
would actually build it.
<p>
After compiling it would generate the executable in release folder (libs/tracy/profiler/build/Release). Run tracy and then the target app, in tracy you'd be able to connect to your app and see the profiler.
</p>
<p>Running it would look like</p>
<pre>.\profiler\build\Release\tracy-profiler.exe</pre>
<p>from libs/tracy. Or</p>
<pre> .\libs\tracy\profiler\build\Release\tracy-profiler.exe  </pre>
<p> From engine (repo) base directory. </p>
<p style="color:#888888">You can, of cource, run it by opening up this directory in your file explorer.</p>
