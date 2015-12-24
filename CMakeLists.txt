cmake_minimum_required( VERSION 2.8 )
project( cmnt )
set( CMAKE_CXX_FLAGS "--std=gnu++11 -Wall -Werror" )

set( LIBSRC_DIR src/cpp )
set( LIBTEST_DIR src/cpp/test )
include_directories( include )

add_library( cmnt_core ${LIBSRC_DIR}/lister.cpp )
target_link_libraries( cmnt_core m )

add_executable( list_test ${LIBTEST_DIR}/list_test.cpp )
target_link_libraries( list_test cmnt_core )
