cmake_minimum_required(VERSION 3.25)


# ------------------------------------------------------------
# Create the vcpkg cache directory if it was injected by the preset
# ------------------------------------------------------------
if(DEFINED ENV{VCPKG_DEFAULT_BINARY_CACHE})
    file(MAKE_DIRECTORY "$ENV{VCPKG_DEFAULT_BINARY_CACHE}")
endif()


project(WeaverManager VERSION 1.0.1 LANGUAGES CXX)

option(ENABLE_TEST "Enable test-only Manager features" OFF)

set(WEAVER_MANAGER_EXECUTABLE_NAME "Weaver Manager" CACHE STRING "Manager executable name")
set(WEAVER_MANAGER_INSTALLER_SUFFIX "" CACHE STRING "Suffix appended to Manager installer names")
set(WEAVER_MANAGER_DIRECTORY_SUFFIX "" CACHE STRING "Suffix appended to Manager installation and data directories")
set(WEAVER_MANAGER_NAME "Weaver Manager${WEAVER_MANAGER_DIRECTORY_SUFFIX}")
set(WEAVER_MANAGER_INSTALL_DIRECTORY "Weaver Manager${WEAVER_MANAGER_DIRECTORY_SUFFIX}")
set(WEAVER_MANAGER_DATA_DIRECTORY_NAME "Weaver Manager${WEAVER_MANAGER_DIRECTORY_SUFFIX}")
set(WEAVER_MANAGER_REGISTRY_KEY "Software\\Weaver\\${WEAVER_MANAGER_INSTALL_DIRECTORY}")
if(WEAVER_MANAGER_DIRECTORY_SUFFIX)
    set(WEAVER_MANAGER_DATA_FOLDER "Weaver Manager Test")
else()
    set(WEAVER_MANAGER_DATA_FOLDER "Weaver Manager")
endif()
set(WEAVER_MANAGER_EXECUTABLE_FILE_NAME "${WEAVER_MANAGER_EXECUTABLE_NAME}.exe")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

set(WEAVER_RELEASE_PUBLIC_KEY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/release-signing-public.pem" CACHE FILEPATH
    "Ed25519 public key (PEM or DER) used to verify release manifests")
set(WEAVER_RELEASE_PUBLIC_KEY_BASE64 "BgCuAxCIw6pFGVM4R/84WVMnNm4BI84q4g8WKZGC4wk=" CACHE STRING
    "Fallback raw 32-byte Ed25519 public key, Base64 encoded")

