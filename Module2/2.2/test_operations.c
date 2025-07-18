#include "operations.h"
#include <check.h>
#include <stdlib.h>

// Тесты на сложение
START_TEST(test_sum)
{
    ck_assert_double_eq_tol(sum(2, 15.0, 20.0), 35, 1e-6);
    ck_assert_double_eq_tol(sum(2, 66.22, 0.44), 66.66, 1e-6);
    ck_assert_double_eq_tol(sum(2, -15.23, -10.0), -25.23, 1e-6);
}
END_TEST

// Тесты на вычитание
START_TEST(test_substraction)
{
    ck_assert_double_eq_tol(substraction(45, 15), 30, 1e-6);
    ck_assert_double_eq_tol(substraction(33.11, 11.02), 22.09, 1e-6);
    ck_assert_double_eq_tol(substraction(56.2, 100.05), -43.85, 1e-6);
}
END_TEST

// Тесты на умножение
START_TEST(test_multiplication)
{
    ck_assert_double_eq_tol(multiplication(20, 4), 80, 1e-6);
    ck_assert_double_eq_tol(multiplication(6.4, 3), 19.2, 1e-6);
    ck_assert_double_eq_tol(multiplication(-3.3, 15.66), -51.678, 1e-6);
}
END_TEST

// Тесты на деление
START_TEST(test_division)
{
    double result;
    ck_assert_int_eq(division(5, 2, &result), 1);
    ck_assert_double_eq_tol(result, 2.5, 1e-6);

    ck_assert_int_eq(division(-66.6, -123.41, &result), 1);
    ck_assert_double_eq_tol(result, 0.539665, 1e-6);

    ck_assert_int_eq(division(-66.6, 0, &result), 0);
    ck_assert_double_eq_tol(result, 0.539665, 1e-6);
}

Suite *contact_list_suite(void) 
{
    Suite *s = suite_create("Contact List");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_sum);
    tcase_add_test(tc_core, test_substraction);
    tcase_add_test(tc_core, test_multiplication);
    tcase_add_test(tc_core, test_division);
    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
    int failed_count = 0;
    Suite *s = contact_list_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    failed_count = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}