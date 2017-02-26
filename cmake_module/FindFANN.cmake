# - Find FANN
# Fast Artificial Neural Network Library
# http://leenissen.dk/fann/wp/
#
# The module defines the following variables:
#  FANN_FOUND - the system has fann
#  FANN_INCLUDE_DIR - where to find fann.h
#  FANN_INCLUDE_DIRS - fann includes
#  FANN_LIBRARY - where to find the fann library
#  FANN_LIBRARIES - aditional libraries
#  FANN_ROOT_DIR - root dir (ex. /usr/local)

# set FANN_INCLUDE_DIR
find_path ( FANN_INCLUDE_DIR
  NAMES
    fann.h
  DOC
    "FANN include directory"
)

# set FANN_INCLUDE_DIRS
set ( FANN_INCLUDE_DIRS ${FANN_INCLUDE_DIR} )

# set FANN_LIBRARY
find_library ( FANN_LIBRARY
  NAMES
    fann
  DOC
    "FANN library location"
)

# set FANN_LIBRARIES
set ( FANN_LIBRARIES ${FANN_LIBRARY} )
if (UNIX)
  list (APPEND FANN_LIBRARIES m)
endif ()

# root dir
# try to guess root dir from include dir
if ( FANN_INCLUDE_DIR )
  string ( REGEX REPLACE "(.*)/include.*" "\\1" FANN_ROOT_DIR ${FANN_INCLUDE_DIR} )

# try to guess root dir from library dir
elseif ( FANN_LIBRARY )
  string ( REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" FANN_ROOT_DIR ${FANN_LIBRARY} )
endif ()

# handle REQUIRED and QUIET options
include ( FindPackageHandleStandardArgs )

find_package_handle_standard_args ( FANN DEFAULT_MSG FANN_LIBRARY
  FANN_INCLUDE_DIR
  FANN_INCLUDE_DIRS
  FANN_LIBRARIES
  FANN_ROOT_DIR
)


mark_as_advanced (
  FANN_LIBRARY
  FANN_LIBRARIES
  FANN_INCLUDE_DIR
  FANN_INCLUDE_DIRS
  FANN_ROOT_DIR
  FANN_INTERFACE_VERSION
)
