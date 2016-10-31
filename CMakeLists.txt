cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(octmarker)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_module/)

set_property(GLOBAL PROPERTY CXX_STANDARD 11)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)


find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)
find_package(Boost 1.40 COMPONENTS program_options filesystem serialization system REQUIRED)
# find_package (Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(LibOctData REQUIRED)
find_package(CppFramework REQUIRED)


set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel.")


include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/buildconstants.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/buildconstants.cpp" @ONLY)
list(APPEND octmarker_SRCS "${CMAKE_CURRENT_BINARY_DIR}/buildconstants.cpp")


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(SYSTEM ${QT_INCLUDES} ${CMAKE_BINARY_DIR})
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${LibOctData_INCLUDE_DIRS})
include_directories(${CppFramework_INCLUDE_DIRS})


if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunreachable-code") #  -Wconversion")        ## Optimize -O3 -Weverything -Weffc++
	if(WIN32)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -mwindows")
	endif()
endif()


set(srcs_directories data_structure octxmlread widgets windows manager markermodules model markerobjects helper)

foreach(loop_var ${srcs_directories})
	file(GLOB sources_base "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/*.cpp")
	file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/src/${loop_var}/**/*.cpp")
	list(APPEND octmarker_SRCS ${sources} ${sources_base})
endforeach(loop_var)

file(GLOB ui_forms ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)


file(GLOB LANG ${CMAKE_CURRENT_SOURCE_DIR}/language_files/*.ts)

set(octmarker_RESOURCES application.qrc)


QT5_WRAP_UI(ui_files_headers ${ui_forms})
QT5_ADD_RESOURCES(octmarker_RESOURCES_RCC ${octmarker_RESOURCES})
QT5_ADD_TRANSLATION(TRANSLATIONS ${LANG})


add_executable(octmarker src/main.cpp ${octmarker_SRCS}
	${ui_files_headers}
	${TRANSLATIONS}
	${octmarker_RESOURCES_RCC}
	${TRANSLATIONS})

if(WIN32 AND MSVC)
	set_target_properties(octmarker PROPERTIES LINK_FLAGS_RELEASE "/subsystem:windows /ENTRY:mainCRTStartup")
endif()

target_link_libraries(octmarker Qt5::Core Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(octmarker ${LibOctData_LIBRARIES})
target_link_libraries(octmarker ${CppFramework_LIBRARIES})

install(TARGETS octmarker RUNTIME DESTINATION bin)

