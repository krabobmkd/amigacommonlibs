

# This file is a 'toolchain description file' for CMake.
# It teaches CMake about the m68k-amigaos-gcc compiler, so that CMake can generate makefiles
# from CMakeLists.txt that invoke it.

# ADAPTED BY VF 2022-08-03

# To use this toolchain file with CMake, invoke CMake with the following command line parameters
# cmake -DCMAKE_TOOLCHAIN_FILE=<Root>/Emscripten.cmake
#       -DCMAKE_BUILD_TYPE=<Debug|RelWithDebInfo|Release|MinSizeRel>
#       -G "Unix Makefiles" (Linux and OSX)
#       <path/to/CMakeLists.txt> # Note, pass in here ONLY the path to the file, not the filename 'CMakeLists.txt' itself.

# After that, build the generated Makefile with the command 'make' . On Windows, you may download and use 'mingw32-make' instead.
#  VF: (or use cmake --build . ).

# The following variable describes the target OS we are building to.
# m68k-amigaos, but may manage more than just "m68k" arch

if(AMIGA)
  return()
endif()

set(CMAKE_SYSTEM_NAME m68k-amigaos)
#set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# to test if(AMIGA) in CMakeLists.txt:
set(AMIGA ON)

set(CMAKE_C_COMPILER_FORCED ON)
set(CMAKE_CXX_COMPILER_FORCED ON)
set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)

set(CMAKE_CROSSCOMPILING TRUE)

# Advertise Emscripten as a 32-bit platform (as opposed to CMAKE_SYSTEM_PROCESSOR=x86_64 for 64-bit platform),
# since some projects (e.g. OpenCV) use this to detect bitness.
set(CMAKE_SYSTEM_PROCESSOR m68k)


