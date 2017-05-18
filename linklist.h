#ifndef LINKLIST_H
#define LINKLIST_H
struct node{
	char key[1000];
	char value[1000];
	struct node *next;
};
struct node *creat_list();
void add_node(struct node *head, char *key, char *value);
void insert_in_end(struct node *head, struct node *pnew);//头插法
void print_list(struct node *head);
char *get_value(struct node *head, char *key);
void destroy_list(struct node *head);
#endif
