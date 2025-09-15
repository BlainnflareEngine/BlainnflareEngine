cmake_minimum_required(VERSION 3.21...4.0.1)

set(PROJECT_NAME
	BlainnflareEngine)

project(${PROJECT_NAME}
		VERSION 0.0.1
		LANGUAGES CXX
)

# common options

# cpp options
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cc)

# =========================================
# debug info
if(${WIN32})
	message("Running on Windows")
elseif(${LINUX})
	message("Running on Linux")
endif()

if(${MSVC})
	message("MSVC version: ${MSVC_VERSION}")
	message("MSVC toolset version: ${MSVC_TOOLSET_VERSION}")
endif()

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")