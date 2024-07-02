# completely redone by krb for Amiga port.
#
cmake_minimum_required (VERSION 3.1)
set(CXX_STANDARD 11)
set(C_STANDARD 11)
project(libjpeg LANGUAGES C CXX VERSION 1.17.6)

if(AMIGA)

	add_definitions(-O2 -m68060 -mhard-float -ffast-math -save-temps )
	add_definitions(-DHAVE_STDLIB_H )

endif()

set(LIBJPEG_SRC
	jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c 
        jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c 
        jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c 
        jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c 
        jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c 
        jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c 
        jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c 
        jquant2.c jutils.c jmemmgr.c
        
		jmemansi.c

	jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h jpegint.h 
        jpeglib.h jversion.h cdjpeg.h cderror.h transupp.h
        jconfig.h
)


add_library(jpeg STATIC  ${LIBJPEG_SRC} )
set_property(TARGET jpeg PROPERTY C_STANDARD 11)

# generate jconfig.h
#if(AMIGA)
#	configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/amiga_jconfig.h
#			${CMAKE_CURRENT_BINARY_DIR}/jconfig.h @ONLY)
#	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
#else()
#	configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/intel64_jconfig.h
#			${CMAKE_CURRENT_BINARY_DIR}/jconfig.h @ONLY)
#	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
#endif()


