#include"link.h"
//5个


//增：输入用户链表和用户结构体				
void add_node(PNODE head, PNODE_DATA pnode_data)
{
	PNODE temp = create_node(pnode_data);
	temp->next = head->next;
	head->next = temp;
	printf("\nNew user add completed.\n");
}

//查：输入用户链表和用户id，返回找到节点的前一个节点或NULL		    *
PNODE find_pre_node(PNODE head, int id)
{
	if(NULL == head->next)
	{
		return NULL;
	}
	PNODE q = head;
	PNODE p = head->next;
	while(p != NULL)
	{
		if(p->pnode_data.id == id)
		{
			return q;
		}
		q = q->next;
		p = p->next;
	}
	return NULL;
}


//创建用户节点							
//输入用户结构体数据					   	
//返回新创建的用户节点						
PNODE create_node(PNODE_DATA pnode_data)
{
	PNODE temp = (PNODE)malloc(sizeof(NODE));
	temp->pnode_data = pnode_data;
	temp->next = NULL;
//	printf("创建完成．\n");
	return temp;
}


//从文件中加载用户数据，返回用户链表								*
PNODE load_data()
{
	FILE *fp;
	if(NULL == (fp = fopen("data", "r")))
	{
		if(NULL == (fp = fopen("data", "w+")))
		{
			system("clear");
			printf("\nHard disk file content loading failed. Please reload.\n");
			exit(0);
		}	
	}
	
	PNODE head = (PNODE)malloc(sizeof(NODE));
	head->next = NULL;
	PNODE_DATA pnode_data; 
	
	while(fread(&pnode_data, sizeof(PNODE_DATA), 1, fp) != 0)
	{
		pnode_data.sockfd = -1;
		
		add_node(head, pnode_data);
	}
	
//	system("clear");
	printf("\nThe data was loaded successfully.\n");

	fclose(fp);
	fp = NULL;
	
	return head;
}


//保存用户数据，输入用户链表
void save_data(PNODE head)
{
	FILE *fp;
	if(NULL == (fp = fopen("data", "w+")))
	{
		system("clear");
		printf("\nData save failed, please save again.\n");
		exit(0);
	}
	if(NULL == head->next)
	{
		printf("The list is empty when you exit, so you don't need to save data.\n");
		return;
	}
	
	PNODE p = head->next;
	while(p != NULL)
	{
		fwrite(&p->pnode_data, sizeof(PNODE_DATA), 1, fp);
		p = p->next;
	}

//	printf("\n用户数据保存成功．\n");
	fclose(fp);
	fp = NULL;
}

