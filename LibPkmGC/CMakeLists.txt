cmake_minimum_required(VERSION 2.8.12)
project(LibPkmGC)

option(LIBPKMGC_DYN_LIB "Build LibPkmGC as a shared library" ON)
message("LIBPKMGC_DYN_LIB:\t${LIBPKMGC_DYN_LIB}")

include(FindBoost)
find_package(Boost 1.55.0 REQUIRED)


file(GLOB_RECURSE source_files ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE header_files ${PROJECT_SOURCE_DIR}/include/*)
set(all_files ${source_files} ${header_files})

set(LIBPKMGC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                 ${Boost_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${LIBPKMGC_INCLUDE_DIRS})



add_definitions(-DLIBPKMGC_SOURCE)
unset(LIBPKMGC_RUNTIME CACHE)
unset(LIBPKMGC_DEFINITONS CACHE)
if(LIBPKMGC_DYN_LIB)
	set(LIBPKMGC_DEFINITIONS -DLIBPKMGC_DYN_LIB CACHE INTERNAL "LibPkmGC's definitions" FORCE)
	add_definitions(-DLIBPKMGC_DYN_LIB)
	add_library(PkmGC SHARED ${all_files})
	set(LIBPKMGC_RUNTIME $<TARGET_FILE:PkmGC> CACHE INTERNAL "Runtime dependencies for projects linking against this library" FORCE)
	install(TARGETS PkmGC
		LIBRARY DESTINATION lib COMPONENT Libraries # .so
		ARCHIVE DESTINATION lib COMPONENT Libraries # .lib, .a
		RUNTIME DESTINATION bin COMPONENT Libraries # .dll
	)
else()
	set(LIBPKMGC_DEFINITIONS -DLIBPKMGC_STATIC_LIB CACHE INTERNAL "PkmGC's definitions" FORCE)
	add_definitions(-DLIBPKMGC_STATIC_LIB)
	add_library(PkmGC STATIC ${all_files})
	install(TARGETS PkmGC
		ARCHIVE DESTINATION lib COMPONENT Libraries # .lib, .a
	)
endif()

install(DIRECTORY "include/LibPkmGC" DESTINATION "include" COMPONENT Headers)
