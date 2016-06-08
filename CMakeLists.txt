cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(octmarker)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)
find_package(Boost 1.40 COMPONENTS program_options filesystem system REQUIRED)
# find_package (Threads REQUIRED)
find_package(OpenCV REQUIRED)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # ${Boost_INCLUDE_DIR})
include_directories(SYSTEM ${QT_INCLUDES} ${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/)


if(CMAKE_COMPILER_IS_GNUCXX)
	set(compiler_flags_debug "-Wall -Wextra -Wunreachable-code")        ## Optimize -O3 -Weverything -Weffc++
	# set(compiler_flags_release "-std=c++11")        ## Optimize -O3 -Weverything
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()


set(srcs_directories data_structure octxmlread widgets windows manager)

foreach(loop_var ${srcs_directories})
	file(GLOB sources_base "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/*.cpp")
	file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/**/*.cpp")
	list(APPEND octmarker_SRCS ${sources} ${sources_base})
endforeach(loop_var)


file(GLOB LANG ${CMAKE_CURRENT_SOURCE_DIR}/language_files/*.ts)

set(octmarker_RESOURCES application.qrc)

# qt5_create_translation(qm_files ${srcs_directories} sources ${LANG})

QT5_ADD_RESOURCES(octmarker_RESOURCES_RCC ${octmarker_RESOURCES})
QT5_ADD_TRANSLATION(TRANSLATIONS ${LANG})


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
	${TRANSLATIONS}
	${octmarker_RESOURCES_RCC}
	${TRANSLATIONS})

target_link_libraries(octmarker Qt5::Core Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS octmarker RUNTIME DESTINATION bin)
