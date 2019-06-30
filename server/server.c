 #include"server.h"
#include"link.h"
#include <mysql/mysql.h>
#include <stddef.h> 
#include <sys/types.h> 
#include <dirent.h> 
void gettime(char *cur_time) {
        char Year[6] = {0};
        char Month[4] = {0};
        char Day[4] = {0};
        char Hour[4] = {0};
        char Min[4] = {0};
        char Sec[4] = {0};

        time_t current_time;
        struct tm* now_time;
        time(&current_time);
        now_time = localtime(&current_time);

        strftime(Year, sizeof(Year), "%Y-", now_time);
        strftime(Month, sizeof(Month), "%m-", now_time);
        strftime(Day, sizeof(Day), "%d ", now_time);
        strftime(Hour, sizeof(Hour), "%H:", now_time);
        strftime(Min, sizeof(Min), "%M:", now_time);
        strftime(Sec, sizeof(Sec), "%S", now_time);

        strncat(cur_time, Year, 5);
        strncat(cur_time, Month, 3);
        strncat(cur_time, Day, 3);
        strncat(cur_time, Hour, 3);
        strncat(cur_time, Min, 3);
        strncat(cur_time, Sec, 3);
}
char cur_time[100] ;
        //gettime(cur_time);//获得当前时间


    MYSQL           mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    char            *query_str=NULL;
    int             rc, i, fields;
    int             rows;


