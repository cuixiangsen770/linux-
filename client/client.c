#include"client.h"
#include<unistd.h>
//函数

//功能函数
//readn 读服务器
//writen 发服务器
//mygets 输入字符串
//press_any_one 暂停

//菜单函数
//show_menu_first 一级菜单
//show_menu_second 二级菜单
//sel 一级菜单处理选择
//next_select二级菜单处理选择

//一级菜单
//login 登陆
//regist注册
//quit 注销

//二级菜单
//view_online_id 查看当前在线用户

//send_pri_mes 发送私聊消息
//private_chat 私聊选择
//send_group_mes 发送群聊消息
//group_chat 群聊二级函数

//view_chat_history 查看聊天记录菜单函数
//view_private_chat_history 私聊记录
//view_group_chat_history 群聊记录

int main(int argc, char **argv)
{
	struct sockaddr_in	servaddr;
	char *serverport="9527";
    //创建套接字
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("套接字创建失败\n");
		return 0;
	}
    //套接字初始化
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("212.64.122.38");
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, serverport, &servaddr.sin_addr);
    //连接
	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		printf("connect连接失败\n");
		return 0;
	}

    //正式开始
	//write
	pthread_create(&tid, NULL, sel, NULL);//创建一个线程调用一级界面接收
	/*
	第一个参数为指向线程标识符的指针。

　　第二个参数用来设置线程属性。

　　第三个参数是线程运行函数的地址。

　　最后一个参数是运行函数的参数。
	*/

	MESSAGE mes;     //用于接收服务器结构体消息:协议、id、消息、name_id
	
	//read
	while(readn(sockfd, &mes, sizeof(MESSAGE)) > 0)
	{
		if(strcmp(mes.message, "login success") == 0)                   //登录成功
		{
			my_id = mes.id;                                             //保存自己的连接套接字到全局变量
			
			pthread_create(&tid, NULL, next_select, &my_id);            //进入第二层菜单
		}
		
		else if(strcmp(mes.message, "login faile") == 0)                //登录失败
		{
			system("clear");
			printf("\n用户名或者密码错误！\n");
			press_any_one();
			pthread_create(&tid, NULL, sel, NULL);                      //重新返回到登录界面
		}
		
		else if(strcmp(mes.message, "the id has already logined") == 0) //id已经登录
		{
			system("clear");
			printf("\n当前用户id:%d已经登陆，无需重新登陆！\n", mes.id);
			exit(0);
		}
		
		else if(strcmp(mes.message, "regist success") == 0)             //注册成功
		{
			my_id = mes.id;
			
			pthread_create(&tid, NULL, next_select, &my_id);            //进入第二层菜单  
		}
		
		else if(strcmp(mes.message, "regist faile") == 0)               //注册失败
		{
			system("clear");
			printf("\n你要注册的用户id已经有用户注册过了，请换一个\n");
			
			press_any_one();
			pthread_create(&tid, NULL, sel, NULL);
		}
		
		else if(strcmp(mes.message, "private_chat success") == 0)       //私聊连接成功
		{
			system("clear");
			printf("\n私人聊天通道建立成功，开始聊天吧~\n");
			printf("请输入\"exit\" 如果你想退出私人聊天的话\n\n");
			printf("我方id： %d\t\t\t\t聊天对象id: %d \n", my_id, chat_id);

			pthread_create(&tid, NULL, send_pri_mes, &mes.id);          //私聊发送消息
		}
		
		else if(strcmp(mes.message, "private_chat faile") == 0)         //私聊连接失败
		{
			system("clear");
			printf("\n你要聊天的对象不存在或者目前不在线，消息已保存为离线消息\n");
			printf("\n聊天对象下次来可以在历史消息中读到您发送给他的消息\n");
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);                
		}
		
		else if(strcmp(mes.com, "pri_data") == 0)                       //接收私聊信息并显示
		{
			char my_id_str[10];
			char chat_id_str[10];
			char file_name[20];
	
			sprintf(my_id_str, "%d", my_id);
			sprintf(chat_id_str, "%d", chat_id);
	
			strcpy(file_name, "data/");
			strcat(file_name, my_id_str);
			strcat(file_name, "_with_");
			strcat(file_name, chat_id_str);
	
			int fd;
			fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
			if(fd == -1)
			{
				perror("打开聊天记录文件失败");
				exit(0);
			}
			
			ticks = time(NULL); 
			sprintf(time_mes, "%s", ctime(&ticks));

			strcpy(save_message, time_mes);
			strcat(save_message, chat_id_str);
			strcat(save_message, " 说: ");
			strcat(save_message, mes.message);
			strcat(save_message, "\n");

			writen(fd, save_message, 100);                               //保存发过来的私聊消息 
			
			if(strcmp(mes.message, "exit") == 0)                        //如果对方发送exit则退出 
			{
				printf("\n\n%s", time_mes);                             //显示时间和发送过来的消息 
				printf("聊天对象id:%d 离开了私人聊天", chat_id);
				printf("\033[0m");
				printf("\n");
				
				goto pass1;                                             //退出时不显示对方发送过来的"exit"消息
			}
			
			printf("\t\t\t\t%s", time_mes);                             //显示时间和发送过来的消息
			printf("\t\t\t\t\033[47;30m%s\033[0m", mes.message);
			printf("\n\n");
			pass1:
				close(fd);   //attention!
		}
		
		else if(strcmp(mes.message, "group chat success") == 0)         //群聊连接成功
		{
			system("clear");
			printf("\n群聊通道建立，请开始聊天吧~\n");
			printf("请输入\"exit\" 如果你想退出私人聊天的话\n\n");

			pthread_create(&tid, NULL, send_group_mes, &my_id);         //群聊发送消息
		}
		
		else if(strcmp(mes.com, "group_message") == 0)                  //接收群聊信息并显示
		{
			char my_id_str[10];
			char file_name[50];
			char id_who_sended[10];

			sprintf(my_id_str, "%d", my_id);
			sprintf(id_who_sended, "%d", mes.id_name);
	
			strcpy(file_name, "data/");
			strcat(file_name, my_id_str);
			strcat(file_name, "_group_chat_history");
	
			int fd;
			fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
			if(fd == -1)
			{
				perror("打开聊天记录文件失败");
				exit(0);
			}
			
			ticks = time(NULL); 
			sprintf(time_mes, "%s", ctime(&ticks));

			strcpy(save_message, time_mes);
			strcat(save_message, id_who_sended);
			strcat(save_message, " 说: ");
			strcat(save_message, mes.message);
			strcat(save_message, "\n");

			writen(fd, save_message, 100);                               //保存群聊消息
			
			if(strcmp(mes.message, "exit") == 0)                        //如果对方发送exit则退出 
			{
				printf("%s", time_mes);     
				printf("用户id:%d 离开了群聊", mes.id_name);    
				printf("\n\n");
				
				goto pass;                                              //退出时不显示所发送的"exit"
			}
			
			if(mes.id_name == my_id)
			{
				goto pass;                                              //如果消息是自己发送的就不显示
			}
			
			printf("\t\t\t\t%s", time_mes);                             //显示群聊消息
			printf("\t\t\t\t用户id:%d 说: ", mes.id_name);   
			printf("\033[47;30m%s\033[0m\n\n",mes.message);
			
			pass:
				close(fd); //attention!
		}

		else if(strcmp(mes.com, "view_online_id") == 0)                 //接收并显示在线用户列表
		{
			system("clear");
			printf("%s", mes.message);
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
		}

		else if(strcmp(mes.com, "user_online") == 0)                    //显示其他用户上线消息
		{
			printf("\n\033[47;31m%s\033[0m\033[3A\n", mes.message);
			printf("\033[0m");
		}
		
		else if(strcmp(mes.com, "look_file") == 0)                    //查看文件
		{
			printf("\n\n%s\n", mes.message);
			down_file();
			
		}
		else if(strcmp(mes.com, "rev_file") == 0)                    //接受文件
		{
			printf("\n\n%s\n", mes.message); 
		}
		
		else if(strcmp(mes.com, "new_friend_none") == 0)                    //新好友不存在
		{
			printf("\n\n%s\n", mes.message); 
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
		}
		
		
		else if(strcmp(mes.com, "new_friend_request") == 0)                    //新好友申请
		{
			printf("\n\n%s\n请选择：1.同意\t2.拒绝\n请输入你的选择：", mes.message); 
			int a;
			scanf("%d",&a);
			while(getchar() != '\n');
			if(a==1){
				printf("你已同意添加%d为好友",mes.id_name);
				press_any_one();
			    pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
			}
			else{
				printf("你已拒绝添加%d为好友",mes.id_name);
				press_any_one();
			    pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
			}
			
		}
		
		else
		{
			continue;
		}
	}
	
	exit(0);
}





