#include "contact_list.h"
#include <stdlib.h>
#include <string.h>

ContactList* head = NULL;
int current_contacts = 0;

int contact_cmp(const Contact* c1, const Contact* c2)
{
    int cmp = strcmp(c1->l_name, c2->l_name);
    if (cmp != 0)
    {
        return cmp;
    }
    
    return strcmp(c1->f_name, c2->f_name);
}

void swap_contacts(ContactList** head_tmp, ContactList* c1, ContactList* c2)
{
    if (c1 == c2) return;

    if (*head_tmp == c1) 
        *head_tmp = c2;
    else if (*head_tmp == c2) 
        *head_tmp = c1;

    ContactList* temp;
    temp = c1->next;
    c1->next = c2->next;
    c2->next = temp;

    if (c1->next) 
        c1->next->prev = c1;
    if (c2->next) 
        c2->next->prev = c2;

    temp = c1->prev;
    c1->prev = c2->prev;
    c2->prev = temp;

    if (c1->prev) 
        c1->prev->next = c1;
    if (c2->prev) 
        c2->prev->next = c2;
}

void sort_contact_list()
{
    if (!head || !head->next)
        return;

    ContactList* current = head;
    while (current) 
    {
        ContactList* min = current;
        ContactList* temp = current->next;

        while (temp) 
        {
            if (contact_cmp(&temp->val, &min->val) < 0)
            {
                min = temp;
            }
            temp = temp->next;
        }

        if (min != current)
        {
            swap_contacts(&head, current, min);
            ContactList* tmp = current;
            current = min;
            min = tmp;
        }

        current = current->next;
    }
}

int add_contact(const Contact* contact)
{
    if (strlen(contact->l_name) == 0 || strlen(contact->f_name) == 0)
        return 0;

    ContactList* new_contact = malloc(sizeof(ContactList));
    if (!new_contact)
        return 0;

    new_contact->val = *contact;
    new_contact->next = NULL;
    new_contact->prev = NULL;

    if (!head)
    {
        head = new_contact;
    }
    else
    {
        ContactList* tail = head;
        while (tail->next)
        {
            tail = tail->next;
        }

        tail->next = new_contact;
        new_contact->prev = tail;
    }

    current_contacts++;
    sort_contact_list();
    return 1;
}

int remove_contact(int id)
{
    if (id < 0 || id >= current_contacts)
    {
        return 0;
    }

    ContactList* current = head;
    for (int i = 0; i < id && current; i++)
    {
        current = current->next;
    }

    if (!current)
        return 0;

    if (current->prev)
        current->prev->next = current->next;
    else
        head = current->next;
    
    if (current->next)
        current->next->prev = current->prev;

    free(current);
    current_contacts--;
    return 1;
}

Contact* get_contact_by_id(int id)
{
    if (id < 0 || id >= current_contacts)
        return NULL;

    ContactList* current = head;
    for (int i = 0; i < id && current; i++)
    {
        current = current->next;
    }

    return current ? &current->val : NULL;
}

int get_contacts_count()
{
    return current_contacts;
}
