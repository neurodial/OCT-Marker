cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(octmarker)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_module/)

set( CMAKE_CXX_STANDARD 11 )


find_package(Boost 1.40 COMPONENTS filesystem serialization system iostreams REQUIRED)
# find_package (Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(LibOctData REQUIRED)
find_package(CppFramework REQUIRED)


option(BUILD_WITH_SEGMENTATION_ML    "build with support for NN"     OFF)
option(BUILD_MATLAB_MEX_FUNCTIONS    "build matlab mex functions"    OFF)
option(BUILD_OCTAVE_MEX_FUNCTIONS    "build octave mex functions"    OFF)
option(BUILD_QT_PROGRAMM             "build main programm"           ON )
option(BUILD_MEX_WITH_STATIC_CPP_LIB "build mex with static c++ lib" OFF)


set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel.")


if(BUILD_QT_PROGRAMM)

	find_package(Qt5Core)
	find_package(Qt5Widgets)
	find_package(Qt5LinguistTools)
	find_package(Qt5Svg)


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
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wunreachable-code -Wconversion") # ")        ## Optimize -O3 -Weverything -Weffc++
		if(WIN32)
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -mwindows")
		endif()
	endif()



	if(BUILD_WITH_SEGMENTATION_ML)
		find_package(FANN REQUIRED)
		add_definitions(-DML_SUPPORT)
		include_directories(SYSTEM ${FANN_INCLUDE_DIR})
	endif()


	set(srcs_directories data_structure algos widgets windows manager markermodules model markerobjects helper imagefilter)

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
		${octmarker_RESOURCES_RCC})

	if(WIN32 AND MSVC)
		# suppress dos window
		set_target_properties(octmarker PROPERTIES LINK_FLAGS_RELEASE "/subsystem:windows /ENTRY:mainCRTStartup")
	endif()

	target_link_libraries(octmarker Qt5::Core Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${OpenCV_LIBS})
	target_link_libraries(octmarker ${LibOctData_LIBRARIES})
	target_link_libraries(octmarker ${CppFramework_LIBRARIES})
	target_link_libraries(octmarker ${FANN_LIBRARY})

	install(TARGETS octmarker RUNTIME DESTINATION bin)

endif()


if(BUILD_MATLAB_MEX_FUNCTIONS)
	find_package(Matlab COMPONENTS MX_LIBRARY REQUIRED)

	matlab_add_mex(NAME read_seg SRC src_matlab/read_seg.cpp src/manager/octmarkerio.cpp src/data_structure/simplematcompress.cpp LINK_TO ${Boost_LIBRARIES})
	set_target_properties(read_seg PROPERTIES COMPILE_DEFINITIONS "MEX_COMPILE")
	if(BUILD_MEX_WITH_STATIC_CPP_LIB)
		set_target_properties(read_seg PROPERTIES LINK_FLAGS "-static-libstdc++")
	endif()

	target_include_directories(read_seg SYSTEM PRIVATE ${Matlab_INCLUDE_DIRS} PUBLIC ${CMAKE_SOURCE_DIR}/src/)
endif()


if(BUILD_OCTAVE_MEX_FUNCTIONS)
	find_package(Octave COMPONENTS MX_LIBRARY REQUIRED)

	octave_add_oct(oct_read_seg SOURCES src_matlab/read_seg.cpp src/manager/octmarkerio.cpp src/data_structure/simplematcompress.cpp LINK_LIBRARIES ${Boost_LIBRARIES} EXTENSION mex)
	set_target_properties(oct_read_seg PROPERTIES COMPILE_DEFINITIONS "MEX_COMPILE")
	if(BUILD_MEX_WITH_STATIC_CPP_LIB)
		set_target_properties(oct_read_seg PROPERTIES LINK_FLAGS "-static-libstdc++")
	endif()
	target_include_directories(oct_read_seg SYSTEM PRIVATE ${OCTAVE_INCLUDE_DIRS} PUBLIC ${CMAKE_SOURCE_DIR}/src/)

endif()