//1.发送私聊消息
void *send_pri_mes(void *arg)
{
	int		 send_sockfd = *(int *)arg;
	char	 my_id_str[10];
	char	 chat_id_str[10];
	char	 file_name[20];
	MESSAGE  pri_data;
	
	sprintf(my_id_str, "%d", my_id);
	sprintf(chat_id_str, "%d", chat_id);
	
	strcpy(file_name, "data/");
	strcat(file_name, my_id_str);
	strcat(file_name, "_with_");
	strcat(file_name, chat_id_str);
	
	int fd;
	fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0664);
	if(fd == -1)
	{
		perror("打开聊天记录文件失败");
		exit(0);
	}

	while(1)
	{
		printf("\n");
		strcpy(pri_data.com, "pri_data");
		pri_data.id = send_sockfd;
		
		mygets(pri_data.message, MAXLINE);
		ticks = time(NULL); 
		sprintf(time_mes, "%s", ctime(&ticks));
		printf("%s", time_mes);	

		strcpy(save_message, time_mes);
		strcat(save_message, "我说:");
		strcat(save_message, pri_data.message);

		writen(fd, save_message, 100);                                   //保存发过去的私聊消息
		
		if(strcmp(pri_data.message, "exit") == 0)
		{
			system("clear");
			printf("\n你退出了私人聊天\n");
			writen(sockfd, &pri_data, sizeof(MESSAGE));
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单
			
			goto pass;
		}
	
		writen(sockfd, &pri_data, sizeof(MESSAGE));
	}
	
	pass:
		close(fd);   //attention

	return (NULL);
}


