
# ==============================================================================
# 📜 Automated Third-Party License Notices Generator for vcpkg Dependencies
# ==============================================================================
function(generate_third_party_notices VCPKG_INSTALLED_DIR OUTPUT_PATH)
    set(VCPKG_SHARE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share")
    set(MD_CONTENT "# Third-Party Notices\n\n")
    string(APPEND MD_CONTENT "This software is based in part on the work of the following open-source projects. ")
    string(APPEND MD_CONTENT "Licenses for these dependencies are listed below.\n\n---\n\n")

    # 1. Process vcpkg-installed port dependencies
    if(EXISTS "${VCPKG_SHARE_DIR}")
        file(GLOB INSTALLED_PORTS RELATIVE "${VCPKG_SHARE_DIR}" "${VCPKG_SHARE_DIR}/*")
        foreach(PORT IN LISTS INSTALLED_PORTS)
            if("${PORT}" STREQUAL "vcpkg" OR NOT IS_DIRECTORY "${VCPKG_SHARE_DIR}/${PORT}")
                continue()
            endif()

            set(COPYRIGHT_FILE "${VCPKG_SHARE_DIR}/${PORT}/copyright")
            if(EXISTS "${COPYRIGHT_FILE}")
                file(READ "${COPYRIGHT_FILE}" LICENSE_TEXT)
                # Normalize Windows-style line endings to standard Unix line feeds
                string(REPLACE "\r\n" "\n" LICENSE_TEXT "${LICENSE_TEXT}")
                string(APPEND MD_CONTENT "## ${PORT}\n\n```text\n${LICENSE_TEXT}\n```\n\n---\n\n")
            endif()
        endforeach()
    else()
        message(WARNING "vcpkg_installed directory not found at: ${VCPKG_SHARE_DIR}")
    endif()

    # 2. Process manual license files passed as optional arguments (ARGN)
    foreach(EXTRA_LICENSE_FILE IN LISTS ARGN)
        if(EXISTS "${EXTRA_LICENSE_FILE}")
            # Extract the dependency name using its parent directory name as a reference
            get_filename_component(PARENT_DIR "${EXTRA_LICENSE_FILE}" DIRECTORY)
            get_filename_component(LIB_NAME "${PARENT_DIR}" NAME)

            # Read file contents and normalize line endings before appending to the buffer
            file(READ "${EXTRA_LICENSE_FILE}" EXTRA_LICENSE_TEXT)
            string(REPLACE "\r\n" "\n" EXTRA_LICENSE_TEXT "${EXTRA_LICENSE_TEXT}")
            string(APPEND MD_CONTENT "## ${LIB_NAME}\n\n```text\n${EXTRA_LICENSE_TEXT}\n```\n\n---\n\n")
            
            message(STATUS "Manual license notice added for: ${LIB_NAME}")
        else()
            message(WARNING "Extra license file not found: ${EXTRA_LICENSE_FILE}")
        endif()
    endforeach()

    # Write the compiled markdown content to the final destination file
    set(NOTICES_FILE "${OUTPUT_PATH}/THIRD-PARTY-NOTICES.md")

    file(WRITE "${NOTICES_FILE}" "${MD_CONTENT}")
    message(STATUS "Successfully generated: ${NOTICES_FILE}")

    # Register the generated file for deployment into the standard system documentation directory
    install(FILES 
        "${NOTICES_FILE}"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endfunction()
