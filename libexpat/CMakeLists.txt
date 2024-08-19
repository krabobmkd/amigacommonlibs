# completely redone by krb for Amiga port.
# still the zconf.h is processed.
cmake_minimum_required (VERSION 3.1)

set(C_STANDARD 11)
project(expat LANGUAGES C VERSION 1.17.6)

set(EXPAT_HDRS
    ascii.h     expat_external.h  internal.h   utf8tab.h  xmltok_impl.h
    asciitab.h  iasciitab.h       latin1tab.h  xmlrole.h
    expat.h     intconfig.h       nametab.h    xmltok.h
)
set(EXPAT_SRCS
    xmlparse.c  xmlrole.c  xmltok.c
	#  xmltok_ns.c  xmltok_impl.c
)

message(WARNING "expat compiled with options:${AMIGA_COMPILER_OPTIONS}")
add_library(expat STATIC ${EXPAT_SRCS} ${EXPAT_HDRS} )
set_property(TARGET expat PROPERTY C_STANDARD 11)
target_compile_definitions(expat PRIVATE PTRCALL= )
if(AMIGA)
target_compile_options(expat PRIVATE -O2 ${AMIGA_COMPILER_OPTIONS} )
endif()
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