# Tell CMake how it should instruct the compiler to generate multiple versions of an outputted .so library: e.g. "libfoo.so, libfoo.so.1, libfoo.so.1.4" etc.
# This feature is activated if a shared library project has the property SOVERSION defined.
set(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-Wl,-soname,")
set(CMAKE_POSITION_INDEPENDENT_CODE OFF CACHE BOOL "no fpic")
# In CMake, CMAKE_HOST_WIN32 is set when we are cross-compiling from Win32 to Emscripten: http://www.cmake.org/cmake/help/v2.8.12/cmake.html#variable:CMAKE_HOST_WIN32
# The variable WIN32 is set only when the target arch that will run the code will be WIN32, so unset WIN32 when cross-compiling.
set(WIN32)

# The same logic as above applies for APPLE and CMAKE_HOST_APPLE, so unset APPLE.
set(APPLE)

# And for UNIX and CMAKE_HOST_UNIX. However, Emscripten is often able to mimic being a Linux/Unix system, in which case a lot of existing CMakeLists.txt files can be configured for Emscripten while assuming UNIX build, so this is left enabled.
set(UNIX 1)

# Do a no-op access on the CMAKE_TOOLCHAIN_FILE variable so that CMake will not issue a warning on it being unused.
#if (CMAKE_TOOLCHAIN_FILE)
#endif()

# In order for check_function_exists() detection to work, we must signal it to pass an additional flag, which causes the compilation
# to abort if linking results in any undefined symbols. The CMake detection mechanism depends on the undefined symbol error to be raised.
set(CMAKE_REQUIRED_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=1")

if ("${M68KAMIGA_ROOT_PATH}" STREQUAL "" AND "${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(M68KAMIGA_ROOT_PATH "C:/cygwin64/opt/amiga")
endif()

# Locate where the Emscripten compiler resides in relative to this toolchain file.
if ("${M68KAMIGA_ROOT_PATH}" STREQUAL "")
        set(M68KAMIGA_ROOT_PATH "/opt/amiga")
endif()

# If not found by above search, locate using the EMSCRIPTEN environment variable.
#if ("${M68KAMIGA_ROOT_PATH}" STREQUAL "")
#        set(M68KAMIGA_ROOT_PATH "$ENV{M68KAMIGA_ROOT}")
#endif()

# Abort if not found. 
if ("${M68KAMIGA_ROOT_PATH}" STREQUAL "")
        message(FATAL_ERROR "Could not locate the m68k-amigaos compiler toolchain directory! Either set the M68KAMIGA_ROOT environment variable, or pass -DM68KAMIGA_ROOT_PATH=xxx to CMake to explicitly specify the location of the compiler!")
endif()


# Normalize, convert Windows backslashes to forward slashes or CMake will crash.
get_filename_component(M68KAMIGA_ROOT_PATH "${M68KAMIGA_ROOT_PATH}" ABSOLUTE)

message(STATUS "m68k-amiga root: ${M68KAMIGA_ROOT_PATH}")


#if (NOT CMAKE_MODULE_PATH)
#	set(CMAKE_MODULE_PATH "")
#endif()
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${M68KAMIGA_ROOT_PATH}/cmake/Modules")
message(STATUS "  ********** CMAKE_TOOLCHAIN_FILE:${CMAKE_TOOLCHAIN_FILE}:")
get_filename_component(TOOLCHAIN_PATH "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
get_filename_component(TOOLCHAIN_PATH "${TOOLCHAIN_PATH}" DIRECTORY)

set(CMAKE_MODULE_PATH "${TOOLCHAIN_PATH}")

message(STATUS "  ********** CMAKE_MODULE_PATH:${CMAKE_MODULE_PATH}")
# VF: TODO
set(CMAKE_FIND_ROOT_PATH "${M68KAMIGA_ROOT_PATH}/m68k-amigaos")


message(STATUS "CMAKE_HOST_SYSTEM:${CMAKE_HOST_SYSTEM}")
message(STATUS "CYGWIN:${CYGWIN}")
if(CMAKE_HOST_WIN32 OR CMAKE_HOST_SYSTEM MATCHES "CYGWIN*")
    set(EXE_SUFFIX ".exe")
else()
    set(EXE_SUFFIX "")
endif()

# Specify the compilers to use for C and C++

set(M68KAMIGAGCC_BIN "${M68KAMIGA_ROOT_PATH}/bin")

set(CMAKE_AR                    "${M68KAMIGAGCC_BIN}/m68k-amigaos-ar${EXE_SUFFIX}")
set(CMAKE_ASM_COMPILER          "${M68KAMIGAGCC_BIN}/m68k-amigaos-as${EXE_SUFFIX}")
set(CMAKE_C_COMPILER            "${M68KAMIGAGCC_BIN}/m68k-amigaos-gcc${EXE_SUFFIX}")
set(CMAKE_CXX_COMPILER          "${M68KAMIGAGCC_BIN}/m68k-amigaos-g++${EXE_SUFFIX}")
set(CMAKE_LINKER                "${M68KAMIGAGCC_BIN}/m68k-amigaos-ld${EXE_SUFFIX}")
set(CMAKE_OBJCOPY               "${M68KAMIGAGCC_BIN}/m68k-amigaos-objcopy${EXE_SUFFIX}")
set(CMAKE_RANLIB                "${M68KAMIGAGCC_BIN}/m68k-amigaos-ranlib${EXE_SUFFIX}")
set(CMAKE_SIZE                  "${M68KAMIGAGCC_BIN}/m68k-amigaos-size${EXE_SUFFIX}")
set(CMAKE_STRIP                 "${M68KAMIGAGCC_BIN}/m68k-amigaos-strip${EXE_SUFFIX}")


# - - - - --  -
# VF: commented because deprecated CMAKE_ variables.
# Don't do compiler autodetection, since we are cross-compiling.
#include(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" Clang)
#CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" Clang)

# To find programs to execute during CMake run time with find_program(), e.g. 'git' or so, we allow looking
# into system paths.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Since Emscripten is a cross-compiler, we should never look at the system-provided directories like /usr/include and so on.
# Therefore only CMAKE_FIND_ROOT_PATH should be used as a find directory. See http://www.cmake.org/cmake/help/v3.0/variable/CMAKE_FIND_ROOT_PATH_MODE_INCLUDE.html
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


LIST(APPEND CMAKE_SYSTEM_INCLUDE_PATH
    "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/ndk-include"
	# allow best parsing of C++ types in qtcreator and ides, but else no need.
	"${M68KAMIGA_ROOT_PATH}/lib/gcc/m68k-amigaos/6.5.0b/include/c++"
    "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/include"
    )
LIST(APPEND CMAKE_SYSTEM_LIBRARY_PATH
    "${M68KAMIGA_ROOT_PATH}/lib"
    )
#if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    # just so qtcreator windows parse that
    include_directories(
    "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/ndk-include"
    "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/include")
#endif()

# We would prefer to specify a standard set of Clang+Emscripten-friendly common convention for suffix files, especially for CMake executable files,
# but if these are adjusted, ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake will fail, since it depends on being able to compile output files with predefined names.
#SET(CMAKE_LINK_LIBRARY_SUFFIX "")
#SET(CMAKE_STATIC_LIBRARY_PREFIX "")
#SET(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
#SET(CMAKE_SHARED_LIBRARY_PREFIX "")
#SET(CMAKE_SHARED_LIBRARY_SUFFIX ".library")
SET(CMAKE_EXECUTABLE_PREFIX "m68k-")
SET(CMAKE_EXECUTABLE_SUFFIX "")
#SET(CMAKE_FIND_LIBRARY_PREFIXES "")
#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

 set(CMAKE_SHARED_LIBRARY_LINK_FLAGS "")

SET(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)

set(CMAKE_C_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG "@")

# Specify the program to use when building static libraries. Force Emscripten-related command line options to clang.
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")


# Hardwire support for cmake-2.8/Modules/CMakeBackwardsCompatibilityC.cmake without having CMake to try complex things
# to autodetect these:
set(CMAKE_SKIP_COMPATIBILITY_TESTS 1)
set(CMAKE_SIZEOF_CHAR 1)
set(CMAKE_SIZEOF_UNSIGNED_SHORT 2)
set(CMAKE_SIZEOF_SHORT 2)
set(CMAKE_SIZEOF_INT 4)
set(CMAKE_SIZEOF_UNSIGNED_LONG 4)
set(CMAKE_SIZEOF_UNSIGNED_INT 4)
set(CMAKE_SIZEOF_LONG 4)
set(CMAKE_SIZEOF_VOID_P 4)
set(CMAKE_SIZEOF_FLOAT 4)
set(CMAKE_SIZEOF_DOUBLE 8)
set(CMAKE_C_SIZEOF_DATA_PTR 4)
set(CMAKE_CXX_SIZEOF_DATA_PTR 4)
set(CMAKE_HAVE_LIMITS_H 1)
set(CMAKE_HAVE_UNISTD_H 1)
set(CMAKE_HAVE_PTHREAD_H 1)
set(CMAKE_HAVE_SYS_PRCTL_H 1)
set(CMAKE_WORDS_BIGENDIAN 1)
set(CMAKE_DL_LIBS)

# note: more option should go in cmakelists
#noixemul is quit mandatory for bebbo gcc6.5

# note amigaOS includes all use "unsigned char" and not char for default character chains
set(M68K_MANDATORY_OPT " -DAMIGA -D__AMIGA__ -v -noixemul -funsigned-char -ansi ")

set(M68K_MANDATORY_OPT " -DAMIGA -D__AMIGA__ -v -noixemul -funsigned-char -ansi ")

# favorite 68060:
# -fpermissive -O2 -m68060 -fsingle-precision-constant -mhard-float -ffast-math -fno-rtti -save-temps

set(CMAKE_C_FLAGS ${M68K_MANDATORY_OPT} )
set(CMAKE_CXX_FLAGS ${M68K_MANDATORY_OPT})
# more reliable ? (test for zlib that does dynamic):

#horrible result:
# add_compile_options( -noixemul -v -funsigned-char -ansi )
# add_compile_definitions( AMIGA=1 __AMIGA__=1  )

set(CMAKE_C_FLAGS_RELEASE " " )
set(CMAKE_C_FLAGS_MINSIZEREL " " )
set(CMAKE_C_FLAGS_RELWITHDEBINFO " -g " )
set(CMAKE_C_FLAGS_DEBUG " -g " )

set(CMAKE_CXX_FLAGS_RELEASE "  " )
set(CMAKE_CXX_FLAGS_MINSIZEREL "  " )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " -g " )
set(CMAKE_CXX_FLAGS_DEBUG " -g " )
set(CMAKE_COMPILE_M68K_MANDATORY_LINKEROPT " -v")

set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_COMPILE_M68K_MANDATORY_LINKEROPT} " CACHE STRING "Amiga-overridden CMAKE_EXE_LINKER_FLAGS")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE " " CACHE STRING "Amiga-overridden CMAKE_EXE_LINKER_FLAGS_RELEASE")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-Os" CACHE STRING "Amiga-overridden CMAKE_EXE_LINKER_FLAGS_MINSIZEREL")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "- -g" CACHE STRING "Amiga-overridden CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE " " CACHE STRING "Amiga-overridden CMAKE_SHARED_LINKER_FLAGS_RELEASE")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "-Os" CACHE STRING "Amiga-overridden CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO " -g" CACHE STRING "Amiga-overridden CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE " " CACHE STRING "Amiga-overridden CMAKE_MODULE_LINKER_FLAGS_RELEASE")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "-Os" CACHE STRING "Amiga-overridden CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO " -g" CACHE STRING "Amiga-overridden CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO")


