INCLUDE(GoogleTest)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

SET(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use shared crt" FORCE)
ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/external/googletest" "googletest")

MARK_AS_ADVANCED(
	BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
	gmock_build_tests gtest_build_samples gtest_build_tests
	gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

MACRO(add_gtest TESTNAME)
	ADD_EXECUTABLE(${TESTNAME} ${ARGN})
	TARGET_LINK_LIBRARIES(${TESTNAME} PRIVATE gtest gmock gtest_main ${THE_LIB_NAME})
	ADD_TEST(NAME ${TESTNAME} COMMAND ${TESTNAME})
	SET_TARGET_PROPERTIES(${TESTNAME} PROPERTIES FOLDER tests/googletests)
ENDMACRO()

add_gtest(AtomTest			test_atom.cpp	  	../src/atom.h ../src/buffer.h)
add_gtest(BufferTest		test_buffer.cpp    	../src/buffer.h)
#add_gtest(FilesystemTest	test_filesystem.cpp	../src/filesystem.h)
#add_gtest(ModuleTest 		test_module.cpp     	../src/modules.h)
#add_gtest(MsgPortTest 		test_msgport.cpp    	../src/system.h ../src/msgports.h)
#add_gtest(SpinLockTest 	test_spinlock.cpp   	../src/spinlock.h ../src/spinlock.h)

