#include "contact_list.h"
#include <string.h>

Contact contacts[MAX_CONTACTS];
int current_contacts = 0;

int add_contact(const Contact* contact)
{
    if (current_contacts >= MAX_CONTACTS)
        return 0;

    if (strlen(contact->l_name) == 0 || strlen(contact->f_name) == 0)
        return 0;

    contacts[current_contacts++] = *contact;
    return 1;
}

int remove_contact(int id)
{
    if (id < 0 || id >= current_contacts)
    {
        return 0;
    }

    for (int i = id; i < current_contacts - 1; i++)
    {
        contacts[i] = contacts[i + 1];
    }

    memset(&contacts[current_contacts - 1], 0, sizeof(Contact));

    current_contacts--;
    return 1;
}

Contact* get_contact_by_id(int id)
{
    return &contacts[id];
}

int get_contacts_count()
{
    return current_contacts;
}
