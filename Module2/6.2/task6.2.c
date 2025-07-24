#include "contact_api.h"
#include <stdio.h>

void refresh_disp()
{
    printf("\033[2J\033[H");
}

int main()
{
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
        printf("6. Выход\n");

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
                export_contacts("export.txt");
                break;
            case '6':
                return 0;
                break;
        }
    }

    return 0;
}
