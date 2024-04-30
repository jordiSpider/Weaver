
if(BUILD_R_PACKAGES)
    execute_process(COMMAND which R OUTPUT_VARIABLE R_EXECUTABLE OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Verificar si se encontró el ejecutable de R
    if(NOT R_EXECUTABLE)
        message(FATAL_ERROR "R must be installed to use this preset.")
    endif()


    
    set(PACKAGES_TO_BUILD "")

    if(BUILD_OPTIMISER_PACKAGE)
        list(APPEND PACKAGES_TO_BUILD "weaverOptimiser")
    endif()

    if(BUILD_WSL_OPTIMISER_PACKAGE)
        list(APPEND PACKAGES_TO_BUILD "weaverWSLOptimiser")
    endif()
    

    foreach(package_name ${PACKAGES_TO_BUILD})
        add_subdirectory(${package_name})

        set(DESCRIPTION_FILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/${package_name}/DESCRIPTION.in")
        set(DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${package_name}/DESCRIPTION")

        configure_file(${DESCRIPTION_FILE_TEMPLATE} ${DESCRIPTION_FILE} @ONLY)
        

        add_custom_target(build_R_package_${package_name}
            COMMAND ${R_EXECUTABLE} "CMD" "build" "${package_name}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Building '${package_name}' R package"
        )


        add_custom_target(install_R_package_${package_name}
            COMMAND ${R_EXECUTABLE} "CMD" "INSTALL" "${package_name}_${PROJECT_VERSION}.tar.gz"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Installing '${package_name}' R package"
        )

        add_dependencies(install_R_package_${package_name} build_R_package_${package_name})
        add_dependencies(main_target install_R_package_${package_name})
    endforeach()
endif()
