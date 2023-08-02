# Find all source files in the current directory
# and save the names to the DIR variable
aux_source_directory(. DIR_Fonts_SRCS)

# Generate link libraries
add_library(Fonts ${DIR_Fonts_SRCS})