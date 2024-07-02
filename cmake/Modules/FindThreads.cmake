# Amiga gcc bebbo has pthread, we only have Amiga case to check here.
# we skip all stupid compilation tests.
# conundrum with this:
set(Threads_FOUND ON)
set(THREADS_FOUND ON)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_USE_WIN32_THREADS_INIT OFF)
set(CMAKE_USE_PTHREADS_INIT ON )
set(CMAKE_HP_PTHREADS_INIT OFF )
# THREADS_HAVE_PTHREAD_ARG
# then back to final original cmake3 final init to have Threads::Threads...

message(STATUS " **** INTERNAL FIND THREAD USED *****")

if(THREADS_FOUND AND NOT TARGET Threads::Threads)
  add_library(Threads::Threads INTERFACE IMPORTED)

#  if(THREADS_HAVE_PTHREAD_ARG)
#    set_property(TARGET Threads::Threads
#                 PROPERTY INTERFACE_COMPILE_OPTIONS "$<$<COMPILE_LANG_AND_ID:CUDA,NVIDIA>:SHELL:-Xcompiler -pthread>"
#                                                    "$<$<NOT:$<COMPILE_LANG_AND_ID:CUDA,NVIDIA>>:-pthread>")
#  endif()

  if(CMAKE_THREAD_LIBS_INIT)
    set_property(TARGET Threads::Threads PROPERTY INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
  endif()
elseif(NOT THREADS_FOUND AND _cmake_find_threads_output)
  file(APPEND
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
    "Determining if compiler accepts -pthread failed with the following output:\n${_cmake_find_threads_output}\n\n")
endif()


#unset(_cmake_find_threads_output)
