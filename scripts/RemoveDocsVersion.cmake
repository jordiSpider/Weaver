execute_process(
	COMMAND "${PYTHON_EXECUTABLE}" "${REMOVE_SCRIPT}"
		--product "${PRODUCT}"
		--version "${VERSION}"
		--repository "${REPOSITORY}"
		--api-url "${API_URL}"
		--source-root "${SOURCE_ROOT}"
		--output-root "${OUTPUT_ROOT}"
		--catalog "${CATALOG}"
	RESULT_VARIABLE result
	OUTPUT_VARIABLE output
	ERROR_VARIABLE error
)

if(output)
	message(STATUS "${output}")
endif()
if(NOT result EQUAL 0)
	message(FATAL_ERROR "${error}")
endif()
