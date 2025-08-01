#include "contact_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ContactTree contacts = {NULL, 0};

Node* create_node(const Contact* contact)
{
    Node* node = malloc(sizeof(Node));
    if (node)
    {
        node->contact = *contact;
        node->left = node->right = NULL;
        node->height = 1;
        node->size = 1;
    }
    return node;
}

int contact_cmp(const Contact* c1, const Contact* c2)
{
    int cmp = strcmp(c1->l_name, c2->l_name);
    if (cmp != 0)
    {
        return cmp;
    }
    
    return strcmp(c1->f_name, c2->f_name);
}

void update_node_height(Node* node)
{
    if (!node) 
        return;

    int left_h = node->left ? node->left->height : 0;
    int right_h = node->right ? node->right->height : 0;
    node->height = (left_h > right_h ? left_h : right_h) + 1;

    int left_size = node->left ? node->left->size : 0;
    int right_size = node->right ? node->right->size : 0;
    node->size = 1 + left_size + right_size;
}

int get_balance_factor(Node* node)
{
    int left_h = node->left ? node->left->height : 0;
    int right_h = node->right ? node->right->height : 0;
    return left_h - right_h;
}

Node* rotate_right(Node* p)
{
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    update_node_height(p);
    update_node_height(q);
    return q;
}

Node* rotate_left(Node* q)
{
    Node* p = q->right;
    q->right = p->left;
    p->left = q;
    update_node_height(q);
    update_node_height(p);
    return p;
}

void balance_node(Node** node)
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

void balance_tree(Node** node)
{
    if (!*node)
        return;

    balance_tree(&(*node)->left);
    balance_tree(&(*node)->right);
    update_node_height(*node);
    balance_node(node);
}

int add_contact(const Contact* contact)
{
    if (strlen(contact->l_name) == 0 || strlen(contact->f_name) == 0)
        return 0;

    Node** current = &contacts.root;
    while (*current) 
    {
        int cmp = contact_cmp(contact, &(*current)->contact);
        if (cmp < 0)
            current = &(*current)->left;
        else
            current = &(*current)->right;
    }

    *current = create_node(contact);
    if (!*current)
        return 0;

    contacts.count++;
    balance_tree(&contacts.root);
    return 1;
}

int remove_contact(int id)
{
    if (id < 0 || id >= contacts.count)
    {
        return 0;
    }

    Node** to_remove = &contacts.root;
    while (*to_remove) 
    {
        int left_size = (*to_remove)->left ? (*to_remove)->left->size : 0;
        if (id == left_size)
            break;
        else if (id < left_size)
            to_remove = &(*to_remove)->left;
        else
        {
            id -= (left_size + 1);
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
    contacts.count--;
    balance_tree(&contacts.root);
    return 1;
}

Contact* get_contact_by_id(int id)
{
    if (id < 0 || id >= contacts.count)
        return NULL;

    Node* current = contacts.root;
    while (current)
    {
        int left_size = current->left ? current->left->size : 0;
        if (id == left_size)
            return &current->contact;
        else if (id < left_size)
            current = current->left;
        else
        {
            id -= (left_size + 1);
            current = current->right;
        }
    }
    return NULL;
}

int get_contacts_count()
{
    return contacts.count;
}

void update_contact(int id)
{
    Contact* old_contact = get_contact_by_id(id);
    if (!old_contact)
        return;

    Contact copy = *old_contact;
    if (remove_contact(id)) {
        add_contact(&copy);
    }
}

void print_tree(Node* root, int level)
{
    if (!root)
        return;

    print_tree(root->right, level + 1);

    for (int i = 0; i < level; i++)
    {
        printf(" ");
    }

    printf("%s %s (h=%d s=%d)\n", root->contact.l_name, root->contact.f_name, root->height, root->size);

    print_tree(root->left, level + 1);
}

void visualize_full_tree()
{
    print_tree(contacts.root, 0);
}
