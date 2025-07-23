#ifndef SUBNET_H
#define SUBNET_H

void ip_to_str(unsigned int, char*);

int str_to_ip(const char*, unsigned int*);

int start_sending_sim(char**);

#endif