if(WEAVER_RELEASE_PUBLIC_KEY_FILE AND EXISTS "${WEAVER_RELEASE_PUBLIC_KEY_FILE}")
    find_program(WEAVER_OPENSSL_EXECUTABLE
        NAMES openssl openssl.exe
        HINTS
            "${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/tools/openssl"
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/openssl"
        REQUIRED
    )

    set(_weaver_key_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    file(MAKE_DIRECTORY "${_weaver_key_dir}")
    set(_weaver_public_key_der "${_weaver_key_dir}/release-signing-public.der")
    set(_weaver_public_key_raw "${_weaver_key_dir}/release-signing-public.raw")
    execute_process(
        COMMAND "${WEAVER_OPENSSL_EXECUTABLE}" pkey -pubin -in "${WEAVER_RELEASE_PUBLIC_KEY_FILE}" -pubout -outform DER -out "${_weaver_public_key_der}"
        RESULT_VARIABLE _weaver_key_export_result
        ERROR_VARIABLE _weaver_key_export_error
    )
    if(NOT _weaver_key_export_result EQUAL 0)
        message(FATAL_ERROR "Could not read Ed25519 public key '${WEAVER_RELEASE_PUBLIC_KEY_FILE}': ${_weaver_key_export_error}")
    endif()

    # Start parsing at the BIT STRING tag in the Ed25519 SubjectPublicKeyInfo;
    # OpenSSL's asn1parse -out writes the BIT STRING payload without its unused
    # bits marker, leaving the raw 32-byte public key.
    execute_process(
        COMMAND "${WEAVER_OPENSSL_EXECUTABLE}" asn1parse -inform DER -in "${_weaver_public_key_der}" -strparse 9 -out "${_weaver_public_key_raw}" -noout
        RESULT_VARIABLE _weaver_key_extract_result
        ERROR_VARIABLE _weaver_key_extract_error
    )
    if(NOT _weaver_key_extract_result EQUAL 0)
        message(FATAL_ERROR "Could not extract raw Ed25519 public key: ${_weaver_key_extract_error}")
    endif()
    file(SIZE "${_weaver_public_key_raw}" _weaver_public_key_raw_size)
    if(NOT _weaver_public_key_raw_size EQUAL 32)
        message(FATAL_ERROR "Expected a 32-byte Ed25519 public key, got ${_weaver_public_key_raw_size} bytes from '${WEAVER_RELEASE_PUBLIC_KEY_FILE}'")
    endif()
    execute_process(
        COMMAND "${WEAVER_OPENSSL_EXECUTABLE}" base64 -A -in "${_weaver_public_key_raw}"
        RESULT_VARIABLE _weaver_key_base64_result
        OUTPUT_VARIABLE _weaver_public_key_base64
        ERROR_VARIABLE _weaver_key_base64_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT _weaver_key_base64_result EQUAL 0)
        message(FATAL_ERROR "Could not Base64-encode Ed25519 public key: ${_weaver_key_base64_error}")
    endif()
    set(WEAVER_RELEASE_PUBLIC_KEY_BASE64 "${_weaver_public_key_base64}")
    message(STATUS "Release verification key loaded from ${WEAVER_RELEASE_PUBLIC_KEY_FILE}")
else()
    message(STATUS "Release verification key file not found; using WEAVER_RELEASE_PUBLIC_KEY_BASE64 fallback")
endif()
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file(src/release_trust_config.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/generated/release_trust_config.h" @ONLY)
configure_file(resources/weaver-manager.desktop
    "${CMAKE_CURRENT_BINARY_DIR}/generated/weaver-manager.desktop" @ONLY)

add_subdirectory(src)
set_target_properties(WeaverManager PROPERTIES OUTPUT_NAME "${WEAVER_MANAGER_EXECUTABLE_NAME}")
target_include_directories(WeaverManager PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(WeaverManager PRIVATE Qt6::Widgets Qt6::Network OpenSSL::Crypto)

if(WIN32)
    # Qt's install deployment script only runs when installing/packaging. Also
    # deploy beside the build executable so it can be launched from the build
    # tree during development.
    find_program(WEAVER_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt.exe windeployqt
        HINTS "${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/tools/Qt6/bin"
        REQUIRED
    )

    add_executable(WeaverManagerUninstaller WIN32 src/manager_uninstaller.cpp)
    target_link_libraries(WeaverManagerUninstaller PRIVATE Qt6::Widgets)
    target_compile_definitions(WeaverManagerUninstaller PRIVATE
        WEAVER_MANAGER_NAME="${WEAVER_MANAGER_NAME}"
        WEAVER_MANAGER_DATA_DIRECTORY_NAME="${WEAVER_MANAGER_DATA_DIRECTORY_NAME}"
    )
    add_custom_command(TARGET WeaverManagerUninstaller POST_BUILD
        COMMAND "${WEAVER_WINDEPLOYQT_EXECUTABLE}"
                "$<TARGET_FILE:WeaverManagerUninstaller>"
                --dir "$<TARGET_FILE_DIR:WeaverManagerUninstaller>"
                --no-translations
        COMMENT "Deploying Qt runtime for Weaver Manager Uninstaller"
        VERBATIM
    )
    qt_generate_deploy_app_script(TARGET WeaverManagerUninstaller NO_TRANSLATIONS
        OUTPUT_SCRIPT uninstaller_deploy_script)
endif()

target_compile_definitions(WeaverManager PRIVATE
    WEAVER_MANAGER_VERSION="${PROJECT_VERSION}"
    WEAVER_MANAGER_NAME="${WEAVER_MANAGER_NAME}"
    WEAVER_MANAGER_EXECUTABLE_NAME="${WEAVER_MANAGER_EXECUTABLE_NAME}"
    WEAVER_MANAGER_DATA_DIRECTORY_NAME="${WEAVER_MANAGER_DATA_DIRECTORY_NAME}"
    WEAVER_REPOSITORY="jordiSpider/Weaver"
)
if(ENABLE_TEST)
    target_compile_definitions(WeaverManager PRIVATE ENABLE_TEST=1)
endif()
qt_generate_deploy_app_script(TARGET WeaverManager NO_TRANSLATIONS OUTPUT_SCRIPT deploy_script)

include(GNUInstallDirs)
install(TARGETS WeaverManager RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(WIN32)
    install(TARGETS WeaverManagerUninstaller RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(SCRIPT ${uninstaller_deploy_script})
endif()
install(SCRIPT ${deploy_script})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/generated/weaver-manager.desktop"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications OPTIONAL)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/licenses/${WEAVER_MANAGER_NAME}"
    RENAME LICENSE)

set(CPACK_PACKAGE_NAME "${WEAVER_MANAGER_NAME}")
set(CPACK_PACKAGE_VENDOR "Weaver Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Manage parallel Weaver installations")
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(WEAVER_MANAGER_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
if(WEAVER_MANAGER_ARCH MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
    set(WEAVER_MANAGER_ARCH "x64")
elseif(WEAVER_MANAGER_ARCH MATCHES "^(aarch64|AARCH64|arm64|ARM64)$")
    set(WEAVER_MANAGER_ARCH "arm64")
endif()
if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "Weaver-Manager${WEAVER_MANAGER_INSTALLER_SUFFIX}-${PROJECT_VERSION}-Windows-${WEAVER_MANAGER_ARCH}")
elseif(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "Weaver-Manager${WEAVER_MANAGER_INSTALLER_SUFFIX}-${PROJECT_VERSION}-macOS-${WEAVER_MANAGER_ARCH}")
elseif(UNIX)
    set(WEAVER_MANAGER_OS_RELEASE "Linux")
    if(EXISTS "/etc/os-release")
        file(READ "/etc/os-release" WEAVER_MANAGER_OS_RELEASE_CONTENT)
        string(REGEX MATCH "PRETTY_NAME=\"([^\"]+)\"" _ ${WEAVER_MANAGER_OS_RELEASE_CONTENT})
        if(CMAKE_MATCH_1)
            string(REPLACE " " "-" WEAVER_MANAGER_OS_RELEASE "${CMAKE_MATCH_1}")
        endif()
    endif()
    set(CPACK_PACKAGE_FILE_NAME "Weaver-Manager${WEAVER_MANAGER_INSTALLER_SUFFIX}-${PROJECT_VERSION}-${WEAVER_MANAGER_OS_RELEASE}-${WEAVER_MANAGER_ARCH}")
endif()
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${WEAVER_MANAGER_INSTALL_DIRECTORY}")
set(CPACK_PACKAGE_CONTACT "jordi@eeza.csic.es")
set(CPACK_PACKAGE_EXECUTABLES "${WEAVER_MANAGER_EXECUTABLE_NAME}" "${WEAVER_MANAGER_NAME}")
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    configure_file(resources/manager_data_directory.nsh.in
        "${CMAKE_CURRENT_BINARY_DIR}/generated/manager_data_directory.nsh" @ONLY)
    file(READ "${CMAKE_CURRENT_BINARY_DIR}/generated/manager_data_directory.nsh"
        _weaver_manager_data_nsis_code)
    string(REPLACE "\\" "\\\\" _weaver_manager_data_nsis_code "${_weaver_manager_data_nsis_code}")
    string(REPLACE "\"" "\\\"" _weaver_manager_data_nsis_code "${_weaver_manager_data_nsis_code}")
    string(REPLACE "\${" "\\\${" _weaver_manager_data_nsis_code "${_weaver_manager_data_nsis_code}")
    set(CPACK_NSIS_DEFINES "${_weaver_manager_data_nsis_code}")
    set(CPACK_NSIS_PAGE_COMPONENTS "Page custom ManagerDataDirPageCreate ManagerDataDirPageLeave")
    set(CPACK_NSIS_DISPLAY_NAME "${WEAVER_MANAGER_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME "${WEAVER_MANAGER_NAME}")
    set(CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64")
    set(CPACK_NSIS_UNINSTALL_NAME "Uninstall ${WEAVER_MANAGER_NAME}")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/jordiSpider/Weaver")
    set(CPACK_NSIS_CONTACT "jordi@eeza.csic.es")
    set(CPACK_NSIS_MENU_LINKS "bin/${WEAVER_MANAGER_EXECUTABLE_FILE_NAME}" "${WEAVER_MANAGER_NAME}")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "!insertmacro ManagerDataDirInstall")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS [=[
ClearErrors
ExecWait '\"$INSTDIR/bin/WeaverManagerUninstaller.exe\"' $0
IfErrors uninstall_managed_versions_failed
IntCmp $0 0 uninstall_managed_versions_done uninstall_managed_versions_failed uninstall_managed_versions_failed
uninstall_managed_versions_failed:
MessageBox MB_ICONSTOP \"Weaver Manager cannot be removed because one or more managed Weaver versions could not be uninstalled.\"
Abort
uninstall_managed_versions_done:
]=])
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB")
    string(TOLOWER "${WEAVER_MANAGER_INSTALLER_SUFFIX}" WEAVER_MANAGER_DEBIAN_SUFFIX)
    set(CPACK_DEBIAN_PACKAGE_NAME "weaver-manager${WEAVER_MANAGER_DEBIAN_SUFFIX}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Weaver Project")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6network6, libcrypto3")
endif()
include(CPack)
