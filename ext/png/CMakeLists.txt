project(extpng)

file(GLOB PNG_FILES "*.c")
file(GLOB ZLIB_FILES "../zlib/*.c")

include_directories(${PROJECT_SOURCE_DIR}/../zlib)
add_library(${PROJECT_NAME} STATIC ${PNG_FILES} ${ZLIB_FILES})

add_definitions(-DPNG_ARM_NEON_IMPLEMENTATION=0)
add_definitions(-DPNG_ARM_NEON_OPT=0)

set(EXTPNG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(EXTPNG_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
