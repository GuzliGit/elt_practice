#include "contact_list.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

// Тесты на добавление
START_TEST(test_add_contact) {
    Contact c1 = {"Man", "Man"};
    Contact c2 = {"", "Empty"};
    Contact c3 = {"Empty", ""};

    // Проверка добавления валидного контакта
    ck_assert_int_eq(add_contact(&c1), 1);
    ck_assert_int_eq(get_contacts_count(), 1);

    // Проверка на контакт без имени
    ck_assert_int_eq(add_contact(&c2), 0);
    ck_assert_int_eq(get_contacts_count(), 1);

    // Проверка на контакт без фамилии
    ck_assert_int_eq(add_contact(&c3), 0);
    ck_assert_int_eq(get_contacts_count(), 1);
}
END_TEST

// Тесты на удаление
START_TEST(test_remove_contact) {
    Contact c1 = {"Last", "First", "Patr"};
    add_contact(&c1);

    // Удаление существующего контакта
    ck_assert_int_eq(remove_contact(0), 1);
    ck_assert_int_eq(get_contacts_count(), 0);

    // Удаление несуществующего контакта
    ck_assert_int_eq(remove_contact(999), 0);
}
END_TEST

// Тесты для get_contact_by_id
START_TEST(test_get_contact_by_id) {
    Contact c1 = {"Ivan", "Ivanov"};
    strcpy(c1.w_info.post, "Developer");
    c1.emails_count = 1;
    strcpy(c1.email[0], "ivanov@gmail.com");
    add_contact(&c1);

    Contact *retrieved = get_contact_by_id(0);
    ck_assert_str_eq(retrieved->l_name, "Ivan");
    ck_assert_str_eq(retrieved->f_name, "Ivanov");
    ck_assert_str_eq(retrieved->w_info.post, "Developer");
    ck_assert_int_eq(retrieved->emails_count, 1);
    ck_assert_str_eq(retrieved->email[0], "ivanov@gmail.com");
}
END_TEST

Suite *contact_list_suite(void) 
{
    Suite *s = suite_create("Contact List");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_add_contact);
    tcase_add_test(tc_core, test_remove_contact);
    tcase_add_test(tc_core, test_get_contact_by_id);
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
