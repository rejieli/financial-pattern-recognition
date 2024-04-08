# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#

# Include the directory where ChartDirector header files are located
include_directories(${CMAKE_SOURCE_DIR}/external/ChartDirector/include)

# Add source to this project's executable.
add_executable (CMakeProject1 "CMakeProject1.cpp" "CMakeProject1.h" "io/csvreader.h" "io/csvreader.cpp" "visuals/chart.h" "visuals/chart.cpp" "math/math.cpp" "numerics/stock.cpp" "patterns/patterns.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeProject1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.




# Add the source files you want to compile
# Replace 'src/CMakeProject1.cpp' with the actual source files you want to compile


# Detect the architecture and link the correct library directory
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  # 64-bit system
  link_directories(${CMAKE_SOURCE_DIR}/external/ChartDirector/lib64)
else()
  # 32-bit system
  link_directories(${CMAKE_SOURCE_DIR}/external/ChartDirector/lib32)
endif()

# Link the ChartDirector library to the target
target_link_libraries(CMakeProject1 ${CMAKE_SOURCE_DIR}/external/ChartDirector/lib64/chartdir70.lib)

add_custom_command(TARGET CMakeProject1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/external/ChartDirector/lib64/chartdir70.dll"
    $<TARGET_FILE_DIR:CMakeProject1>)
