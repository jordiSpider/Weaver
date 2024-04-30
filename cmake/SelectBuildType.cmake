
if(NOT DEFINED CMAKE_BUILD_TYPE)
	message(FATAL_ERROR "Variable 'CMAKE_BUILD_TYPE' not defined. Choose the type of build, options are: Debug or Release.")
endif()


if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
	set(RELEASE_MODE ON)
	set(DEBUG_MODE OFF)
elseif(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug$")
	set(RELEASE_MODE OFF)
	set(DEBUG_MODE ON)
else()
	message(FATAL_ERROR "Unknown Cmake build type. Options are: Debug Release.")
endif()
