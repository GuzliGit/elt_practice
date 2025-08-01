#include "contact_api.h"
#include "contact_list.h"
#include <stdio.h>
#include <stdlib.h>

#define FILENAME "contacts.txt"

void refresh_disp()
{
    printf("\033[2J\033[H");
}

int main()
{
    init_exec_dir();
    write_contacts_from_file(FILENAME);
    char input;

    while (1)
    {
        refresh_disp();
        printf("Телефонная книга:\n");
        printf("1. Добавить новый контакт\n");
        printf("2. Редактировать контакт\n");
        printf("3. Удалить контакт\n");
        printf("4. Список контактов\n");
        printf("5. Выгрузить текущие контакты в текстовый файл\n");
        printf("6. Вывод бинарного дерева\n");
        printf("7. Выход\n");

        input = getchar();
        if (input == EOF) 
        {
            clearerr(stdin);
            input = '\0';
            continue;
        }
        while(getchar() != '\n');

        switch(input)
        {
            case '1':
                write_contact();
                break;
            case '2':
                edit_contact();
                break;
            case '3':
                delete_contact();
                break;
            case '4':
                print_contacts();
                printf("Для продолжения нажмите Enter...\n");
                while(getchar() != '\n');
                break;
            case '5':
                export_contacts(FILENAME);
                printf("Для продолжения нажмите Enter...\n");
                while(getchar() != '\n');
                break;
            case '6':
                visualize_full_tree();
                printf("Для продолжения нажмите Enter...\n");
                while(getchar() != '\n');
                break;
            case '7':
                return EXIT_SUCCESS;
                break;
        }
    }

    return EXIT_SUCCESS;
}
