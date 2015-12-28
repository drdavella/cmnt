cmake_minimum_required( VERSION 2.8 )
project( cmnt )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++11 -Wall -Werror" )

find_package(Boost COMPONENTS program_options REQUIRED)

set( LIBSRC_DIR src/cpp )
set( LIBTEST_DIR src/cpp/test )
include_directories( include ${Boost_INCLUDE_DIR} )

add_library( lister ${LIBSRC_DIR}/lister.cpp )
target_link_libraries( lister m )

add_library( comment ${LIBSRC_DIR}/comment.cpp )

add_executable( list_test ${LIBTEST_DIR}/list_test.cpp )
target_link_libraries( list_test ${Boost_LIBRARIES} lister )
