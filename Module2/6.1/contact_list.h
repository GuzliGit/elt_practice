#ifndef CONTACT_LIST_H
#define CONTACT_LIST_H

#define LF_NAME_PATRONYMIC_LEN 30
#define WORK_NAME_LEN 30
#define WORK_ADDRESS_LEN 30
#define WORK_POST_LEN 30
#define EMAIL_LEN 40
#define MAX_MAILS 5
#define PHONE_NUM_LEN 25
#define MAX_PHONES 5

typedef struct WorkInfo
{
    char name[WORK_NAME_LEN];
    char address[WORK_ADDRESS_LEN];
    char post[WORK_POST_LEN];
} WorkInfo;

typedef struct Contact
{
    char l_name[LF_NAME_PATRONYMIC_LEN];
    char f_name[LF_NAME_PATRONYMIC_LEN];
    char patronymic[LF_NAME_PATRONYMIC_LEN];
    WorkInfo w_info;
    int emails_count;
    char email[MAX_MAILS][EMAIL_LEN];
    int phones_count;
    char phone_number[MAX_PHONES][PHONE_NUM_LEN];
} Contact;

typedef struct ContactList
{
    Contact val;
    struct ContactList* next;
    struct ContactList* prev;
} ContactList;

int contact_cmp(const Contact*, const Contact*);

void swap_contacts(ContactList**, ContactList*, ContactList*);

void sort_contact_list();

int add_contact(const Contact*);

int remove_contact(int);

Contact* get_contact_by_id(int);

int get_contacts_count();

#endif