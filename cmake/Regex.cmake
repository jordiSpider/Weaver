set(WHITESPACE " \t\r\n")

set(SLASH "/\\")

set(WORD "A-Za-z0-9_")

set(INCLUDE_PATTERN "#include[${WHITESPACE}]+\"([${WORD}/]+)\.h\"")

set(EXTERNAL_INCLUDE_PATTERN "#include[${WHITESPACE}]+<([${WORD}/]+)\.(h|hpp)>")

set(TRAIT "[${WORD}]+")

set(LINE_BREAK "[${WHITESPACE}]+")

set(ENUM_TRAIT_PATTERN "enum${LINE_BREAK}TraitType${LINE_BREAK}{${LINE_BREAK}(${TRAIT}(,${LINE_BREAK}${TRAIT})*)[${LINE_BREAK}}")

set(CPP_FILE_PATTERN "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/[${WORD}]*.cpp")