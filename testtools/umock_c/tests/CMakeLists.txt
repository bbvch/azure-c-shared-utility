#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for micromock unittests folder, it is empty
cmake_minimum_required(VERSION 2.8.11)

add_subdirectory(umockcall_unittests)
add_subdirectory(umockcallrecorder_unittests)
#add_subdirectory(umock_c_integrationtests)
add_subdirectory(umock_c_unittests)
add_subdirectory(umock_c_without_init_unittests)
add_subdirectory(umocktypename_unittests)
add_subdirectory(umocktypes_unittests)
add_subdirectory(umocktypes_without_init_unittests)
add_subdirectory(umocktypes_c_unittests)
add_subdirectory(umocktypes_charptr_unittests)