#
set(CMAKE_POSITION_INDEPENDENT_CODE OFF CACHE BOOL "Description")
set(POSITION_INDEPENDENT_CODE OFF CACHE BOOL "Description")


# Experimental support for targeting generation of Visual Studio project files (vs-tool) of Emscripten projects for Windows.
# To use this, pass the combination -G "Visual Studio 10" -DCMAKE_TOOLCHAIN_FILE=Emscripten.cmake
#if ("${CMAKE_GENERATOR}" MATCHES "^Visual Studio.*")
#	# By default, CMake generates VS project files with a <GenerateManifest>true</GenerateManifest> directive.
#	# This causes VS to attempt to invoke rc.exe during the build, which will fail since app manifests are meaningless for Emscripten.
#	# To disable this, add the following linker flag. This flag will not go to emcc, since the Visual Studio CMake generator will swallow it.
#	set(EMSCRIPTEN_VS_LINKER_FLAGS "/MANIFEST:NO")
#	# CMake is hardcoded to write a ClCompile directive <ObjectFileName>$(IntDir)</ObjectFileName> in all VS project files it generates.
#	# This makes VS pass emcc a -o param that points to a directory instead of a file, which causes emcc autogenerate the output filename.
#	# CMake is hardcoded to assume all object files have the suffix .obj, so adjust the emcc-autogenerated default suffix name to match.
#	set(EMSCRIPTEN_VS_LINKER_FLAGS "${EMSCRIPTEN_VS_LINKER_FLAGS} --default-obj-ext .obj")
#	# Also hint CMake that it should not hardcode <ObjectFileName> generation. Requires a custom CMake build for this to work (ignored on others)
#	# See http://www.cmake.org/Bug/view.php?id=14673 and https://github.com/juj/CMake
#	set(CMAKE_VS_NO_DEFAULT_OBJECTFILENAME 1)

