# Project organization for Visual Studio
# This file handles the organization of the project structure in Visual Studio

include_guard(GLOBAL)

# Function to organize main project files
function(organize_main_project)
    # Group main project files
    source_group("Source Files" FILES main.cpp)
    source_group("common" FILES common/pch.h)
endfunction()

# Function to organize engine project structure
function(organize_engine_project)
    # Organize Visual Studio project structure to match real hierarchy

    source_group("engine\\include\\components" FILES 
        include/components/*.h
    )

    source_group("engine\\include\\runtime" FILES 
        include/runtime/*.h
    )

    source_group("engine\\include\\subsystems" FILES 
        include/subsystems/*.h
    )

    source_group("engine\\include\\tools" FILES 
        include/tools/*.h
    )

    source_group("engine\\src\\runtime" FILES 
        src/runtime/*.cpp
    )

    source_group("engine\\src\\subsystems" FILES 
        src/subsystems/*.cpp
    )

    source_group("engine\\src\\tools" FILES 
        src/tools/*.cpp
    )
endfunction()

# Function to organize editor project structure
function(organize_editor_project)
    # Organize Visual Studio project structure to match real hierarchy
    source_group("editor\\src" FILES 
        src/Editor.cpp
        src/editor_main.cpp
    )

    source_group("editor\\include" FILES 
        include/editor_main.h
        include/Editor.h
    )

    source_group("editor\\ui" FILES 
        ui/editor_main.ui
    )
endfunction()
