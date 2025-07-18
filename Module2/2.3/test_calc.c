#include "calc.h"
#include <check.h>
#include <math.h>
#include <stdlib.h>

START_TEST(test_calc)
{
    char error[256] = {0};
    // Проверка корректных выражений
    ck_assert_double_eq_tol(calculate("2*(2+4)", error), 12, 1e-6);
    ck_assert_double_eq_tol(calculate("2 + (-15 * 4)", error), -58, 1e-6);
    ck_assert_double_eq_tol(calculate("15 / 5 + 23 * 4", error), 95, 1e-6);
    ck_assert_double_eq_tol(calculate("54.4 / (4 * 12)", error), 1.13, 1e-2);
    // Проверка выражений с ошибками
    double result = calculate("1.2.3+4", error);
    ck_assert(isnan(result));
    ck_assert_str_eq(error, "Invalid num: 1.2.3");
    
    result = calculate("4/0", error);
    ck_assert(isnan(result));
    ck_assert_str_eq(error, "Division by zero!");

    result = calculate("4/4/", error);
    ck_assert(isnan(result));
    ck_assert_str_eq(error, "Not enough operands!");
}
END_TEST

Suite *contact_list_suite(void) 
{
    Suite *s = suite_create("Contact List");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_calc);
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