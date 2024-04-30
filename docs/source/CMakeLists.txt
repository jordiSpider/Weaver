
if(BUILD_DOC_VERBOSE)
	set(DOXYGEN_QUIET_BUILD "NO")
	set(SPHINX_QUIET_BUILD "OFF")
else()
	set(DOXYGEN_QUIET_BUILD "YES")
	set(SPHINX_QUIET_BUILD "ON")
endif()


add_subdirectory(doxygen)

# Temporary: Creation of the api documentation
if(BUILD_SPHINX_DOC OR BUILD_DOXYGEN_DOC)
    set(API_DOC_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/api")

    function(create_folder_index elem doc_path)
        set(file "${API_DOC_DIRECTORY}${doc_path}/${elem}_index.rst")

        file(WRITE ${file} "")

        string(REPLACE "/" "-" elem_path "${doc_path}/${elem}")

        file(APPEND ${file} ".. _${elem_path}_index:\n")
        file(APPEND ${file} "\n")

        file(APPEND ${file} "${elem}\n")
        string(LENGTH "${elem}" elem_length)
        foreach(i RANGE ${elem_length})
            file(APPEND ${file} "=")
        endforeach()
        file(APPEND ${file} "\n")
        file(APPEND ${file} "\n")

        file(APPEND ${file} ".. toctree::\n")
        file(APPEND ${file} "   :glob:\n")
        file(APPEND ${file} "\n")
        file(APPEND ${file} "   ${elem}/*\n")
    endfunction()

    function(create_file_index elem doc_path)
        get_filename_component(elem_without_ext ${elem} NAME_WE)

        set(file "${API_DOC_DIRECTORY}${doc_path}/${elem_without_ext}.rst")

        file(WRITE ${file} "")
        
        string(REPLACE "/" "-" elem_path "${doc_path}/${elem_without_ext}")

        file(APPEND ${file} "..  _${elem_path}:\n")
        file(APPEND ${file} "\n")

        file(APPEND ${file} "${elem_without_ext}\n")
        string(LENGTH "${elem_without_ext}" elem_length)
        foreach(i RANGE ${elem_length})
            file(APPEND ${file} "=")
        endforeach()
        file(APPEND ${file} "\n")
        file(APPEND ${file} "\n")

        file(APPEND ${file} ".. doxygenfile:: ${elem}\n")
        file(APPEND ${file} "   :project: ${CMAKE_PROJECT_NAME}\n")
    endfunction()

    function(folder_doc source_code_path doc_path)
        file(GLOB folder_elems "${INCLUDE_DIRECTORY_PATH}/${source_code_path}/*")

        foreach(elem IN LISTS folder_elems)
            get_filename_component(elem_name ${elem} NAME)

            if(IS_DIRECTORY ${elem})
                create_folder_index(${elem_name} "${doc_path}")

                file(MAKE_DIRECTORY "${API_DOC_DIRECTORY}${doc_path}/${elem_name}")

                folder_doc("${source_code_path}/${elem_name}" "${doc_path}/${elem_name}")
            else()
                create_file_index(${elem_name} "${doc_path}")
            endif()
        endforeach()
    endfunction()

    folder_doc("" "")
endif()


if(BUILD_SPHINX_DOC OR BUILD_SPHINX_CONF OR UPDATE_SPHINX_LOCALE OR CLEAN_DOC)
    # Set documentation build output directory
    set(SPHINX_OUTPUT_DIRECTORY "${SPHINX_BUILD_DIRECTORY}/html")

    if(BUILD_SPHINX_DOC OR BUILD_SPHINX_CONF OR UPDATE_SPHINX_LOCALE)
        set(ALL_LANGUAGES "en")
        set(MAIN_LANGUAGE "en")

        if (NOT MAIN_LANGUAGE IN_LIST ALL_LANGUAGES)
            string(JOIN "," formatted_list ${ALL_LANGUAGES})
            message(FATAL_ERROR "The main language is not in the available languages: [${formatted_list}]")
        endif()

        string(TIMESTAMP COPYRIGHT_YEAR "%Y" UTC)

        set(SPHINX_LOCALE_DIRECTORY "locale")

        set(SPHINX_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

        set(SPHINX_CONF_FILE_TEMPLATE "${SPHINX_SOURCE_DIRECTORY}/conf.py.in")
        set(SPHINX_CONF_FILE "${SPHINX_SOURCE_DIRECTORY}/conf.py")

        configure_file(${SPHINX_CONF_FILE_TEMPLATE} ${SPHINX_CONF_FILE} @ONLY)

        if(BUILD_SPHINX_DOC OR UPDATE_SPHINX_LOCALE)
            set(SPHINX_VERSION 4.2.0)
        
            find_program(
                SPHINX_EXECUTABLE
                NAMES sphinx-build sphinx-build.exe
            )
            mark_as_advanced(SPHINX_EXECUTABLE)

            if(SPHINX_EXECUTABLE)
                execute_process(
                    COMMAND ${SPHINX_EXECUTABLE} --version
                    RESULT_VARIABLE EXIT_CODE
                    OUTPUT_VARIABLE CURRENT_SPHINX_VERSION
                )
            endif()

            string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" VERSION_MATCH "${CURRENT_SPHINX_VERSION}")
            set(CURRENT_SPHINX_VERSION ${CMAKE_MATCH_1})

            if(EXIT_CODE EQUAL 0)
                if(CURRENT_SPHINX_VERSION VERSION_LESS SPHINX_VERSION)
                    set(SPHINX_EXECUTABLE "OFF")
                endif()
            else()
                set(SPHINX_EXECUTABLE "OFF")
            endif()

            if(NOT SPHINX_EXECUTABLE)
                message(FATAL_ERROR "Sphinx (v${SPHINX_VERSION}+) need to be installed to generate the documentation")
            else()
                message(STATUS "Found Sphinx: ${SPHINX_EXECUTABLE} (found suitable version \"${CURRENT_SPHINX_VERSION}\", minimum required is \"${SPHINX_VERSION}\")")
            endif()


            if(UPDATE_SPHINX_LOCALE)
                add_custom_target(
                    get_all_text_doc ALL
                    COMMAND make gettext
                    WORKING_DIRECTORY ${DOCS_DIRECTORY}
                    COMMENT "Getting all the text of the documentation"
                )

                set(LANGUAGES_TO_UPDATE ${ALL_LANGUAGES})
                list(REMOVE_ITEM LANGUAGES_TO_UPDATE ${MAIN_LANGUAGE})

                foreach(language IN LISTS LANGUAGES_TO_UPDATE)
                    add_custom_target(
                        update_language_${language} ALL
                        COMMAND sphinx-intl -c ${SPHINX_CONF_FILE} update -p ${SPHINX_BUILD_DIRECTORY}/gettext -l ${language}
                        DEPENDS get_all_text_doc
                        WORKING_DIRECTORY ${DOCS_DIRECTORY}
                        COMMENT "Updating language '${language}' texts"
                    )
                endforeach()

                file(GLOB CURRENT_LOCALE_FOLDERS "${SPHINX_LOCALE_DIRECTORY}/*")

                set(CURRENT_LANGUAGES)

                foreach(language_folder IN LISTS CURRENT_LOCALE_FOLDERS)
                    get_filename_component(language ${language_folder} NAME)
                    list(APPEND CURRENT_LANGUAGES ${language})
                endforeach()

                set(LANGUAGES_TO_REMOVE ${CURRENT_LANGUAGES})

                foreach(language IN LISTS LANGUAGES_TO_UPDATE)
                    list(REMOVE_ITEM LANGUAGES_TO_REMOVE ${language})
                endforeach()

                foreach(language IN LISTS LANGUAGES_TO_REMOVE)
                    file(REMOVE_RECURSE ${SPHINX_LOCALE_DIRECTORY}/${language})
                endforeach()
            endif()

            if(BUILD_SPHINX_DOC)
                set(VERSION "latest")

                file(WRITE versions.yaml "\"${VERSION}\":\n  languages:\n")

                foreach(language IN LISTS ALL_LANGUAGES)
                    file(APPEND versions.yaml "    - \"${language}\"\n")
                endforeach()

                foreach(language IN LISTS ALL_LANGUAGES)
                    if (language STREQUAL MAIN_LANGUAGE)
                        set(LANGUAGE_OUTPUT_DIRECTORY "${SPHINX_OUTPUT_DIRECTORY}/${VERSION}")
                    else()
                        set(LANGUAGE_OUTPUT_DIRECTORY "${SPHINX_OUTPUT_DIRECTORY}/${VERSION}/${language}")
                    endif()

                    if(SPHINX_QUIET_BUILD)
                        add_custom_target(
                            build_doc_${language}_quiet ALL
                            COMMAND ${SPHINX_EXECUTABLE} -b html -j 1 -q -D language='${language}' -D version='${VERSION}' -D html_baseurl='${SPHINX_OUTPUT_DIRECTORY}' ${SPHINX_SOURCE_DIRECTORY} ${LANGUAGE_OUTPUT_DIRECTORY}
                            DEPENDS build_doxygen_doc
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            COMMENT "Generating documentation '${language}' with Sphinx in quiet mode"
                        )
                    else()
                        add_custom_target(
                            build_doc_${language} ALL
                            COMMAND ${SPHINX_EXECUTABLE} -b html -j 1 -D language='${language}' -D version='${VERSION}' -D html_baseurl='${SPHINX_OUTPUT_DIRECTORY}' ${SPHINX_SOURCE_DIRECTORY} ${LANGUAGE_OUTPUT_DIRECTORY}
                            DEPENDS build_doxygen_doc
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            COMMENT "Generating documentation '${language}' with Sphinx"
                        )
                    endif()
                endforeach()
            endif()
        endif()
    endif()

    if(CLEAN_DOC)
        file(REMOVE_RECURSE ${SPHINX_OUTPUT_DIRECTORY})
    endif()
endif()
