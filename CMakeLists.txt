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

add_library( editor ${LIBSRC_DIR}/comment_editor.cpp )

add_executable( cmnt ${LIBSRC_DIR}/cmnt.cpp )
target_link_libraries( cmnt ${Boost_LIBRARIES} lister comment editor )

# Test programs (not CTests for now)
add_executable( list_test ${LIBTEST_DIR}/list_test.cpp )
target_link_libraries( list_test ${Boost_LIBRARIES} lister comment )

add_executable( add_comment ${LIBTEST_DIR}/add_comment.cpp )
target_link_libraries( add_comment ${Boost_LIBRARIES} comment )
