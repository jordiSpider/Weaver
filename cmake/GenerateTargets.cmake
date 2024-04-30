include("${PROJECT_SOURCE_DIR}/cmake/Regex.cmake")

include("${PROJECT_SOURCE_DIR}/cmake/ExternalLibraries.cmake")


set(CMAKE_OBJECT_POSTFIX "_o")


if(NOT DEFINED TARGETS_PATHS)
    message(FATAL_ERROR "The variable 'TARGETS_PATHS', which contains a list of paths, must be defined.")
endif()

while(TARGETS_PATHS)
    # Extract a path
    list(GET TARGETS_PATHS 0 file_path)
    list(REMOVE_AT TARGETS_PATHS 0)
    
    if(${file_path} MATCHES "\\.${SRC_EXTENSION}$")
        string(REGEX MATCH ${CLASS_PATH_SRC} matchedPart "${file_path}")
        set(executablePath ${CMAKE_MATCH_1})

        string(REGEX MATCH ${CLASS_NAME} matchedPart "${executablePath}")
        set(executableName ${CMAKE_MATCH_0})

        if(RELEASE_MODE)
            set(executableTarget ${executableName})
        elseif(DEBUG_MODE)
            set(executableTarget ${executableName}${CMAKE_DEBUG_POSTFIX})
        endif()

        list(APPEND EXECUTABLES ${executableTarget})
        list(APPEND COMPILATION_TARGETS ${executableTarget})

        list(APPEND ${executableTarget}_files ${file_path})

        
        file(READ "${file_path}" executableContent)

        # Search for all includes in the content
        string(REGEX MATCHALL ${INCLUDE_PATTERN} matches "${executableContent}")

        foreach(match ${matches})
            string(REGEX MATCH ${INCLUDE_PATTERN} matchedPart "${match}")
            set(classPath ${CMAKE_MATCH_1})

            set(src_file "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${classPath}.${SRC_EXTENSION}")
            if(EXISTS ${src_file})
                set(include_file "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${classPath}.${INCLUDE_EXTENSION}")
                list(APPEND TARGETS_PATHS ${include_file})
            
                string(REGEX MATCH ${TARGET_PATH} matchedPart ${include_file})
                set(matchTarget ${CMAKE_MATCH_1})
                string(REPLACE "/" ${UNION} refactorMatchTarget ${matchTarget})

                list(APPEND ${executableTarget}_depen ${refactorMatchTarget})
            endif()
        endforeach()

        # Search for all external includes in the content
        string(REGEX MATCHALL ${EXTERNAL_INCLUDE_PATTERN} matches "${executableContent}")
        
        foreach(match ${matches})
            string(REGEX MATCH ${EXTERNAL_INCLUDE_PATTERN} matchedPart "${match}")
            set(class ${CMAKE_MATCH_1})

            list(FIND EXTERNAL_CLASSES ${class} index)
            if(NOT index EQUAL -1)
                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})
                list(APPEND ${executableTarget}_depen ${MAP_DEPEN_${refactorClass}})
            endif()
        endforeach()

        list(REMOVE_DUPLICATES ${executableTarget}_depen)
    elseif(${file_path} MATCHES "\\.${INCLUDE_EXTENSION}$")
        string(REGEX MATCH ${TARGET_PATH} matchedPart "${file_path}")
        set(target ${CMAKE_MATCH_1})
        string(REPLACE "/" ${UNION} refactorTarget ${target})

        if(NOT refactorTarget IN_LIST COMPILATION_TARGETS)
            file(GLOB target_src_files "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${target}/*\.${SRC_EXTENSION}")

            foreach(src_file ${target_src_files})
                string(REGEX MATCH ${CLASS_PATH_SRC} matchedPart "${src_file}")
                set(classPath ${CMAKE_MATCH_1})

                set(include_file "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${classPath}.${INCLUDE_EXTENSION}")
                if(EXISTS ${include_file})
                    list(APPEND ${refactorTarget}_files ${include_file} ${src_file})

                    file(READ "${include_file}" elemContent)

                    # Search for all includes in the content
                    string(REGEX MATCHALL ${INCLUDE_PATTERN} matches "${elemContent}")

                    foreach(match ${matches})
                        string(REGEX MATCH ${INCLUDE_PATTERN} matchedPart "${match}")
                        set(classPath ${CMAKE_MATCH_1})

                        set(src_file "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${classPath}.${SRC_EXTENSION}")
                        if(EXISTS ${src_file})
                            set(include_file "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${classPath}.${INCLUDE_EXTENSION}")
                            list(APPEND TARGETS_PATHS ${include_file})
                        
                            string(REGEX MATCH ${TARGET_PATH} matchedPart ${include_file})
                            set(matchTarget ${CMAKE_MATCH_1})
                            string(REPLACE "/" ${UNION} refactorMatchTarget ${matchTarget})

                            list(APPEND ${refactorTarget}_depen ${refactorMatchTarget})
                        endif()
                    endforeach()

                    # Search for all external includes in the content
                    string(REGEX MATCHALL ${EXTERNAL_INCLUDE_PATTERN} matches "${elemContent}")
                    
                    foreach(match ${matches})
                        string(REGEX MATCH ${EXTERNAL_INCLUDE_PATTERN} matchedPart "${match}")
                        set(class ${CMAKE_MATCH_1})

                        list(FIND EXTERNAL_CLASSES ${class} index)
                        if(NOT index EQUAL -1)
                            string(REPLACE "/" ${PATH_SEP} refactorClass ${class})
                            list(APPEND ${refactorTarget}_depen ${MAP_DEPEN_${refactorClass}})
                        endif()
                    endforeach()

                    list(REMOVE_DUPLICATES ${refactorTarget}_depen)

                    list(REMOVE_ITEM ${refactorTarget}_depen ${refactorTarget})

                    list(APPEND COMPILATION_TARGETS ${refactorTarget})
                endif()
            endforeach()
        endif()
    endif()
endwhile()

list(REMOVE_DUPLICATES COMPILATION_TARGETS)
