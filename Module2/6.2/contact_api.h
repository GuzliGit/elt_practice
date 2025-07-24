#ifndef CONTACT_API_H
#define CONTACT_API_H

#include "contact_list.h"

int set_l_name(Contact* contact);

int set_f_name(Contact* contact);

int set_patronymic(Contact* contact);

int set_w_name(Contact* contact);

int set_w_address(Contact* contact);

int set_w_post(Contact* contact);

int set_emails(Contact* contact);

int set_phones(Contact* contact);

int edit_emails(Contact* contact);

int edit_phones(Contact* contact);

int input_contact_info(Contact* contact);

int write_contact();

int edit_contact();

int delete_contact();

void print_contacts();

void print_contact_by_id(int);

int export_contacts(const char*);

#endif