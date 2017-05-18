#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"
//销毁
void destroy_list(struct node *head){
	struct node *ptmp;
	while(head){
		ptmp = head;
		head = head->next;
		free(ptmp);
	}
}
//按名字key查找,返回key对应的value
char *get_value(struct node *head, char *key){
	struct node *ptmp;
	for(ptmp=head->next; ptmp; ptmp=ptmp->next){
		if(strcmp(ptmp->key, key)==0){
			return ptmp->value;
		}
	}
	return NULL;
}
//打印
void print_list(struct node *head){
	struct node *ptmp;
	for(ptmp=head->next; ptmp; ptmp=ptmp->next){
		printf("%s:%s\n", ptmp->key, ptmp->value);
	}
	printf("\n");
}
//尾插法
void insert_in_end(struct node *head, struct node *pnew){
	struct node *ptmp;
	for(ptmp=head; ptmp->next; ptmp=ptmp->next);
	ptmp->next = pnew;
}
//添加节点
void add_node(struct node *head, char *key, char *value){
	struct node *pnew;
	pnew = (struct node *)malloc(sizeof(struct node));
	strcpy(pnew->key, key);
	strcpy(pnew->value, value);
	pnew->next = NULL;
	insert_in_end(head, pnew);//尾插法
}
//创建链表
struct node *creat_list(){
	struct node *head;
	head = (struct node *)malloc(sizeof(struct node));
	strcpy(head->key, "");
	strcpy(head->value, "");
	head->next = NULL;
	return head;
}
