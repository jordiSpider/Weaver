set(WHITESPACE " \t\r\n")

set(CLASS_PATH_SRC "${SRC_DIRECTORY}/([A-Za-z0-9/_]+)\.cpp")

set(CLASS_PATH_INCLUDE "${INCLUDE_DIRECTORY}/([A-Za-z0-9/_]+)\.${INCLUDE_EXTENSION}")

set(CLASS_NAME "[^/]+$")

set(INCLUDE_PATTERN "#include[${WHITESPACE}]+\"([A-Za-z0-9_/]+)\.${INCLUDE_EXTENSION}\"")

set(EXTERNAL_INCLUDE_PATTERN "#include[${WHITESPACE}]+<([A-Za-z0-9_/]+)\.(h|hpp)>")

set(TRAIT "[A-Za-z0-9_]+")

set(LINE_BREAK "[${WHITESPACE}]+")

set(ENUM_TRAIT_PATTERN "enum${LINE_BREAK}TraitType${LINE_BREAK}{${LINE_BREAK}(${TRAIT}(,${LINE_BREAK}${TRAIT})*)[${LINE_BREAK}}")

set(CPP_FILE_PATTERN "${SRC_DIRECTORY}/*.cpp")

set(PARENT_PATH ".*/([^/]+)/[^/]+$")

set(TARGET_PATH "${INCLUDE_DIRECTORY}/([A-Za-z0-9/_]+)/[A-Za-z0-9/_]+\.${INCLUDE_EXTENSION}")