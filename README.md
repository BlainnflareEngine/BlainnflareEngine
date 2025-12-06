# BlainnflareEngine

## Building the engine

<p>To build the engine you need CMake and Qt6 installed.</p>

<p>Open the libs folder and run the clone_dependencies.bat</p>

<p>If you want to build with editor run with flag </p>
<pre>-DBLAINN_INCLUDE_EDITOR=ON</pre>

<p>Open the project in your IDE of choice, and when generating the CMake you need to specify the flag  </p>
<pre>
-DCMAKE_PREFIX_PATH="[your path to Qt]/msvc2022_64/lib/cmake" 
</pre>

<p>You may possibly need to also add</p>
<pre>-DCMAKE_POLICY_VERSION_MINIMUM=3.5</pre>
