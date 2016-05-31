cmake_minimum_required(VERSION 2.6)
project(octmarker)
find_package(Qt4 REQUIRED)

FIND_PACKAGE(Boost 1.40 COMPONENTS program_options filesystem system REQUIRED)
# find_package (Threads REQUIRED)
find_package(OpenCV REQUIRED)


# include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # ${Boost_INCLUDE_DIR})
include_directories(SYSTEM ${QT_INCLUDES} ${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/)



if(CMAKE_COMPILER_IS_GNUCXX)
	set(compiler_flags_debug "-Wall -Wextra -Wunreachable-code")        ## Optimize -O3 -Weverything -Weffc++
	# set(compiler_flags_release "-std=c++11")        ## Optimize -O3 -Weverything
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()


set(srcs_directories data_structure octxmlread widgets windows)
set(qt_header_directories widgets windows)
set(ui_directories widgets)

foreach(loop_var ${srcs_directories})
	file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/*.cpp")
	list(APPEND octmarker_SRCS ${sources})
endforeach(loop_var)

foreach(loop_var ${qt_header_directories})
	file(GLOB header "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/*.h")
	list(APPEND octmarker_HEADERS ${header})
endforeach(loop_var)

foreach(loop_var ${ui_directories})
	file(GLOB ui_files "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/ui/*.ui")
	list(APPEND ui_forms ${ui_files})
endforeach(loop_var)



QT4_WRAP_UI(ui_files_headers ${ui_forms})
QT4_WRAP_CPP(octmarker_HEADERS_MOC ${octmarker_HEADERS})
QT4_ADD_RESOURCES(octmarker_RESOURCES_RCC ${octmarker_RESOURCES})
QT4_ADD_TRANSLATION(TRANSLATIONS ${LANG})



if(compiler_flags_debug)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${compiler_flags_debug}")
endif()
if(compiler_flags_release)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${compiler_flags_release}")
endif()
if(compiler_flags)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_flags}")
endif()



add_executable(octmarker src/main.cpp ${octmarker_SRCS}
	${ui_files_headers}
	${octmarker_HEADERS_MOC}
	${TRANSLATIONS})

target_link_libraries(octmarker ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS octmarker RUNTIME DESTINATION bin)
