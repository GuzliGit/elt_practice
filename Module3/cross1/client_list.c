#include "client_list.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ContactTree clients = {NULL, 0};

static Node* create_node(const ClientInfo* client)
{
    Node* node = malloc(sizeof(Node));
    if (node)
    {
        node->client = *client;
        node->left = node->right = NULL;
        node->height = 1;
    }
    return node;
}

static void update_node_height(Node* node)
{
    if (!node) 
        return;

    int left_h = node->left ? node->left->height : 0;
    int right_h = node->right ? node->right->height : 0;
    node->height = (left_h > right_h ? left_h : right_h) + 1;
}

static int get_balance_factor(Node* node)
{
    int left_h = node->left ? node->left->height : 0;
    int right_h = node->right ? node->right->height : 0;
    return left_h - right_h;
}

static Node* rotate_right(Node* p)
{
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    update_node_height(p);
    update_node_height(q);
    return q;
}

static Node* rotate_left(Node* q)
{
    Node* p = q->right;
    q->right = p->left;
    p->left = q;
    update_node_height(q);
    update_node_height(p);
    return p;
}

static void balance_node(Node** node)
{
    int factor = get_balance_factor(*node);

    if (factor > 1)
    {
        if (get_balance_factor((*node)->left) < 0)
            (*node)->left = rotate_left((*node)->left);
        *node = rotate_right(*node);
    }
    else if (factor < -1)
    {
        if (get_balance_factor((*node)->right) > 0)
            (*node)->right = rotate_right((*node)->right);
        *node = rotate_left(*node);
    }
}

static void balance_tree(Node** node)
{
    if (!*node)
        return;

    balance_tree(&(*node)->left);
    balance_tree(&(*node)->right);
    update_node_height(*node);
    balance_node(node);
}

int add_client(ClientInfo* client)
{
    if (!client || client->cli_addr.sin_port == 0)
        return 0;

    client->msg_count = 0;

    Node** current = &clients.root;
    char c1[MAX_ID_LEN], c2[MAX_ID_LEN];
    while (*current) 
    {
        snprintf(c1, MAX_ID_LEN, "%s:%d", inet_ntoa(client->cli_addr.sin_addr), ntohs(client->cli_addr.sin_port));
        snprintf(c2, MAX_ID_LEN, "%s:%d", inet_ntoa((*current)->client.cli_addr.sin_addr), ntohs((*current)->client.cli_addr.sin_port));
        int cmp = strcmp(c1, c2);
        if (cmp < 0)
            current = &(*current)->left;
        else
            current = &(*current)->right;
    }

    *current = create_node(client);
    if (!*current)
        return 0;

    clients.count++;
    balance_tree(&clients.root);
    return 1;
}

int remove_client(char* ip_port)
{
    if (strlen(ip_port) == 0)
    {
        return 0;
    }

    Node** to_remove = &clients.root;
    char c2[MAX_ID_LEN];
    while (*to_remove) 
    {
        snprintf(c2, MAX_ID_LEN, "%s:%d", inet_ntoa((*to_remove)->client.cli_addr.sin_addr), ntohs((*to_remove)->client.cli_addr.sin_port));
        int cmp = strcmp(ip_port, c2);
        if (cmp == 0)
        {
            break;
        }
        else if (cmp < 0)
        {
            to_remove = &(*to_remove)->left;
        }
        else
        {
            to_remove = &(*to_remove)->right;
        }
    }

    if (!*to_remove)
        return 0;

    Node* temp = *to_remove;
    if (!temp->left)
        *to_remove = temp->right;
    else if (!temp->right)
        *to_remove = temp->left;
    else
    {
        Node** min = &temp->right;
        while ((*min)->left) 
        {
            min = &(*min)->left;
        }

        Node* min_node = *min;
        *min = min_node->right;
        min_node->left = temp->left;
        min_node->right = temp->right;
        *to_remove = min_node;
    }

    free(temp);
    clients.count--;
    balance_tree(&clients.root);
    return 1;
}

ClientInfo* get_client(char* ip_port)
{
    if (strlen(ip_port) == 0)
    {
        return NULL;
    }

    Node* current = clients.root;
    char c2[MAX_ID_LEN];
    while (current)
    {
        snprintf(c2, MAX_ID_LEN, "%s:%d", inet_ntoa(current->client.cli_addr.sin_addr), ntohs(current->client.cli_addr.sin_port));
        int cmp = strcmp(ip_port, c2);
        if (cmp == 0)
        {
            return &current->client;
        }
        else if (cmp < 0)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }
    return NULL;
}

int get_clients_count()
{
    return clients.count;
}

Node* get_root()
{
    return clients.root;
}

void free_tree(Node* node)
{
    if (!node)
    {
        return;
    }

    free_tree(node->left);
    free_tree(node->right);
    free(node);
}
