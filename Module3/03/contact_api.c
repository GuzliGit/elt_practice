#include "contact_api.h"
#include "contact_list.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static char exec_dir[PATH_MAX];

void init_exec_dir()
{
    if (readlink("/proc/self/exe", exec_dir, sizeof(exec_dir)) == -1) 
    {
        strcpy(exec_dir, ".");
    } 
    else 
    {
        char* last_slash = strrchr(exec_dir, '/');
        if (last_slash)
        {
            *last_slash = '\0';
        }
    }
}

int set_l_name(Contact* contact)
{
    contact->l_name[0] = '\0';

    while (1)
    {
        printf("Фамилия (обязательно): ");
        if (fgets(contact->l_name, LF_NAME_PATRONYMIC_LEN, stdin) == NULL)
        {
            clearerr(stdin);
            return 0;
        }

        contact->l_name[strcspn(contact->l_name, "\n")] = '\0';

        if (strlen(contact->l_name) > 0) break;
        printf("Данное поле обязательно к заполнению!\n");
    }

    return 1;
}

int set_f_name(Contact* contact)
{
    while (1)
    {
        printf("Имя (обязательно): ");
        if (fgets(contact->f_name, LF_NAME_PATRONYMIC_LEN, stdin) == NULL)
        {
            clearerr(stdin);
            return 0;
        }

        contact->f_name[strcspn(contact->f_name, "\n")] = '\0';

        if (strlen(contact->f_name) > 0) break;
        printf("Данное поле обязательно к заполнению!\n");
    }

    return 1;
}

int set_patronymic(Contact* contact)
{
    printf("Введите отчество (Enter для пропуска): ");
    if ((fgets(contact->patronymic, LF_NAME_PATRONYMIC_LEN, stdin) != NULL) && strlen(contact->patronymic) > 0)
    {
        contact->patronymic[strcspn(contact->patronymic, "\n")] = '\0';
    }
    else 
    {
        contact->patronymic[0] = '\0';
    }

    return 1;
}

int set_w_name(Contact* contact)
{
    printf("Введите название места работы (Enter для пропуска): ");
    if ((fgets(contact->w_info.name, WORK_NAME_LEN, stdin) != NULL) && strlen(contact->w_info.name) > 0)
    {
        contact->w_info.name[strcspn(contact->w_info.name, "\n")] = '\0';
    }
    else 
    {
        contact->w_info.name[0] = '\0';
    }

    return 1;
}

int set_w_address(Contact* contact)
{
    printf("Введите адрес места работы (Enter для пропуска): ");
    if ((fgets(contact->w_info.address, WORK_ADDRESS_LEN, stdin) != NULL) && strlen(contact->w_info.address) > 0)
    {
        contact->w_info.address[strcspn(contact->w_info.address, "\n")] = '\0';
    }
    else 
    {
        contact->w_info.address[0] = '\0';
    }

    return 1;
}

int set_w_post(Contact* contact)
{
    printf("Введите должность (Enter для пропуска): ");
    if ((fgets(contact->w_info.post, WORK_POST_LEN, stdin) != NULL) && strlen(contact->w_info.post) > 0)
    {
        contact->w_info.post[strcspn(contact->w_info.post, "\n")] = '\0';
    }
    else 
    {
        contact->w_info.post[0] = '\0';
    }

    return 1;
}

int set_emails(Contact* contact)
{   
    char input[10];

    printf("Введите кол-во email адресов для ввода (0-%d): ", MAX_MAILS);
    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        int email_count = atoi(input);
        email_count = email_count > 0 ? (email_count > MAX_MAILS ? MAX_MAILS : email_count) : 0;

        for (int i = 0; i < email_count; i++)
        {
            printf("%d) ", i + 1);
            if (fgets(contact->email[i], EMAIL_LEN, stdin) != NULL)
            {
                contact->email[i][strcspn(contact->email[i], "\n")] = '\0';
                contact->emails_count++;
            }
        }
    }

    return 1;
}

int set_phones(Contact* contact)
{
    char input[10];

    printf("Введите кол-во номеров телефона для ввода (0-%d): ", MAX_PHONES);
    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        int phone_count = atoi(input);
        phone_count = phone_count > 0 ? (phone_count > MAX_PHONES ? MAX_PHONES : phone_count) : 0;

        for (int i = 0; i < phone_count; i++)
        {
            printf("%d) ", i + 1);
            if (fgets(contact->phone_number[i], PHONE_NUM_LEN, stdin) != NULL)
            {
                contact->phone_number[i][strcspn(contact->phone_number[i], "\n")] = '\0';
                contact->phones_count++;
            }
        }
    }

    return 1;
}

