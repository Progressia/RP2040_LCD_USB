# Find all source files in the current directory
# and save the names to the DIR variable
aux_source_directory(. DIR_SRC)

include_directories(../lib/Config)
include_directories(../lib/GUI)
include_directories(../lib/LCD)
include_directories(../lib/QMI8658)
# Generate link libraries
add_library(src ${DIR_SRC})
target_link_libraries(src PUBLIC Config LCD QMI8658 hardware_clocks hardware_rtc hardware_adc)