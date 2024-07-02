# This file sets the basic flags for the GAMBIT compiler
# Generate the C files
# -I
set( CMAKE_VASM_COMPILE_OBJECT "<CMAKE_VASM_COMPILER> -devpac -Fhunk -m68020 -I${M68KAMIGA_ROOT_PATH}/m68k-amigaos/ndk-include <SOURCE> -o <OBJECT> && <CMAKE_VASM_COMPILER> -devpac -m68020 -I${M68KAMIGA_ROOT_PATH}/m68k-amigaos/ndk-include <SOURCE> -o <OBJECT>.txt")
#set( CMAKE_VASM_LINK_EXECUTABLE "<CMAKE_VASM_COMPILER> -o <TARGET> -exe <OBJECTS>" )
set( CMAKE_VASM_LINK_EXECUTABLE "" )
set( CMAKE_VASM_INFORMATION_LOADED 1 )
