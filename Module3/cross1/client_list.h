#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <netinet/in.h>

#define MAX_ID_LEN 32

typedef struct ClientInfo
{
    struct sockaddr_in cli_addr;
    int msg_count;
} ClientInfo;

typedef struct Node
{
    ClientInfo client;
    struct Node* left;
    struct Node* right;
    int height;
} Node;

typedef struct ContactTree
{
    Node* root;
    int count;
} ContactTree;

static Node* create_node(const ClientInfo*);

static void update_node_height(Node*);

static int get_balance_factor(Node*);

static Node* rotate_right(Node*);

static Node* rotate_left(Node*);

static void balance_node(Node**);

static void balance_tree(Node**);

int add_client(ClientInfo*);

int remove_client(char*);

ClientInfo* get_client(char*);

int get_clients_count();

Node* get_root();

void free_tree(Node*);

#endif