int edit_emails(Contact* contact)
{
    char choice;

    printf("Выберите почту, которую хотите изменить: \n");
    for (int i = 0; i < contact->emails_count; i++)
    {
        printf("%d) %s|\n", i + 1, contact->email[i]);
    }

    if (contact->emails_count < MAX_MAILS)
    {
        printf("%d) Добавить новую|\n", contact->emails_count == MAX_MAILS ? MAX_MAILS : contact->emails_count + 1);
    }

    choice = getchar();
    int id = choice - '0';
    while(getchar() != '\n');

    if (id <= 0 || id > MAX_MAILS || choice == '\n')
    {
        return 0;
    }

    printf("(Enter в пустой строке для удаления)\n");
    if (fgets(contact->email[id - 1], EMAIL_LEN, stdin) != NULL)
    {
        contact->email[id - 1][strcspn(contact->email[id - 1], "\n")] = '\0';
        if (contact->email[id - 1][0] == '\0')
        {
            for (int i = id - 1; i < contact->emails_count - 1; i++)
            {
                strcpy(contact->email[id - 1], contact->email[i]);
            }
            contact->email[contact->emails_count - 1][0] = '\0';
            contact->emails_count = contact->emails_count == 0 ? 0 : contact->emails_count - 1;
        }
        else if (id > contact->emails_count)
        {
            contact->emails_count = contact->emails_count == MAX_MAILS ? MAX_MAILS : contact->emails_count + 1;
        }
    }
    
    return 1;
}

int edit_phones(Contact* contact)
{
    char choice;

    printf("Выберите телефон, который хотите изменить: \n");
    for (int i = 0; i < contact->phones_count; i++)
    {
        printf("%d) %s|\n", i + 1, contact->phone_number[i]);
    }

    if (contact->phones_count < MAX_PHONES)
    {
        printf("%d) Добавить новый|\n", contact->phones_count == MAX_PHONES ? MAX_PHONES : contact->phones_count + 1);
    }

    choice = getchar();
    int id = choice - '0';
    while(getchar() != '\n');

    if (id <= 0 || id > MAX_PHONES || choice == '\n')
    {
        return 0;
    }

    printf("(Enter в пустой строке для удаления)\n");
    if (fgets(contact->phone_number[id - 1], PHONE_NUM_LEN, stdin) != NULL)
    {
        contact->phone_number[id - 1][strcspn(contact->phone_number[id - 1], "\n")] = '\0';
        if (contact->phone_number[id - 1][0] == '\0')
        {
            for (int i = id - 1; i < contact->phones_count - 1; i++)
            {
                strcpy(contact->phone_number[id - 1], contact->phone_number[i]);
            }
            contact->phone_number[contact->phones_count - 1][0] = '\0';
            contact->phones_count = contact->phones_count == 0 ? 0 : contact->phones_count - 1;
        }
        else if (id > contact->phones_count)
        {
            contact->phones_count = contact->phones_count == MAX_MAILS ? MAX_MAILS : contact->phones_count + 1;
        }
    }

    return 1;
}

int input_contact_info(Contact* contact)
{
    set_l_name(contact);

    set_f_name(contact);

    set_patronymic(contact);

    set_w_name(contact);

    set_w_address(contact);

    set_w_post(contact);

    set_emails(contact);

    set_phones(contact);

    return 1;
}

int write_contact()
{
    Contact contact = {0};
    if (input_contact_info(&contact))
    {
        add_contact(&contact);
        return 1;
    }

    return 0;
}

static int is_create_contacts_file(const char* filename)
{
    printf("Файл с информацией о контактах не найден или недоступен!\nСоздать новый?\n1.Да\n2.Нет\n");
    int input = getchar();
    
    while (getchar() != '\n');
    
    return (input == '1');

}

int write_contacts_from_file(const char* filename)
{
    int fd;
    struct stat f_info;

    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", exec_dir, filename);

    if (stat(full_path, &f_info) == - 1)
    {
        if (!is_create_contacts_file(full_path))
            return 0;
    }

    mode_t old_umask = umask(0);
    fd = open(full_path, O_RDONLY | O_CREAT, 0666);
    umask(old_umask);
    
    if (fd == -1)
    {
        perror("file open/create error");
        exit(EXIT_FAILURE);
    }

    Contact contact = {0};
    while (read(fd, &contact, sizeof(Contact))) 
    {
        add_contact(&contact);
    }

    close(fd);
    return 1;
}

