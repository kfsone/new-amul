#ifndef THE_TESTS_GTEST_ALIASES_H
#define THE_TESTS_GTEST_ALIASES_H


// Test for "not an error" (error_t == 0) case.
#define EXPECT_SUCCESS(predicate) EXPECT_EQ(error_t(0), (predicate))

// Test for "is an error" (error_t != 0) case.
#define EXPECT_ERROR(err, predicate) EXPECT_EQ(error_t(err), (predicate))

// Test for a null pointer.
#define EXPECT_NULL(value) EXPECT_EQ(value, nullptr)

// Test for a not-null pointer.
#define EXPECT_NOT_NULL(value) EXPECT_NE(value, nullptr)


#endif  // THE_TESTS_GTEST_ALIASES_H
