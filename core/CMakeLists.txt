# Set the library name
set(LIB_NAME smscore)

# Add all the source files
file(GLOB_RECURSE SRC_FILES "*.c")

# Create the library
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE ${SRC_FILES})


