#
# Copyright (c) 2008-2014 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Set project name
project (Urho3D-Samples)

# Find Urho3D library
find_package (Urho3D REQUIRED)
include_directories (${URHO3D_INCLUDE_DIRS})

# Include common to all samples
set (COMMON_SAMPLE_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ExistenceApps.h" "${CMAKE_CURRENT_SOURCE_DIR}/ExistenceApps.inl")

# Define dependency libs
set (INCLUDE_DIRS_ONLY ${CMAKE_CURRENT_SOURCE_DIR})

# All files
file (GLOB EXISTENCE_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/*.cpp")
file (GLOB EXISTENCE_H_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/*.h")

# Specific files
set (EXISTENCEEDITOR_CPP_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/GameObject.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/InteractObject.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/Character.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/Player.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/DroneEntity.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/Entity.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/AIController.cpp
)

set (EXISTENCEEDITOR_H_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/GameObject.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/InteractObject.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/Character.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/DroneEntity.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/Entity.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ExistenceComponents/AIController.h
)

# Add samples
if(URHO3D_EXISTENCECLIENT)
    add_subdirectory (ExistenceClient)
endif ()

if(URHO3D_EXISTENCEEDITOR)
    add_subdirectory (ExistenceEditor)
endif ()