//2.群聊发送消息函数  传入参数：客户端id
void *send_group_mes(void *arg)
{
	int my_id = *(int *)arg;
	
	MESSAGE group_data;
	
	while(1)
	{
		printf("\n");
		strcpy(group_data.com, "group_message");
		
		mygets(group_data.message, MAXLINE);                            //输入私聊消息
		
		ticks = time(NULL); 
		sprintf(time_mes, "%s", ctime(&ticks));
		printf("%s", time_mes);	

		if(strcmp(group_data.message, "exit") == 0)
		{
			system("clear");
			printf("\n你退出了当前群聊\n");
			writen(sockfd, &group_data, sizeof(MESSAGE));
			
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
			
			goto pass;	
		}
	
		writen(sockfd, &group_data, sizeof(MESSAGE));
	}
	
	pass:
		printf("欢迎回来~^_^\n");
	
	return (NULL);
}


//3.第二级选择线程函数，进行功能选择   						*
//输入参数：客户端id  	
void *next_select(void *arg)
{
	int local_my_id = *(int *)arg;
	
	char select = '0';
	show_menu_second();
	select = tolower(getchar());

	while((select != '\n') && (getchar() != '\n')); 
	switch(select)
	{
		case '1':
			view_online_id();
			break;
		case '2':
			private_chat();
			break;
		case '3':
			group_chat();
			break;
		case '4':
			view_chat_history();
			break;
		case '5':
		    send_file();
			break;
		case 'l':
		    look_file();
			break;
		case 'f':
		    down_file();
			break;
		case '6':
			quit(local_my_id);
			system("clear");
			sleep(2);
			exit(0);
		/*"\n\n\t7、添加好友"
			"\n\n\t8、好友列表"
			"\n\n\t9、创建群聊"
			"\n\n\t10、添加群聊"
			"\n\n\t11、发送指定群聊消息"
			"\n\n\t12、查看群聊消息"
			"\n\n\t13、管理我创建的群聊"
			"\n\n\t14、给离线好友留言"
		*/
		case '7':
		add_new();
		    break;
		case '8':
		friend_list();
		    break;
		case '9':
		make_group();
		    break;
		case 'a':
		add_group();
		    break;
		case 'b':
		send_group();
		    break;
		case 'c':
		look_group();
		    break;
		case 'd':
		man_group();
		    break;
		case 'e':
		    send_outline();
		    break;
		default:
			system("clear");  
			printf("\n输入错误！\n");
			press_any_one();
			pthread_create(&tid, NULL, next_select, &my_id);            //返回到第二层菜单  
	}

	return (NULL);
}