//All print messages on the server are used to facilitate program tracking and debugging
int main(int argc, char **argv)
{
	head = load_data();//启动服务器，加载用户数据
	gettime(&cur_time);
	//mysql
    
    if (NULL == mysql_init(&mysql)) {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
    if (NULL == mysql_real_connect(&mysql,
                "172.17.16.5",
                "root",
                "c12345678",
                "linuxqq",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }else
	{ printf("Connected MySQL successful! \n"); }
	
	
	//遍历链表
	PNODE p = head->next;
	while(p != NULL)
	{
		printf("id = %d, sockfd = %d\n", p->pnode_data.id, p->pnode_data.sockfd);
		p = p->next;
	}
	
	
	socklen_t	clilen;//int 客户端消息长度变量
	struct sockaddr_in	cliaddr, servaddr;//套接字 服务器和客户端

    //创建套接字
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);//打印服务器套接字
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
    //绑定
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    //监听
	listen(listenfd, 5);
	printf("Now listening port %d\n", SERV_PORT);//在.h文件中

	int times = 0;
	for( ; ; )
	{
		clilen = sizeof(cliaddr);//长度赋值给客户端套接字长度
         
		 
		//等待accept
		connfd[times] = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		printf("connfd[%d] = %d\n", times, connfd[times]);//打印出来新连接的客户端套接字和位次

		int ret;//线程id
		ret = pthread_create(&tid[times], NULL, str_echo, &connfd[times]);//创建线程处理客户端发来的请求
		/*
	第一个参数为指向线程标识符的指针。

　　第二个参数用来设置线程属性。

　　第三个参数是线程运行函数的地址。

　　最后一个参数是运行函数的参数。
	*/
		if(ret != 0)
		{
			perror("pthread_create");
			return -1;
		}
		else
		{
			printf("ip: %s connected...\n", inet_ntoa(cliaddr.sin_addr));
		}
		
		times++;
	}
	
	return 0;
}



//处理客户端发来的消息,输入客户端连接套接字						*	
void *str_echo(void *param)
{
	int sockfd = *(int *)param;
	int my_id;
	int find_id;
	DATA data;
	
	printf("sockfd = %d\n", sockfd);

	int num = 0;
	//读取客户端消息函数，替代系统函数read
	while(readn(sockfd, &data, sizeof(DATA)) > 0)
	{	
		num++;
		printf("\ncom[%d]: %s\n", num, data.com);
        //mysql insert
        char myidd[11];		
		sprintf(myidd, "%d", data.id); 
		
		char query_strr[1024] = "INSERT INTO `logs` (`log_id`, `com`, `id`, `message`, `id_name`) VALUES (NULL, '";
        strncat(query_strr,data.com,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,myidd,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,"2333",100);
        strncat(query_strr,"','",10);
        strncat(query_strr,"2333",100);
        strncat(query_strr,"');",10);


    rc = mysql_real_query(&mysql, query_strr, strlen(query_strr));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        //return -1;
    }
    res = mysql_store_result(&mysql);
	
		
		//按协议处理
		if(strcmp(data.com, "exit") == 0)
		{
			printf("^_^ Users with id %d have quit.\n", data.id);
			PNODE find = find_pre_node(head, data.id);
			 
			printf("find->next->pnode_data.sockfd = %d\n", find->next->pnode_data.sockfd);

			int i;
			for(i = 0; i < 50; i++)
			{
				if(find_sockfd[i] == find->next->pnode_data.sockfd)
				{
					pthread_mutex_lock(&mutex);                         //线程锁，防止其他线程同时更改全局变量find_sockfd[]
					find_sockfd[i] = 0;
					printf("find_sockfd[%d]Initialization for0.\n", i);	
					pthread_mutex_unlock(&mutex);                       //解锁
				}
			}
			
			find->next->pnode_data.sockfd = -1;
			printf("^_^The linked list sockfd of user %d has been initialized to %d\n", data.id, find->next->pnode_data.sockfd);
		}
		
		else if(strcmp(data.com, "login") == 0)                         //登录      
		{
			PNODE find = find_pre_node(head, data.id);            
			if(NULL == find || (strcmp(find->next->pnode_data.passwd, data.message) != 0))    //登录失败
			{
				strcpy(data.message, buf1);
				writen(sockfd, &data, sizeof(DATA));
			}
			else if(find->next->pnode_data.sockfd != -1)                //id已经登录 
			{
				strcpy(data.message, buf7);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //登录成功
			{
				my_id = find->next->pnode_data.id;

				printf("Before login sockfd = %d\n", find->next->pnode_data.sockfd);
				printf("Users with id %d logged in successfully.\n", find->next->pnode_data.id);
				find->next->pnode_data.sockfd = sockfd;
				printf("After login sockfd = %d\n", find->next->pnode_data.sockfd);
				
				int i;
				for(i = 0; i < 50; i++)
				{
					if(find_sockfd[i])
					{
						char online_message[MAXLINE];
						char online_id_str[5];

						sprintf(online_id_str, "%d", find->next->pnode_data.id);
						strcpy(online_message, "\nUser ");
						strcat(online_message, online_id_str);
						strcat(online_message, " is online now.");

						strcpy(data.com, "user_online");
						strcpy(data.message, online_message);
						
						printf("Send an online reminder to an online user whose connection socket is%d.\n", find_sockfd[i]);
						writen(find_sockfd[i], &data, sizeof(DATA));
					}
				}
				
				for(i = 0; i < 50; i++)                                 //登录成功，把连接套接字加入到find_sockfd[10]
				{
					if(find_sockfd[i])
						continue;
					
					pthread_mutex_lock(&mutex);
					find_sockfd[i] = find->next->pnode_data.sockfd;
					pthread_mutex_unlock(&mutex);
					
					break;
				}
				
				strcpy(data.message, buf0);
				writen(sockfd, &data, sizeof(DATA));
	
			}
	
		}
		
		else if(strcmp(data.com, "regist") == 0)                        //注册 
		{
			PNODE find = find_pre_node(head, data.id);
			if(NULL == find)                                            //可以注册
			{
				printf("New users can be created.\n");
				
				PNODE_DATA pnode_data;
				pnode_data.id = data.id;
				
				my_id = data.id;
				
				strncpy(pnode_data.passwd, data.message, MAXLINE);    
				pnode_data.sockfd = sockfd;
				
				add_node(head, pnode_data);
				
				int i;
				for(i = 0; i < 50; i++)
				{
					if(find_sockfd[i])
					{
						char online_message[MAXLINE];
						char online_id_str[5];

						sprintf(online_id_str, "%d", data.id);
						strcpy(online_message, "\nUser ");
						strcat(online_message, online_id_str);
						strcat(online_message, " is online now.");

						strcpy(data.com, "user_online");
						strcpy(data.message, online_message);
						
						printf("Send an online reminder to an online user whose connection socket is%d.\n", find_sockfd[i]);
						writen(find_sockfd[i], &data, sizeof(DATA));
					}
				}
				
				for(i = 0; i < 50; i++)                                 //注册成功，把连接套接字加入到find_sockfd[10]
				{
					if(find_sockfd[i])
						continue;
					
					pthread_mutex_lock(&mutex);
					find_sockfd[i] = sockfd;
					pthread_mutex_unlock(&mutex);
					
					break;
				}
				
				strcpy(data.message, buf3);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //id已经注册
			{
				printf("The ID already exists.\n");
				strcpy(data.message, buf4);
				writen(sockfd, &data, sizeof(DATA));
			}
		}
		
		else if(strcmp(data.com, "private_chat") == 0)                  //私聊
		{
			PNODE find = find_pre_node(head, data.id);
			if(NULL == find || find->next->pnode_data.sockfd == -1)     //没有私聊对象或对象没在线
			{	
				printf("There are no users to talk to.\n");
				strcpy(data.message, buf6);
				writen(sockfd, &data, sizeof(DATA));
			}
			else                                                        //返回可以私聊的信息给客户端
			{
				find_id = find->next->pnode_data.id;
				data.id = find->next->pnode_data.sockfd;

				printf("You can talk in private.\n");
				strcpy(data.message, buf5);
				writen(sockfd, &data, sizeof(DATA));
				//insert
			char myidd[11];		
		    sprintf(myidd, "%d", data.id_name); 
		    char toidd[11];
			sprintf(toidd, "%d", find_id);
		char query_strr[1024] = "INSERT INTO `p_chat` (`p_id`, `id`, `message`, `id_name`,time) VALUES (NULL, '";
        strncat(query_strr,myidd,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,data.message,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,toidd,100);
        strncat(query_strr,"','",10);
		strncat(query_strr,cur_time,100);
        strncat(query_strr,"');",10);


    rc = mysql_real_query(&mysql, query_strr, strlen(query_strr));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        //return -1;
    }
    res = mysql_store_result(&mysql);
		
			}
		}
	
		else if(strcmp(data.com, "pri_data") == 0)                      //发送私聊消息
		{
			if(strcmp(data.message, "exit") == 0)
			{
				printf("User %d has exited.\n", my_id);
			}
			
			printf("Send a message to %d.\n", find_id);
			data.id_name = my_id;
			writen(data.id, &data, sizeof(DATA));
			//insert
			char myidd[11];		
		    sprintf(myidd, "%d", data.id_name); 
		    char toidd[11];
			sprintf(toidd, "%d", find_id);
		char query_strr[1024] = "INSERT INTO `p_chat` (`p_id`, `id`, `message`, `id_name`,time) VALUES (NULL, '";
        strncat(query_strr,myidd,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,data.message,100);
        strncat(query_strr,"','",10);
        strncat(query_strr,toidd,100);
        strncat(query_strr,"','",10);
		strncat(query_strr,cur_time,100);
        strncat(query_strr,"');",10);


    rc = mysql_real_query(&mysql, query_strr, strlen(query_strr));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        //return -1;
    }
    res = mysql_store_result(&mysql);
		}
		
		else if(strcmp(data.com, "group_chat") == 0)                    //把套接字加入到群聊结构体
		{
			int i;
			for(i = 0; i < 50; i++)
			{
				printf("^_^ group_chat_sockfd[%d] = %d, sockfd = %d\n", i, group_chat_sockfd[i], sockfd);

				if(group_chat_sockfd[i] == sockfd)
					break;
				if(group_chat_sockfd[i])
					continue;

				pthread_mutex_lock(&mutex);
				group_chat_sockfd[i] = sockfd;
				pthread_mutex_unlock(&mutex);
					
				break;
			}
			
			printf("Add socket %d to group_chat_sockfd successfully.\n", sockfd);

			strcpy(data.message, buf8);
			writen(sockfd, &data, sizeof(DATA));
		}
		
		else if(strcmp(data.com, "group_message") == 0)                 //发送群聊消息
		{
			if(strcmp(data.message, "exit") == 0)
			{
				printf("User %d has quit group chat.\n",my_id);

				int i;
				for(i = 0; i < 50; i++)
				{
					if(group_chat_sockfd[i] == sockfd)
					{
						pthread_mutex_lock(&mutex);
						group_chat_sockfd[i] = 0;
						printf("group_chat_sockfd[%d]Initialized as0.\n", i);
						pthread_mutex_unlock(&mutex);
						
						break;
					}
				}
			}
			
			int i;
			for(i = 0; i < 50; i++)
			{
				if(group_chat_sockfd[i] != 0)                           //如果这里的0是-1,write时终端会有信心显示，为什么？                     
				{
					printf("group_chat_sockfd[%d] = %d, my sockfd = %d\n", i, find_sockfd[i], sockfd);

					if((group_chat_sockfd[i] == sockfd) && (strcmp(data.message, "exit") == 0))  //如果是退出群聊的用户就不发送消息
						continue;
					data.id_name = my_id;
					
					printf("Send a message to the sockfd %d user.\n", group_chat_sockfd[i]);
					writen(group_chat_sockfd[i], &data, sizeof(DATA));
				}
			}
		}
		
		else if(strcmp(data.com, "view_online_id") == 0)                //发送在线用户列表
		{
			printf("Send a list of online users.\n");

			strcpy(data.message, "\nNow online user:\n");
			PNODE p = head->next;
			char buf[10];
			while(p != NULL)                                            //遍历链表查看在线用户
			{
				if(p->pnode_data.sockfd != -1)
				{
					sprintf(buf, "%d", p->pnode_data.id);
					strcat(data.message, buf);
					strcat(data.message, "\n");
				}
				p = p->next;
			}
			
			writen(sockfd, &data, sizeof(DATA));
			printf("A list of online users has been sent.\n");
		}
		
		//新添加功能
		//新添加功能
		//新添加功能
		//===============//
		else if(strcmp(data.com, "look_file") == 0)                //发送文件列表
		{
			//system("sudo su");
			/*
			int object_sockfd;
			printf("%d Send a file to %d\n",data.id,data.id_name);
            strcpy(data.com, "send_file");
			//strcpy(data.message, "文件已发送给指定用户：");
			PNODE p = head->next;
			char buf[10];
			while(p != NULL)                                            //遍历链表查看在线用户
			{
				if(p->pnode_data.id ==data.id_name)//sockfd
				{
					sprintf(buf, "%d", p->pnode_data.id);
					strcat(data.message, buf);
					strcat(data.message, "\n");
					object_sockfd=p->pnode_data.sockfd;//保存要发给用户的sockfd
				}
				p = p->next;
			}

			//writen(sockfd, &data, sizeof(DATA));
			//writen(object_sockfd,&data,sizeof(DATA));
			//然后处理发送文件的目标
			/*
			DATA datadata;
			strcpy(datadata.message, "文件是：");
			strcat(datadata.message, data.id);
			strcat(datadata.message, "发送来的\n");
			strcpy(datadata.com, "rev_file");
			writen(object_sockfd, &datadata, sizeof(DATA));
			printf("send file success\n");
			*/
			
			DIR* dp; 
            struct dirent* ep; 

            dp = opendir("/opt/lampp/htdocs/data"); 
            if (dp != NULL) 
            { 
            while (ep = readdir(dp)) 
			{
			//修改权限
            char cmd[1024];
	        stpcpy(cmd,"chmod 777 /opt/lampp/htdocs/data/");
	        strcat(cmd, ep->d_name);
	        system(cmd);			
				
			//写入	
            strcat(data.message,ep->d_name); 
		    strcat(data.message,"\n"); 
			}
            (void) closedir(dp); 
            } 
			strcpy(data.com, "look_file");
			writen(sockfd, &data, sizeof(DATA));
		}
		
		//new
		//new
		//new
		
		else if(strcmp(data.com, "new_friend") == 0)      //添加好友
		{
			PNODE find = find_pre_node(head, data.id);
			if(NULL == find || find->next->pnode_data.sockfd == -1)     //没有添加好友对象或对象没在线
			{	
			    strcpy(data.com, "new_friend_none");
				printf("There are no users to make friend.\n");
				strcpy(data.message, "你想添加的好友不存在或者暂时不在线");
				writen(sockfd, &data, sizeof(DATA));
			}
			else {
				find_id = find->next->pnode_data.id;
				data.id = find->next->pnode_data.sockfd;
				strcpy(data.com, "new_friend_request");
				data.id_name = my_id;
				char myidd[11];		
		        sprintf(myidd, "%d", data.id_name); 
				strcpy(data.message, myidd);
				strcat(data.message,"想添加你为好友，是否同意？ 1.同意\t2.拒绝，请选择：");
				writen(data.id, &data, sizeof(DATA));
			}   

			
		
		}
		
		else if(strcmp(data.com, "new_friend_rev") == 0)      //添加好友回复
		{
			printf("\n%s\n",data.message);
			 PNODE find = find_pre_node(head, data.id);
		     //if(strcmp(data.message,"ok")){
				 printf("%d agree %d to friend\n",my_id,data.id);
				 find_id = find->next->pnode_data.id;
				 int toid = find->next->pnode_data.sockfd;
				 strcpy(data.com, "new_friend_rev");
				data.id_name = my_id;
				char myidd[11];		
		        sprintf(myidd, "%d", data.id_name); 
				strcpy(data.message, myidd);
				strcat(data.message,"已同意添加你为好友，你们已经是好友啦，快点开始聊天吧~");
				writen(toid, &data, sizeof(DATA));
					//insert
		    char toidd[11];
			sprintf(toidd, "%d", find_id);
		    char query_strr[1024] = "INSERT INTO `friend` (`f_id`, `host`, `friend`, `f_time`) VALUES (NULL, '";
            strncat(query_strr,myidd,100);
            strncat(query_strr,"','",10);
            strncat(query_strr,toidd,100);
            strncat(query_strr,"','",10);
		    strncat(query_strr,cur_time,100);
            strncat(query_strr,"');",10);
            rc = mysql_real_query(&mysql, query_strr, strlen(query_strr));
            if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            //return -1;
            }
            res = mysql_store_result(&mysql);
			//
			char query_strrr[1024] = "INSERT INTO `friend` (`f_id`, `host`, `friend`, `f_time`) VALUES (NULL, '";
            strncat(query_strrr,toidd,100);
            strncat(query_strrr,"','",10);
            strncat(query_strrr,myidd,100);
            strncat(query_strrr,"','",10);
		    strncat(query_strrr,cur_time,100);
            strncat(query_strrr,"');",10);
            rc = mysql_real_query(&mysql, query_strrr, strlen(query_strrr));
            if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            //return -1;
            }
            res = mysql_store_result(&mysql);
			
			//}
			/*
			 else {
	              find_id = find->next->pnode_data.id;
				 data.id = find->next->pnode_data.sockfd;
				 strcpy(data.com, "new_friend_rev");
				data.id_name = my_id;
				char myidd[11];		
		        sprintf(myidd, "%d", data.id_name); 
				strcpy(data.message, myidd);
				strcat(data.message,"已拒绝与你成为好友");
				writen(data.id, &data, sizeof(DATA));
				  
			 }
		     */
		}
		
		else if(strcmp(data.com, "friend_list") == 0)      //好友列表
		{
			printf("\n%d want to see friend list\n",my_id);
			stpcpy(data.message,"你的好友列表：\n");
			char query_s[1024] = "select distinct friend from friend where host='";
			char myider[11];
			sprintf(myider, "%d",my_id);
			strncat(query_s,myider,100);
			strncat(query_s,"';",10);
			rc = mysql_real_query(&mysql, query_s, strlen(query_s));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
    }
    res = mysql_store_result(&mysql);
    rows = mysql_num_rows(res);
    printf("The total rows is: %d\n", rows);
    fields = mysql_num_fields(res);
    printf("The total fields is: %d\n", fields);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
    }
	
    while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
		   strncat(data.message,row[i],100);
		   strncat(data.message,"\n",6);
        }
    }
			//最后发送
			writen(sockfd, &data, sizeof(DATA));
		}
		
		else if(strcmp(data.com, "make_group") == 0)                 //创建群聊
		{
		    char myid1[11];
			sprintf(myid1, "%d", my_id);
		    char query_strr1[1024] = "INSERT INTO `user_group` (`host`, `group_name`, `make_time`) VALUES ('";
            strncat(query_strr1,myid1,100);
            strncat(query_strr1,"','",10);
            strncat(query_strr1,data.message,100);
            strncat(query_strr1,"','",10);
		    strncat(query_strr1,cur_time,100);
            strncat(query_strr1,"');",10);
            rc = mysql_real_query(&mysql, query_strr1, strlen(query_strr1));
            if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            //return -1;
            }
            res = mysql_store_result(&mysql);
		    strcpy(data.com, "make_group");
			strcat(data.message,"群聊创建成功！");
			writen(sockfd, &data, sizeof(DATA));
		}
		
		else if(strcmp(data.com, "add_group") == 0)      //添加群聊
		{
			char myid2[11];
			sprintf(myid2, "%d", my_id);
		    char query_strr2[1024] = "INSERT INTO `user_group_number` (`number`, `group_name`,`add_time`) VALUES ('";
            strncat(query_strr2,myid2,100);
            strncat(query_strr2,"','",10);
            strncat(query_strr2,data.message,100);
            strncat(query_strr2,"','",10);
		    strncat(query_strr2,cur_time,100);
            strncat(query_strr2,"');",10);
            rc = mysql_real_query(&mysql, query_strr2, strlen(query_strr2));
            if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            //return -1;
            }
            res = mysql_store_result(&mysql);
		    strcpy(data.com, "make_group");
			strcat(data.message,"成功加入群聊！");
			writen(sockfd, &data, sizeof(DATA));
			
		}
		
		else if(strcmp(data.com, "send_group") == 0)      //向群聊发送消息
		{
			char myid3[11];
			sprintf(myid3, "%d", my_id);
		    char query_strr3[2048] = "INSERT INTO `group_message` (`host`, `message`,`who_send`,send_time) VALUES ('";
			char host_id1[10];
			sprintf(host_id1, "%d", data.id);
            strncat(query_strr3,host_id1,100);
            strncat(query_strr3,"','",10);
            strncat(query_strr3,data.message,100);
            strncat(query_strr3,"','",10);
			strncat(query_strr3,myid3,100);
            strncat(query_strr3,"','",10);
		    strncat(query_strr3,cur_time,100);
            strncat(query_strr3,"');",10);
            rc = mysql_real_query(&mysql, query_strr3, strlen(query_strr3));
            if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            //return -1;
            }
            res = mysql_store_result(&mysql);
			//
		    strcpy(data.com, "send_group");
			strcat(data.message,"成功发送消息！");
			writen(sockfd, &data, sizeof(DATA));
			
		}
		
		else if(strcmp(data.com, "look_group") == 0)      //查看群消息
		{
			printf("\n%d want to see group message\n",my_id);
			stpcpy(data.message,"当前群最近消息为：\n");
			char query_s4[1024] = "select * from group_message where host='";
			char host_idd[11];
			sprintf(host_idd, "%d",data.id);
			strncat(query_s4,host_idd,100);
			strncat(query_s4,"';",10);
			rc = mysql_real_query(&mysql, query_s4, strlen(query_s4));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
    }
    res = mysql_store_result(&mysql);
    rows = mysql_num_rows(res);
    printf("The total rows is: %d\n", rows);
    fields = mysql_num_fields(res);
    printf("The total fields is: %d\n", fields);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
    }
	
    while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
		   strncat(data.message,row[i],100);
		   strncat(data.message,"\t",6);
        }
		strncat(data.message,"\n",6);
    }
			//最后发送
			writen(sockfd, &data, sizeof(DATA));
		}
		
		else 
		{
			printf("The client sends an unknown message and ignores it.\n");
			continue;
		}
	}
	
	save_data(head);  //保存数据到链表
	close(sockfd);    //关闭连接套接字
	return(NULL);
}




//读取客户端消息函数，替代系统函数read
//输入套接字、字符串、长度，返回字符串长度							*
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if(errno == EINTR)
				nread = 0;        //and call read() again
			else
				return(-1);
		}
		else if(nread == 0)       //EOF
			break;
		nleft	-= nread;
		ptr 	+= nread;
	}
	return(n-nleft);             //return >= 0
}


//发送消息给客户端函数，替代系统函数write						
//输入套接字、字符串、长度，返回字符串长度	
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t	nleft; 
	ssize_t	nwritten;
	const char	*ptr;

	ptr = vptr; 
	nleft = n;

	while(nleft > 0)
	{
		if((nwritten = write(fd, ptr,nleft)) <= 0)
		{
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

