# Find all source files in the current directory
# and save the names to the DIR variable
aux_source_directory(. DIR_GUI_SRCS)

include_directories(../Config)

# Generate link libraries
add_library(GUI ${DIR_GUI_SRCS})
target_link_libraries(GUI PUBLIC Config)