cmake_minimum_required(VERSION 3.25)
project(WeaverGitHubPagesDocs NONE)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(DOCS_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/source" CACHE PATH "Root directory containing versioned documentation sources")
set(DOCS_BUILD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build/html" CACHE PATH "Root directory for generated HTML")
set(DOCS_WORK_ROOT "${CMAKE_CURRENT_BINARY_DIR}/work" CACHE PATH "Temporary documentation build directory")
set(DOCS_DEFAULT_LANGUAGE "en" CACHE STRING "Default Sphinx output language")
set(DOCS_GITHUB_REPOSITORY "jordiSpider/Weaver" CACHE STRING "GitHub repository in owner/name format")
set(DOCS_GITHUB_API_URL "https://api.github.com" CACHE STRING "GitHub API base URL")
set(DOCS_SPHINX_JOBS "1" CACHE STRING "Number of Sphinx workers")
set(DOCS_GENERATION "" CACHE STRING "Component selected for generation")

if(NOT DOCS_GENERATION STREQUAL "" AND NOT DOCS_GENERATION MATCHES "^(weaver|manager|landing|remove-weaver|remove-manager)$")
	message(FATAL_ERROR "DOCS_GENERATION must be weaver, manager, landing, remove-weaver, or remove-manager")
endif()

if(DOCS_GENERATION STREQUAL "weaver" AND (NOT DEFINED DOCS_WEAVER_VERSION OR "${DOCS_WEAVER_VERSION}" STREQUAL ""))
	message(FATAL_ERROR "Set DOCS_WEAVER_VERSION to the version source directory to generate")
elseif(DOCS_GENERATION STREQUAL "manager" AND (NOT DEFINED DOCS_MANAGER_VERSION OR "${DOCS_MANAGER_VERSION}" STREQUAL ""))
	message(FATAL_ERROR "Set DOCS_MANAGER_VERSION to the version source directory to generate")
elseif(DOCS_GENERATION STREQUAL "remove-weaver" AND (NOT DEFINED DOCS_WEAVER_VERSION OR "${DOCS_WEAVER_VERSION}" STREQUAL ""))
	message(FATAL_ERROR "Set DOCS_WEAVER_VERSION to the version source directory to remove")
elseif(DOCS_GENERATION STREQUAL "remove-manager" AND (NOT DEFINED DOCS_MANAGER_VERSION OR "${DOCS_MANAGER_VERSION}" STREQUAL ""))
	message(FATAL_ERROR "Set DOCS_MANAGER_VERSION to the version source directory to remove")
endif()

foreach(product WEAVER MANAGER)
	if(product STREQUAL "WEAVER")
		set(version_variable DOCS_WEAVER_VERSION)
	else()
		set(version_variable DOCS_MANAGER_VERSION)
	endif()
	if(DEFINED ${version_variable} AND NOT "${${version_variable}}" STREQUAL "")
		if("${${version_variable}}" MATCHES "[/\\\\]" OR "${${version_variable}}" STREQUAL "." OR "${${version_variable}}" STREQUAL "..")
			message(FATAL_ERROR "${version_variable} must be a single path component")
		endif()
	endif()
endforeach()

if(NOT IS_DIRECTORY "${DOCS_SOURCE_ROOT}")
	message(FATAL_ERROR "Documentation source directory not found: ${DOCS_SOURCE_ROOT}")
endif()

set(DOCS_SHARED_SOURCE_DIR "${DOCS_SOURCE_ROOT}/_shared")
set(DOCS_WEAVER_SOURCE_DIR "${DOCS_SOURCE_ROOT}/weaver")
set(DOCS_MANAGER_SOURCE_DIR "${DOCS_SOURCE_ROOT}/weaver-manager")
set(DOCS_LANDING_SOURCE_DIR "${DOCS_SOURCE_ROOT}/landing")
set(DOCS_WEAVER_OUTPUT_ROOT "${DOCS_BUILD_ROOT}/weaver")
set(DOCS_MANAGER_OUTPUT_ROOT "${DOCS_BUILD_ROOT}/weaver-manager")
set(DOCS_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

function(add_catalog_update_target target product source_dir)
	if(NOT IS_DIRECTORY "${source_dir}")
		message(FATAL_ERROR "${product} documentation source directory not found: ${source_dir}")
	endif()
	add_custom_target(${target}
		COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/UpdateReleaseCatalogs.py"
			--repository "${DOCS_GITHUB_REPOSITORY}"
			--api-url "${DOCS_GITHUB_API_URL}"
			--product "${product}"
			--source-root "${source_dir}"
			--catalog "${DOCS_BUILD_ROOT}/${product}/versions.json"
		VERBATIM
		COMMENT "Updating ${product} documentation release catalog from GitHub Releases"
	)
endfunction()

add_custom_target(remove-weaver-version
	COMMAND "${CMAKE_COMMAND}"
		-DPYTHON_EXECUTABLE=${Python3_EXECUTABLE}
		-DREMOVE_SCRIPT=${DOCS_SCRIPT_DIR}/RemoveDocsVersion.py
		-DPRODUCT=weaver
		-DVERSION=${DOCS_WEAVER_VERSION}
		-DREPOSITORY=${DOCS_GITHUB_REPOSITORY}
		-DAPI_URL=${DOCS_GITHUB_API_URL}
		-DSOURCE_ROOT=${DOCS_WEAVER_SOURCE_DIR}
		-DOUTPUT_ROOT=${DOCS_WEAVER_OUTPUT_ROOT}
		-DCATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json
		-P "${DOCS_SCRIPT_DIR}/RemoveDocsVersion.cmake"
	COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/UpdateVersionSelectors.py"
		--product "weaver"
		--catalog "${DOCS_BUILD_ROOT}/weaver/versions.json"
		--output-root "${DOCS_WEAVER_OUTPUT_ROOT}"
		--source-root "${DOCS_SOURCE_ROOT}"
		--default-language "${DOCS_DEFAULT_LANGUAGE}"
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_LANDING_SOURCE=${DOCS_LANDING_SOURCE_DIR}"
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/landing"
		"DOCS_BUILD_ROOT=${DOCS_BUILD_ROOT}"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_MANAGER_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLanding.py"
	VERBATIM
	COMMENT "Removing Weaver documentation version ${DOCS_WEAVER_VERSION}"
)

add_custom_target(remove-manager-version
	COMMAND "${CMAKE_COMMAND}"
		-DPYTHON_EXECUTABLE=${Python3_EXECUTABLE}
		-DREMOVE_SCRIPT=${DOCS_SCRIPT_DIR}/RemoveDocsVersion.py
		-DPRODUCT=weaver-manager
		-DVERSION=${DOCS_MANAGER_VERSION}
		-DREPOSITORY=${DOCS_GITHUB_REPOSITORY}
		-DAPI_URL=${DOCS_GITHUB_API_URL}
		-DSOURCE_ROOT=${DOCS_MANAGER_SOURCE_DIR}
		-DOUTPUT_ROOT=${DOCS_MANAGER_OUTPUT_ROOT}
		-DCATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json
		-P "${DOCS_SCRIPT_DIR}/RemoveDocsVersion.cmake"
	COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/UpdateVersionSelectors.py"
		--product "weaver-manager"
		--catalog "${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		--output-root "${DOCS_MANAGER_OUTPUT_ROOT}"
		--source-root "${DOCS_SOURCE_ROOT}"
		--default-language "${DOCS_DEFAULT_LANGUAGE}"
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_LANDING_SOURCE=${DOCS_LANDING_SOURCE_DIR}"
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/landing"
		"DOCS_BUILD_ROOT=${DOCS_BUILD_ROOT}"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_MANAGER_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLanding.py"
	VERBATIM
	COMMENT "Removing Weaver Manager documentation version ${DOCS_MANAGER_VERSION}"
)

add_catalog_update_target(update-weaver-catalog "weaver" "${DOCS_WEAVER_SOURCE_DIR}")
add_catalog_update_target(update-manager-catalog "weaver-manager" "${DOCS_MANAGER_SOURCE_DIR}")

add_custom_target(generate-weaver
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_OUTPUT_ROOT=${DOCS_WEAVER_OUTPUT_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/weaver"
		"DOCS_SHARED_SOURCE=${DOCS_SHARED_SOURCE_DIR}"
		"DOCS_PRODUCT=weaver"
		"DOCS_VERSION=${DOCS_WEAVER_VERSION}"
		"DOCS_DEFAULT_LANGUAGE=${DOCS_DEFAULT_LANGUAGE}"
		"DOCS_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildVersionedDocs.py"
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_LANDING_SOURCE=${DOCS_LANDING_SOURCE_DIR}"
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/landing"
		"DOCS_BUILD_ROOT=${DOCS_BUILD_ROOT}"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_MANAGER_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLanding.py"
	VERBATIM
	COMMENT "Generating Weaver documentation version ${DOCS_WEAVER_VERSION}"
)

add_custom_target(generate-manager
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_OUTPUT_ROOT=${DOCS_MANAGER_OUTPUT_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/weaver-manager"
		"DOCS_SHARED_SOURCE=${DOCS_SHARED_SOURCE_DIR}"
		"DOCS_PRODUCT=weaver-manager"
		"DOCS_VERSION=${DOCS_MANAGER_VERSION}"
		"DOCS_DEFAULT_LANGUAGE=${DOCS_DEFAULT_LANGUAGE}"
		"DOCS_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildVersionedDocs.py"
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_LANDING_SOURCE=${DOCS_LANDING_SOURCE_DIR}"
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/landing"
		"DOCS_BUILD_ROOT=${DOCS_BUILD_ROOT}"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_MANAGER_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLanding.py"
	VERBATIM
	COMMENT "Generating Weaver Manager documentation version ${DOCS_MANAGER_VERSION}"
)

add_custom_target(generate-landing
	COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/UpdateVersionSelectors.py"
		--product "weaver"
		--catalog "${DOCS_BUILD_ROOT}/weaver/versions.json"
		--output-root "${DOCS_WEAVER_OUTPUT_ROOT}"
		--source-root "${DOCS_SOURCE_ROOT}"
		--default-language "${DOCS_DEFAULT_LANGUAGE}"
	COMMAND "${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/UpdateVersionSelectors.py"
		--product "weaver-manager"
		--catalog "${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		--output-root "${DOCS_MANAGER_OUTPUT_ROOT}"
		--source-root "${DOCS_SOURCE_ROOT}"
		--default-language "${DOCS_DEFAULT_LANGUAGE}"
	COMMAND "${CMAKE_COMMAND}" -E env
		"DOCS_LANDING_SOURCE=${DOCS_LANDING_SOURCE_DIR}"
		"DOCS_SOURCE_ROOT=${DOCS_SOURCE_ROOT}"
		"DOCS_WORK_ROOT=${DOCS_WORK_ROOT}/landing"
		"DOCS_BUILD_ROOT=${DOCS_BUILD_ROOT}"
		"DOCS_WEAVER_CATALOG=${DOCS_BUILD_ROOT}/weaver/versions.json"
		"DOCS_MANAGER_CATALOG=${DOCS_BUILD_ROOT}/weaver-manager/versions.json"
		"DOCS_SPHINX_JOBS=${DOCS_SPHINX_JOBS}"
		"${Python3_EXECUTABLE}" "${DOCS_SCRIPT_DIR}/BuildLanding.py"
	VERBATIM
	COMMENT "Regenerating documentation landing page"
)

add_dependencies(generate-weaver update-weaver-catalog)
add_dependencies(generate-weaver update-manager-catalog)
add_dependencies(generate-manager update-manager-catalog)
add_dependencies(generate-manager update-weaver-catalog)
add_dependencies(generate-landing update-weaver-catalog update-manager-catalog)