//4.查看在线用户			
void view_online_id()
{
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
}



//5.私聊选择函数							
//向服务器发送私聊协议				
void private_chat()
{
	system("clear");

	MESSAGE private_chat_data;
	strcpy(private_chat_data.com, "private_chat");

	printf("请输入你想聊天的对象id:\n");
	scanf("%d", &private_chat_data.id);
	getchar();
	
	if(private_chat_data.id < 1 || private_chat_data.id > 9999)
	{
		system("clear");
		printf("输入错误！\n");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
		
		goto pass;
	}

	chat_id = private_chat_data.id;                                     //保存私聊对象的id，用于创建消息记录文件名

	writen(sockfd, &private_chat_data, sizeof(MESSAGE));
	
	pass:
		;
}



//6.群聊选择
void group_chat()
{
	MESSAGE group_chat_data;
	
	strcpy(group_chat_data.com, "group_chat");

	writen(sockfd, &group_chat_data, sizeof(MESSAGE));
}



//7.查看聊天记录：进行私聊、群聊消息选择				*
void view_chat_history()
{
	int id_or_group_chat;
	
	system("clear");
	printf("\n请输入你要查看和哪个用户的聊天记录,\n或者输入要查看消息记录的群号:\n");
	scanf("%d", &id_or_group_chat);
	while(getchar() != '\n');
	
	if(id_or_group_chat < 0 || id_or_group_chat > 9999)
	{
		system("clear");
		printf("\nInput error.");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
	}

	else if(id_or_group_chat == 0)
	{
		view_group_chat_history();
	}
	
	else if(id_or_group_chat < 9999 && id_or_group_chat != 0)
	{
		view_private_chat_history(id_or_group_chat);
	}
}


//8.私聊记录
void view_private_chat_history(int id_or_group_chat)
{
	char my_id_str[10];
	char chat_id_str[10];
	char find_file_name[20];

	sprintf(my_id_str, "%d", my_id);
	sprintf(chat_id_str, "%d", id_or_group_chat);

	strcpy(find_file_name, my_id_str);
	strcat(find_file_name, "_with_");
	strcat(find_file_name, chat_id_str);
	
	char *dir_name = "data";
	DIR *dir;
	dir = opendir(dir_name);
	if(dir == NULL)
	{
		perror("open data");
		exit(0);
	}

	struct dirent *tmp;
	int exist = 0;
	while((tmp = readdir(dir)) != NULL)
	{
		if(strcmp(tmp->d_name, find_file_name) == 0)
		{
			exist = 1;
			break;
		}
	}
	
	if(exist)
	{
		char find_file_name_with_dir[20];
		
		strcpy(find_file_name_with_dir, "data/");
		strcat(find_file_name_with_dir, find_file_name);
		
		int fd;
		fd = open(find_file_name_with_dir, O_RDONLY);
		if(fd == -1)
		{
			perror("open");
			exit(0);
		}
		
		system("clear");
		char read_message[100];
		while(read(fd, read_message, 100) > 0)
		{
			printf("%s\n", read_message);
		}
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);               //返回到第二层菜单  
		
		goto pass;
	}

	else
	{
		system("clear");
		printf("\n没有与用户 %d 的聊天记录\n", id_or_group_chat);
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
	
		goto pass;
	}

	pass:
		printf("按回车继续 ^_^\n");
}



//9.群聊记录
void view_group_chat_history()
{
	char my_id_str[10];
	char find_file_name[50];

	sprintf(my_id_str, "%d", my_id);
	
	strcpy(find_file_name, my_id_str);
	strcat(find_file_name, "_group_chat_history");
	
	char find_file_name_with_dir[50];                                   
	strcpy(find_file_name_with_dir, "data/");
	strcat(find_file_name_with_dir, find_file_name);
	
	int fd;
	fd = open(find_file_name_with_dir, O_RDONLY);
	if(fd != -1)                                 //可以不要像私聊那样打开目录再寻找文件是否存在，这里直接打开文件，判断文件是否存在
	{
		system("clear");
		
		char read_message[100];
		while(read(fd, read_message, 100) > 0)
		{
			printf("%s\n", read_message);
		}
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
		
		goto pass;
	}

	else
	{
		system("clear");
		printf("\n没有该群或者该群暂无消息记录\n");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);               
		
		goto pass;
	}
	
	pass:
		printf("按任意键继续^_^\n");
}


