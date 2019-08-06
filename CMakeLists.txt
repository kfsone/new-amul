CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# 'THE' is the place-holder name, TBD

PROJECT(THE CXX)

SET(THE_LIB_NAME "thelib")
SET(THE_EXE_NAME "theexe")
FILE(GLOB THE_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

OPTION(THE_TESTS "Build and run unit tests" ON)
IF (NOT THE_TESTS)
    OPTION(THE_CLANG_TIDY "Use clang-tidy on New-AMUL source code" OFF)
    OPTION(THE_CLANG_TIDY_FIX "Use -fix-errors with clang-tidy" OFF)
ELSE ()
    SET(THE_CLANG_TIDY "OFF")
    SET(THE_CLANG_TIDY_FIX "OFF")
ENDIF ()
OPTION(THE_CPPCHECK "Use CPP Check to validate code (if available)" ON)

IF (MSVC)

	ADD_COMPILE_DEFINITIONS("_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE")

ELSE ()

    IF (CMAKE_C_COMPILER_ID MATCHES "[cC][lL][aA][nN][gG]")
        SET(CLANG true)
    ELSEIF (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        SET(GCC true)
    ENDIF ()

	STRING(APPEND THE_FLAGS_DEBUG " -O0 -g3")
	STRING(APPEND THE_FLAGS " -Wall -Wpedantic -Wextra -Wshadow -Wwrite-strings")
	OPTION(THE_SANITIZERS "Enable address/undefined sanitizers" ON)
	IF (THE_SANITIZERS)
		STRING(APPEND THE_FLAGS_DEBUG " -fsanitize=address -fsanitize=undefined")
		STRING(APPEND THE_FLAGS_DEBUG " -fno-omit-frame-pointer")
        IF (CLANG)
			STRING(APPEND THE_FLAGS_DEBUG " -fprofile-instr-generate -fcoverage-mapping")
        ENDIF ()
    ENDIF ()

	STRING(APPEND CMAKE_CXX_FLAGS_DEBUG " ${THE_FLAGS_DEBUG}")
	STRING(APPEND CMAKE_CXX_FLAGS " ${THE_FLAGS}")
ENDIF ()

IF (THE_CLANG_TIDY)
	IF (THE_CLANG_TIDY_FIX)
        MESSAGE(STATUS "Enabling clang-tidy -fix")
        SET(CLANG_TIDY_FIX_ERRORS -fix-errors)
    ELSE ()
        MESSAGE(STATUS "Enabling clang-tidy")
    ENDIF ()
    SET(CMAKE_CXX_CLANG_TIDY clang-tidy --checks= --format-style=file ${CLANG_TIDY_FIX_ERRORS} -p=${CMAKE_BINARY_DIR})
ENDIF()

IF (THE_CPPCHECK)
    FIND_PROGRAM(CPPCHECK_EXE NAMES cppcheck)
    IF (NOT CPPCHECK_EXE STREQUAL "CPPCHECK_EXE-NOTFOUND")
        MESSAGE(STATUS "CPPCheck Enabled")
        MESSAGE(WARNING "CppCheck <= 1.82 does not support C++17")
        SET(CPPCHECK_ARGS "--enable=all;--template={file}:{line}: {severity}: {id} {message} {callstack};--suppress=missingIncludeSystem;--quiet")
        SET(CMAKE_CXX_CPPCHECK "${CPPCHECK_EXE};${CPPCHECK_ARGS}")
    ENDIF()
ENDIF ()

ADD_SUBDIRECTORY(src)

IF (THE_TESTS AND NOT THE_CLANG_TIDY)
	MESSAGE(STATUS "Testing enabled")
	ENABLE_TESTING()
	ADD_SUBDIRECTORY(tests)
ENDIF()

# .PHONE : cov
# cov::
#   ninja clean
#   ninja -j8 tests
#   LLVM_PROFILE_FILE="tests.profraw" tests/tests
#   llvm-profdata-6.0 merge -sparse tests.profraw -o tests.profdata
#   llvm-cov-6.0 show -format=html tests/tests -instr-profile=tests.profdata >cov.html
#   llvm-cov-6.0 report tests/tests -instr-profile=tests.profdata

