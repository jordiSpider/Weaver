cmake_minimum_required(VERSION 3.25)
project(WeaverLocalDocs NONE)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(DOCS_GENERATION "" CACHE STRING "Local documentation product to build: weaver, weaver-xml, or manager")
set(DOCS_WORK_ROOT "${CMAKE_CURRENT_BINARY_DIR}/work" CACHE PATH "Temporary local documentation build directory")
set(DOCS_DEFAULT_LANGUAGE "en" CACHE STRING "Default Sphinx output language")
set(DOCS_SPHINX_JOBS "1" CACHE STRING "Number of Sphinx workers")
set(DOCS_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

if(NOT DOCS_GENERATION MATCHES "^(weaver|weaver-xml|manager)$")
	message(FATAL_ERROR "DOCS_GENERATION must be weaver, weaver-xml, or manager")
endif()

if(DOCS_GENERATION STREQUAL "manager")
	set(selected_product "weaver-manager")
else()
	set(selected_product "weaver")
endif()
set(selected_source "${CMAKE_CURRENT_SOURCE_DIR}/source/${selected_product}/local")
if(NOT IS_DIRECTORY "${selected_source}")
	message(FATAL_ERROR "Local documentation source not found: ${selected_source}")
endif()

foreach(product weaver weaver-manager)
	if(product STREQUAL "weaver")
		set(target generate-local-weaver)
	else()
		set(target generate-local-manager)
	endif()
	add_custom_target(${target}
		COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLocalDocs.py"
			--product "${product}"
			--source "${CMAKE_CURRENT_SOURCE_DIR}/source/${product}/local"
			--output-root "${CMAKE_CURRENT_SOURCE_DIR}/build/${product}"
			--work-root "${DOCS_WORK_ROOT}/${product}"
			--default-language "${DOCS_DEFAULT_LANGUAGE}"
			--jobs "${DOCS_SPHINX_JOBS}"
		VERBATIM
		COMMENT "Generating local ${product} documentation as latest"
	)
endforeach()

if(DOCS_GENERATION MATCHES "^(weaver|weaver-xml)$")
	set(DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
	set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
	set(DOXYGEN_VERSION "1.9.8" CACHE STRING "Minimum Doxygen version")
	set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/weaver/local/api")
	set(DOXYFILE_PROJECT_VERSION "Local")
	set(DOXYGEN_INPUT_DIR "${INCLUDE_DIR}")
	set(DOXYGEN_DOCS_DIR "${DOCS_DIR}")
	set(CMAKE_PROJECT_NAME "Weaver")

	find_package(Doxygen ${DOXYGEN_VERSION} REQUIRED)
	get_filename_component(DOXYGEN_OUTPUT_DIR "${DOXYGEN_OUTPUT_DIR}" ABSOLUTE)
	get_filename_component(DOXYGEN_INPUT_DIR "${DOXYGEN_INPUT_DIR}" ABSOLUTE)
	get_filename_component(DOXYGEN_DOCS_DIR "${DOXYGEN_DOCS_DIR}" ABSOLUTE)
	file(TO_CMAKE_PATH "${DOXYGEN_OUTPUT_DIR}" DOXYGEN_OUTPUT_DIR)
	file(TO_CMAKE_PATH "${DOXYGEN_INPUT_DIR}" DOXYGEN_INPUT_DIR)
	file(TO_CMAKE_PATH "${DOXYGEN_DOCS_DIR}" DOXYGEN_DOCS_DIR)
	configure_file("${DOCS_DIR}/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

	add_custom_target(clean-local-weaver-xml
		COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/CleanDoxygenXml.py" "${DOXYGEN_OUTPUT_DIR}/xml"
		COMMENT "Cleaning local Weaver Doxygen XML output..."
		VERBATIM
	)
	add_custom_target(generate-local-weaver-xml
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${DOXYGEN_OUTPUT_DIR}"
		COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
		COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/NormalizeDoxygenXmlPaths.py" "${DOXYGEN_OUTPUT_DIR}/xml"
		WORKING_DIRECTORY "${DOCS_DIR}"
		COMMENT "Generating local Weaver API XML with Doxygen..."
		VERBATIM
	)
	add_dependencies(generate-local-weaver-xml clean-local-weaver-xml)
	if(DOCS_GENERATION STREQUAL "weaver")
		add_dependencies(generate-local-weaver generate-local-weaver-xml)
	endif()
endif()