//10.注销，向服务器发送退出协议	
//输入参数：客户端Id
void quit(int my_id)
{
	MESSAGE quit_data;
	
	strcpy(quit_data.com, "exit");
	quit_data.id = my_id;

	writen(sockfd, &quit_data, sizeof(MESSAGE));
}



//11.登录界面函数，进行登录、注册
void *sel(void *arg)
{
	char select = 0;
	show_menu_first();
	select = tolower(getchar());
	while((select != '\n') && (getchar() != '\n'));  
	switch(select)
	{
		case '1':
			login();
			break;
		case '2':
			regist();
			break;
		case '3':
			system("clear");
			exit(0);
		default:
			system("clear");  
			printf("\n\t输入错误!\n");
			
			press_any_one();	
			pthread_create(&tid, NULL, sel, NULL);
	}
	return(NULL);
}



//12.登录							*
void login()
{
	system("clear");

	MESSAGE login_data;
	strcpy(login_data.com, "login");

	printf("请输入你的用户id:");
	scanf("%d", &login_data.id);
	while(getchar() != '\n');
	
	if(login_data.id < 1 || login_data.id > 9999)
	{
		system("clear");
		printf("\n输入错误\n");
		exit(0);
	}

	strcpy(login_data.message, getpass("请输入你的用户密码:"));

	writen(sockfd, &login_data, sizeof(MESSAGE));
}



//13.注册
void regist()
{
	system("clear");

	MESSAGE regist_data;
	strcpy(regist_data.com, "regist");

	printf("\n请输入你想要注册的用户id:");
	scanf("%d", &regist_data.id);
	while(getchar() != '\n');

	if(regist_data.id < 1 || regist_data.id > 9999)
	{
		system("clear");
		printf("\n用户id在1到9999之间，你的输入有误\n");
		exit(0);
	}
	
	char input_passwd_first[MAXLINE];
	char input_passwd_second[MAXLINE];

	strcpy(input_passwd_first, getpass("请输入你的密码:"));
	strcpy(input_passwd_second, getpass("请再次输入你的密码:"));

	if(strcmp(input_passwd_first, input_passwd_second) != 0)
	{
		system("clear");
		printf("\n两次输入的代码不一致，注册用户失败\n");
		exit(0);
	}
	
	
	
	else
	{
		strcpy(regist_data.message, input_passwd_second);
	}

	writen(sockfd, &regist_data, sizeof(MESSAGE));
}


//14.输入字符串函数，替代系统函数gets						*
void mygets(char buf[], int len)
{
	int i = 0;
	
	for(i = 0; i < len-1; i++)  
	{
		buf[i] = getchar();
		
		if(buf[i] == '\n')
		{
			break;
		}
	}
	
	if(buf[i] != '\n')
	{
		while(getchar() != '\n');
	}
	
	buf[i] = '\0';
}


//15.暂停
void press_any_one()
{
	printf("\n按任意键继续\n");
	getchar();
}



//16.菜单
void show_menu_first()
{
	system("clear");
	printf(
			"\n\n\t  欢迎使用三人组Linux聊天系统"
			"\n\n\t1、用户登陆"
			"\n\n\t2、新用户注册"
			"\n\n\t3、退出系统"
			"\n\n\t请输入您的选择:\n"
	  		);
}




//17.二级菜单
void show_menu_second()
{
	system("clear");
	printf(
			"\n"
			"\n\n\t  ---欢迎使用三人组Linux聊天系统"
			"\n\n\t1、查看当前在线用户"
			"\t2、私聊消息"
			"\n\n\t3、群聊消息"
			"\t4、查看消息记录"
			"\n\n\t5、文件上传"
			"\tf、文件下载"
			"\tl、文件列表"
			"\n\n\t6、退出登陆"
			"\n\n\t以下是优化程序后的功能"
			"\n\n\t7、添加好友"
			"\t8、好友列表"
			"\n\n\t9、创建群聊"
			"\ta、添加群聊"
			"\n\n\tb、发送指定群聊消息"
			"\tc、查看群聊消息"
			"\n\n\td、管理我创建的群聊"
			"\te、给离线好友留言"
			"\n\n\t请输入你的选择：\n"
	  		);
}


