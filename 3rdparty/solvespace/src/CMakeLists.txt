# platform utilities

if(WIN32)
    set(util_SOURCES
        platform/w32util.cpp)
else()
    set(util_SOURCES
        platform/unixutil.cpp)
endif()

# libslvs

set(libslvs_SOURCES
    util.cpp
    entity.cpp
    expr.cpp
    constraint.cpp
    constrainteq.cpp
    system.cpp)

set(libslvs_HEADERS
    solvespace.h)

if(STATIC_LIB)
	add_library(slvs STATIC
	   	 ${libslvs_SOURCES}
		 ${libslvs_HEADERS}
    		 ${util_SOURCES}
    		 lib.cpp)
else()
	add_library(slvs SHARED
	   	 ${libslvs_SOURCES}
		 ${libslvs_HEADERS}
    		 ${util_SOURCES}
    		 lib.cpp)
endif()
target_compile_definitions(slvs
    PRIVATE -DLIBRARY)

target_include_directories(slvs
    PUBLIC ${CMAKE_SOURCE_DIR}/include)

set_target_properties(slvs PROPERTIES
    PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/include/slvs.h)
