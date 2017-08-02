#pragma once

#ifndef _UNITC_H_
#define _UNITC_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <setjmp.h>

#ifdef NDEBUG
    #warning if you yse NDEBUG define, abort() diagnostic may be not working
#endif

#define MAX_STRLEN 128
#define _CONSOLE_COLOR_RED      "\033[1;31m"
#define _CONSOLE_COLOR_GREEN    "\033[1;32m"
#define _CONSOLE_COLOR_RESET    "\033[0m"

static struct
{
    jmp_buf sigabrt_jmp_buf;
    struct
    {
        int runs;
        int passes;
        int fails;
        int aborted;
    } statistics;
    struct
    {
        int number_in_suite;
        char test_text[MAX_STRLEN];
        char error_reason[MAX_STRLEN];
    } test;
} _unitc = {0};

static void _test_log(uint line, bool check_state)
{
    _unitc.test.number_in_suite++;
    _unitc.statistics.runs++;

    if (check_state)
    {
        _unitc.statistics.passes++;
        printf("%s + %s", _CONSOLE_COLOR_GREEN, _CONSOLE_COLOR_RESET);
    }
    else
    {
        _unitc. statistics.fails++;
        printf("%s - %s", _CONSOLE_COLOR_RED, _CONSOLE_COLOR_RESET);
    }

    printf("test %2d  %s\r\n", _unitc.test.number_in_suite, _unitc.test.test_text);

    if (!check_state)
    {
        printf("\tline %d, cause: %s\r\n", line, _unitc.test.error_reason);
    }

    fflush(stdout);
}

#define TEST_TRUE(expected) do {                                                                  \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_TRUE(%s)", #expected);                 \
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "is not true");                             \
        _test_log(__LINE__, (expected) == true);                                                    \
    } while(0)

#define TEST_FALSE(expected) do {                                                                 \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_FALSE(%s)", #expected);  \
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "is not false");                         \
        _test_log(__LINE__, (expected) == false);                                                           \
    } while(0)

#define TEST_EQ(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_EQ(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d == %d)", expected, result);\
        _test_log(__LINE__, (expected) == (result));                                                                \
    } while(0)

#define TEST_NE(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_NE(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d <> %d)", expected, result);\
        _test_log(__LINE__, (expected) != (result));                                                                \
    } while(0)

#define TEST_LT(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_LT(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d < %d", expected, result);\
        _test_log(__LINE__, (expected) < (result));                                                                 \
    } while(0)

#define TEST_LE(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_LE(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d <= %d", expected, result);\
        _test_log(__LINE__, (expected) <= (result));                                                                \
    } while(0)

#define TEST_GT(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_GT(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d > %d", expected, result);\
        _test_log(__LINE__, (expected) > (result));                                                                 \
    } while(0)

#define TEST_GE(expected, result) do {                                                                    \
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_GE(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "failed %d >= %d", expected, result);\
        _test_log(__LINE__, (expected) >= (result));                                                                \
    } while(0)

#define TEST_STREQ(expected, result) do {                                                                    \
        const char* str1 = (expected) ? (expected) : "_NULL_";\
        const char* str2 = (result) ? (result) : "_NULL_";\
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_GE(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "value1==\"%s\" and value2==\"%s\"", str1, str2);\
        _test_log(__LINE__, strcmp(str1, str2) == 0);                                                                \
    } while(0)

#define TEST_STRNE(expected, result) do {                                                                    \
        const char* str1 = (expected) ? (expected) : "_NULL_";\
        const char* str2 = (result) ? (result) : "_NULL_";\
        snprintf (_unitc.test.test_text, MAX_STRLEN, "TEST_GE(%s, %s)", #expected, #result);\
        snprintf (_unitc.test.error_reason, MAX_STRLEN, "value1==\"%s\" and value2==\"%s\"", str1, str2);\
        _test_log(__LINE__, strcmp(str1, str2) != 0);                                                             \
    } while(0)

static void _sigabrt_trap(int dummy)
{
    longjmp(_unitc.sigabrt_jmp_buf, dummy);
}

#define run_suite(test_function)                                                                    \
    do {                                                                                            \
        printf("\r\nRun suite %s:\r\n", #test_function);                                            \
        fflush(stdout);                                                                             \
        _unitc.test.number_in_suite = 0;                                                            \
        signal(SIGABRT, _sigabrt_trap);                                                             \
        if (!setjmp(_unitc.sigabrt_jmp_buf))    /* set abort() trap for assert()    */              \
            test_function();                    /* run test                         */              \
        else {                                  /* trap worked                      */              \
            printf("%s - %s", _CONSOLE_COLOR_RED, _CONSOLE_COLOR_RESET);                            \
            printf("Suite aborted\r\n");                                                            \
            _unitc.statistics.runs++;                                                               \
            _unitc.statistics.aborted++;                                                            \
        }                                                                                           \
    } while(0)

static int units_report()
{
    printf("\r\nRuns: %d ", _unitc.statistics.runs);
    printf("Passes: %d ", _unitc. statistics.passes);
    printf("Fails: %d ", _unitc.statistics.fails);
    printf("Aborted: %d ", _unitc.statistics.aborted);
    printf("Status: ");

    if (_unitc.statistics.fails)
    {
        printf("%sFAIL%s\r\n\n", _CONSOLE_COLOR_RED, _CONSOLE_COLOR_RESET);
        return EXIT_FAILURE;
    }
    else
    {
        printf("%sALL TESTS PASSED%s\r\n\n", _CONSOLE_COLOR_GREEN, _CONSOLE_COLOR_RESET);
        return EXIT_SUCCESS;
    }
}

#pragma GCC diagnostic pop

#endif // _UNITC_H_