//18.读取服务器消息函数，替代系统函数read
//输入参数：套接字、字符串、长度					*	
//返回值  ：字符串长度							
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




//19.发送消息给服务器函数，替代系统函数write						*
//输入参数：套接字、字符串、长度					*	
//返回值  ：字符串长度							*
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



//20
/*
void  send_file(){
	char fileaddr[1024];
    MESSAGE file_data;
	strcpy(file_data.com, "send_file");
	file_data.id=my_id;
	printf("请输入你想传送文件的用户id:\n");
	scanf("%d", &file_data.id_name);
	printf("请输入你想传送文件的位置:\n");
	scanf("%s", &fileaddr);
    getchar();
	FILE *fq;
	if(fq = fopen("~/Desktop/linuxV1.2/client/client/client.c","rb")==NULL){
	printf("文件未找到!\n");
	press_any_one();
	system("clear");
	pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
	}
	else{
   //char   bufferr[4096];
	//bzero(bufferr,sizeof(bufferr));
    while(!feof(fq)){
    int len = fread(file_data.message, 1, sizeof(file_data.message), fq);
	}
	if(file_data.id_name < 1 || file_data.id_name > 9999)
	{
		system("clear");
		printf("输入错误！\n");
		
		press_any_one();
		pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
		

	}
	else{
	writen(sockfd, &file_data, sizeof(MESSAGE));
	printf("文件已发送");
	press_any_one();
	system("clear");
	pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
    }
	
	
	}
	
}
*/
void send_file(){
	char fileaddr[256];
	printf("请输入你想传送文件的位置:\n");
	scanf("%s", &fileaddr);
	char cmd[1024];
	stpcpy(cmd,"scp ");
	strcat(cmd, fileaddr);
	strcat(cmd, " ubuntu@212.64.122.38:/opt/lampp/htdocs/data");
	//printf("%s",cmd);
	system(cmd);
	press_any_one();
	system("clear");
	pthread_create(&tid, NULL, next_select, &my_id);                //返回到第二层菜单  
}

void look_file(){
	MESSAGE view_online_id_data;
	strcpy(view_online_id_data.com, "look_file");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	//system("scp -P ubuntu@212.64.122.38:/opt/lampp/htdocs/data");
	//getchar();
	//down_file();

}

void down_file(){
	char fileaddr[256];
	printf("\n请输入你想获得的文件名称:\n");
	scanf("%s", &fileaddr);
	char cmd[1024];
	stpcpy(cmd,"wget 212.64.122.38/data/");
	strcat(cmd, fileaddr);
	system(cmd);
	getchar();
	press_any_one();
	system("clear");
	pthread_create(&tid, NULL, next_select, &my_id);   //返回二级菜单
	
}


void add_new(){
	MESSAGE new_friend;
	strcpy(new_friend.com, "new_friend");
	
	printf("请输入你想添加好友的id:");
	
	int f_id;
	scanf("%d", &f_id);
	while(getchar() != '\n');
	if(f_id < 1 || f_id > 9999)
	{
		system("clear");
		printf("\n用户id在1到9999之间，你的输入有误\n");
		exit(0);
	}
	new_friend.id=f_id;
	new_friend.id_name=my_id;
	writen(sockfd, &new_friend, sizeof(MESSAGE));
	printf("\n好友请求已发送，请耐心等待\n");
	press_any_one();
	system("clear");
	pthread_create(&tid, NULL, next_select, &my_id);   //返回二级菜单
}


void friend_list(){
	MESSAGE view_online_id_data;
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
}

void make_group(){
	MESSAGE view_online_id_data;
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
	
}


void add_group(){
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
}

void send_group(){
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
}


void look_group(){
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
			
			
}


void man_group(){
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
	
}


void send_outline(){
	MESSAGE view_online_id_data;
	
	strcpy(view_online_id_data.com, "view_online_id");
	writen(sockfd, &view_online_id_data, sizeof(MESSAGE));
	
	
}































































