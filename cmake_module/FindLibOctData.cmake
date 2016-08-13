# - Try to find LibOctData
# Once done this will define
#  LibOctData_FOUND - System has LibOctData
#  LibOctData_INCLUDE_DIRS - The LibOctData include directories
#  LibOctData_LIBRARIES - The libraries needed to use LibOctData
#  LibOctData_DEFINITIONS - Compiler switches required for using LibOctData

find_package(PkgConfig)
# pkg_check_modules(PC_LIBXML QUIET libxml-2.0)
# set(LibE2E_DEFINITIONS ${PC_LIBXML_CFLAGS_OTHER})

find_path(LibOctData_INCLUDE_DIR NAMES octdata PATHS ../
          # HINTS ${PC_LIBXML_INCLUDEDIR} ${PC_LIBXML_INCLUDE_DIRS}
          PATH_SUFFIXES liboctdata
          )


find_library(LibOctData_LIBRARY NAMES liboctdata.so
			PATHS ${LibOctData_INCLUDE_DIR}
			PATH_SUFFIXES release build
             # HINTS ${PC_LIBXML_LIBDIR} ${PC_LIBXML_LIBRARY_DIRS}
             )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LibE2E_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibOctData DEFAULT_MSG
                                  LibOctData_LIBRARY
                                  LibOctData_INCLUDE_DIR)

mark_as_advanced(LibOctData_INCLUDE_DIR LibOctData_LIBRARY )

set(LibOctData_LIBRARIES ${LibOctData_LIBRARY} )
set(LibOctData_INCLUDE_DIRS ${LibOctData_INCLUDE_DIR} )
