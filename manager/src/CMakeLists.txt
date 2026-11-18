add_executable(WeaverManager WIN32
	main.cpp
	manager_logging.cpp
	manager_logging.h
	manager_paths.cpp
	manager_paths.h
	main_window.cpp
	main_window.h
)

if(WIN32)
	add_custom_command(TARGET WeaverManager POST_BUILD
		COMMAND "${WEAVER_WINDEPLOYQT_EXECUTABLE}"
				"$<TARGET_FILE:WeaverManager>"
				--dir "$<TARGET_FILE_DIR:WeaverManager>"
				--no-translations
		COMMENT "Deploying Qt runtime beside WeaverManager"
		VERBATIM
	)
endif()
