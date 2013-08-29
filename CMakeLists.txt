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

message("${Boost_LIBRARIES}")

FUNCTION(sample_build)
# Find all the input files
  SET(sample_dir "${CMAKE_CURRENT_SOURCE_DIR}/samples")

  FILE(GLOB sample_src RELATIVE "${sample_dir}"
      "${sample_dir}/*.cpp")
  MESSAGE(STATUS "Build samples in ${sample_dir}")

  FOREACH(sample_rel_filename ${sample_src})
    SET(sample_filename "${sample_dir}/${sample_rel_filename}")
    SET(sample_name "${sample_rel_filename}")
    STRING(REGEX REPLACE ".cpp" "" sample_name "${sample_name}")
  
    MESSAGE(STATUS "Build sample: ${sample_name} ${sample_filename}")
    ADD_EXECUTABLE(${sample_name} ${sample_filename})
    TARGET_LINK_LIBRARIES(${sample_name} ${Boost_LIBRARIES} rt)
  ENDFOREACH()
ENDFUNCTION()

sample_build()

INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_BINARY_DIR} ./include ../TbdLib/include  )
ADD_DEFINITIONS(-g -O3 -fsigned-char -Wall -fPIC -std=c++11)
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