int edit_contact()
{
    int curr_contacts = get_contacts_count();

    if (curr_contacts == 0)
    {
        return 0;
    }

    char input[10];

    printf("Введите ID контакта, чьи данные вы хотите изменить (1-%d): ", curr_contacts);
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return 0;
    }

    int id = atoi(input);

    if (id < 1 || id > curr_contacts)
    {
        return 0;
    }

    char choice;
    int is_key_val_changed = 0;
    while (1)
    {
        printf("\033[2J\033[H");
        print_contact_by_id(id - 1);
        printf("Введите номер поля, которое вы хотите изменить:\n");
        printf("1. Фамилия\n");
        printf("2. Имя\n");
        printf("3. Отчество\n");
        printf("4. Название места работы\n");
        printf("5. Адрес места работы\n");
        printf("6. Должность\n");
        printf("7. Адреса почты\n");
        printf("8. Телефоны\n");
        printf("9. Выход\n");

        choice = getchar();
        if (choice == EOF) 
        {
            clearerr(stdin);
            choice = '\0';
            continue;
        }
        while(getchar() != '\n');

        switch(choice)
        {
            case '1':
                set_l_name(get_contact_by_id(id -1));
                is_key_val_changed = 1;             
                break;
            case '2':
                set_f_name(get_contact_by_id(id -1));
                is_key_val_changed = 1;
                break;
            case '3':
                set_patronymic(get_contact_by_id(id - 1));
                break;
            case '4':
                set_w_name(get_contact_by_id(id - 1));
                break;
            case '5':
                set_w_address(get_contact_by_id(id - 1));
                break;
            case '6':
                set_w_post(get_contact_by_id(id - 1));
                break;
            case '7':
                edit_emails(get_contact_by_id(id - 1));
                break;
            case '8':
                edit_phones(get_contact_by_id(id - 1));
                break;
            case '9': case '\n':
                if (is_key_val_changed)
                {
                    update_contact(id - 1);
                }
                return 0;
                break;
        }
    }

    if (is_key_val_changed)
    {
        printf("AAA\n");
        update_contact(id - 1);
    }

    return 0;
}

int delete_contact()
{
    int curr_contacts = get_contacts_count();

    if (curr_contacts == 0)
    {
        return 0;
    }

    char input[10];

    printf("Введите ID контакта, который вы хотите удалить (1-%d): ", curr_contacts);
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return 0;
    }

    int id = atoi(input);

    if (id < 1 || id > curr_contacts)
    {
        return 0;
    }

    if (remove_contact(id - 1))
    {
        return 1;
    }

    return 0;
}

void print_contacts()
{
    int count = get_contacts_count();

    printf("Телефонная книга (%d контактов):\n", count);
    
    for (int i = 0; i < count; i++) 
    {
        print_contact_by_id(i);
    }
}

void print_contact_by_id(int id)
{
    const Contact* contact = get_contact_by_id(id);
    printf("%d. %s %s %s\n", id + 1, contact->l_name, contact->f_name, contact->patronymic);
    
    if (strlen(contact->w_info.name) > 0)
    {
        printf("Место работы: %s %s %s\n", contact->w_info.name, contact->w_info.address, contact->w_info.post);
    }
    if (contact->emails_count > 0)
    {
        printf("Адреса почты:\n");
        for (int j = 0; j < contact->emails_count; j++)
        {
            printf(" %s|", contact->email[j]);
        }
        printf("\n");
    }
    if (contact->phones_count > 0) 
    {
        printf("   Телефоны:\n");
        for (int j = 0; j < contact->phones_count; j++)
        {
            printf(" %s|", contact->phone_number[j]);
        }
        printf("\n");
    }
    printf("=============================\n");
}

int export_contacts(const char* filename)
{
    int fd;
    struct stat f_info;

    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", exec_dir, filename);
    
    if (stat(full_path, &f_info) == - 1)
    {
        if (!is_create_contacts_file(full_path))
            return 0;
    }

    fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror("file write/create error");
        exit(EXIT_FAILURE);
    }

    Contact *contact;
    for (int i = 0; i < get_contacts_count(); i++) 
    {
        contact = get_contact_by_id(i);
        if (write(fd, contact, sizeof(Contact)) == -1)
        {
            perror("write error");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    printf("Информация успешно записана в %s\n", filename);
    return 1;
}
