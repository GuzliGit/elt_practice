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

typedef struct Node
{
    Contact contact;
    struct Node* left;
    struct Node* right;
    int height;
    int size;
} Node;

typedef struct ContactTree
{
    Node* root;
    int count;
} ContactTree;

Node* create_node(const Contact*);

int contact_cmp(const Contact*, const Contact*);

void update_node_height(Node*);

int get_balance_factor(Node*);

Node* rotate_right(Node*);

Node* rotate_left(Node*);

void balance_node(Node**);

void balance_tree(Node**);

int add_contact(const Contact*);

int remove_contact(int);

Contact* get_contact_by_id(int);

int get_contacts_count();

void update_contact(int);

void print_tree(Node*, int);

void visualize_full_tree();

#endif