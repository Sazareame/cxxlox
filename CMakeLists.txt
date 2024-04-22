# CMakeList.txt : CMake project for cxxlox, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(
	cxxlox
	LANGUAGES CXX
)

aux_source_directory(src SRC)

# Add source to this project's executable.
add_executable (cxxlox  ${SRC})
target_include_directories(cxxlox PUBLIC include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cxxlox PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