#	# Apply and cache Emscripten Visual Studio IDE-specific linker flags.
#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_VS_LINKER_FLAGS}" CACHE STRING "")
#	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${EMSCRIPTEN_VS_LINKER_FLAGS}" CACHE STRING "")
#	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${EMSCRIPTEN_VS_LINKER_FLAGS}" CACHE STRING "")
#endif()

#if (NOT DEFINED CMAKE_CROSSCOMPILING_EMULATOR)
#  find_program(NODE_JS_EXECUTABLE NAMES nodejs node)
#  if(NODE_JS_EXECUTABLE)
#    set(CMAKE_CROSSCOMPILING_EMULATOR "${NODE_JS_EXECUTABLE}" CACHE FILEPATH "Path to the emulator for the target system.")
#  endif()
#endif()
# No-op on CMAKE_CROSSCOMPILING_EMULATOR so older versions of cmake do not
# complain about unused CMake variable.
if(CMAKE_CROSSCOMPILING_EMULATOR)
endif()


# - - - add special regular 68k assembler
set(CMAKE_VASM_COMPILER          "${M68KAMIGAGCC_BIN}/vasmm68k_mot${EXE_SUFFIX}")
mark_as_advanced( CMAKE_VASM_COMPILER )
set( CMAKE_VASM_SOURCE_FILE_EXTENSIONS s;asm )
# Remember this as a potential error
set( CMAKE_VASM_OUTPUT_EXTENSION .o )
set( CMAKE_VASM_COMPILER_ENV_VAR "" )
# Configure variables set in this file for fast reload later on
configure_file( ${CMAKE_CURRENT_LIST_DIR}/CMakeVASMCompiler.cmake.in
                ${CMAKE_PLATFORM_INFO_DIR}/CMakeVASMCompiler.cmake )
