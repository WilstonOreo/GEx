# cmake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# project id
PROJECT(Tomo)

# force outut into several folders
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
SET (CMAKE_CURRENT_BINARY_DIR
  ${PROJECT_BINARY_DIR}/moc )
SET (CMAKE_PROJECT_DOC_DIR
  ${PROJECT_BINARY_DIR}/doc )


# requirements
FIND_PACKAGE(Qt4 REQUIRED)
set(Boost_INCLUDE_DIR ../../boost_1_54_0)
set(Boost_LIBRARY_DIR ../../boost_1_54_0/stage/lib)


find_package(Boost 1.54 COMPONENTS program_options system filesystem REQUIRED)

IF (Boost_FOUND)
  #message("${Boost_LIBRARY_DIR}")
  include_directories(${Boost_INCLUDE_DIR})
  #  set(CMAKE_LIBRARY_PATH  ${CMAKE_LIBRARY_PATH} ${Boost_LIBRARY_DIR})
  link_directories(${Boost_LIBRARY_DIR})
  ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

ADD_DEFINITIONS("-DDEBUG")
INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_BINARY_DIR} ./include ../TbdLib/include  )

message("${Boost_LIBRARIES}")

# Tests
ADD_EXECUTABLE(Offset ./samples/Offset.cpp)
TARGET_LINK_LIBRARIES(Offset ${Boost_LIBRARIES} rt)

ADD_EXECUTABLE(Hexagon ./samples/Hexagon.cpp)
TARGET_LINK_LIBRARIES(Hexagon ${Boost_LIBRARIES} rt)

ADD_EXECUTABLE(Voronoi ./samples/Voronoi.cpp)
TARGET_LINK_LIBRARIES(Voronoi ${Boost_LIBRARIES} rt)

ADD_EXECUTABLE(Curvature ./samples/Curvature.cpp)
TARGET_LINK_LIBRARIES(Curvature ${Boost_LIBRARIES} rt)

ADD_EXECUTABLE(Chop ./samples/Chop.cpp)
TARGET_LINK_LIBRARIES(Chop ${Boost_LIBRARIES} rt)

ADD_EXECUTABLE(ClosestPoint ./samples/ClosestPoint.cpp)
TARGET_LINK_LIBRARIES(ClosestPoint ${Boost_LIBRARIES} rt)



ADD_DEFINITIONS(-g -Os -fsigned-char -Wall -fPIC -std=c++11)
ADD_DEFINITIONS(-ferror-limit=1 -fcolor-diagnostics -fdiagnostics-show-template-tree)


# add a target to generate API documentation with Doxygen
#FIND_PACKAGE(Doxygen)
#IF(DOXYGEN_FOUND)
#  CONFIGURE_FILE(${CMAKE_PROJECT_DOC_DIR}/tomo.doxygen ${CMAKE_PROJECT_DOC_DIR} @ONLY)
#  ADD_CUSTOM_TARGET(doc
#    ${DOXYGEN_EXECUTABLE} ${CMAKE_PROJECT_DOC_DIR}/tomo.doxygen
#    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#    COMMENT "Generating API documentation with Doxygen (see ./doc/html/index.html)" VERBATIM
#    )
#ENDIF(DOXYGEN_FOUND)
