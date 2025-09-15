# BlainnflareEngine

## Profiler

This project uses Tracy Profiler(https://github.com/wolfpld/tracy).

This is a live profiler, meaning you need to launch a separate app that would listen to your application.

To build Tracy you need to run these two commands from tracy directory (libs/tracy, in our case)
<pre>cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release</pre>
This would generate the build for config for us and
<pre>cmake --build profiler/build --config Release --parallel</pre>
would actually build it.

After compiling it would generate the executable in release folder (libs/tracy/profiler/build/Release). Run tracy and then the target app, in tracy you'd be able to connect to your app and see the profiler.
