#include "unitc.h"
#include <assert.h>

bool test_function(bool run_assert)
{
    assert(!run_assert);
    return true;
}

void test_valid() // valid tests
{
    // Check logical
    TEST_TRUE(true);
    TEST_FALSE(false);

    // Check compare
    TEST_EQ(9, 9);  // ==
    TEST_NE(6, 9);  // !=
    TEST_LT(4, 5);  // <
    TEST_LE(5, 5);  // <=
    TEST_GT(9, 5);  // >
    TEST_GE(9, 9);  // >=

    // Check string
    TEST_STREQ("foo", "foo");
    TEST_STRNE("foo", "bar");
}

void test_invalid() // not valid tests
{
    // Check logical
    TEST_TRUE(0);
    TEST_FALSE(!false);

    // Check compare
    TEST_EQ(9, 0);      // ==
    TEST_NE(6, 3 + 3);  // !=
    TEST_LT(5, 5);      // <
    TEST_LE(9, 5);      // <=
    TEST_GT(4, 5);      // >
    TEST_GE(-1, 9);     // >=

    // Check string
    TEST_STREQ("", "foo");
    TEST_STRNE("foo", "foo");
}

void test_assert() // aborted test
{
    TEST_TRUE(test_function(false));  // normal
    TEST_TRUE(test_function(true));   // aborted
    TEST_TRUE(test_function(false));  // never be called
}

int main()
{
    run_suite(test_valid);      // valid tests
    run_suite(test_invalid);    // not valid tests
    run_suite(test_assert);     // aborted test

    return units_report();
}
