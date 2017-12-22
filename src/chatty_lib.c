/*
	[file]
	@author:	REmaxer
	@last_mod:	07/02/13
	@lang:		C
	@desc:		chatty library
	@vers:		0.1
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include "../include/chatty.h"

#define USERS_FILE	"users"

/* chatty_save_users_on_file implementation */
static int chatty_save_users_on_file(char *filename,int *u_sock_arr,int us_n){
	FILE *fd;

	if( (fd = fopen(filename,"w+b")) == NULL)
		return 0;
	fwrite(u_sock_arr,sizeof(int),us_n,fd);
	fflush(fd);
	fclose(fd);
	return 1;
}

/* chatty_load_users_on_file implementation */
static int chatty_load_users_from_file(char *filename,int *u_sock_arr){
	FILE *fd;
	int supp;

	if( (fd = fopen(filename,"r")) == NULL)
		return 0;
	while(fread(&supp,sizeof(int),1,fd) > 0){
		if(supp != 0){
			*u_sock_arr = supp;
			u_sock_arr++;
		}
	}
	fclose(fd);
	return 1;
}

/* chatty_get_users_online implementation */
static int chatty_get_users_online(char *filename){
	FILE *fd;
	int users;
	int supp;

	users = 0;
	if( (fd = fopen(filename,"r")) == NULL)
			return 0;
	while( fread(&supp,sizeof(int),1,fd) > 0 ){
		if(supp != 0){
			users++;
		}
	}
	fclose(fd);
	return users;
}

/* chatty init implementation */
int chatty_init(){
	const int back_log = 10;
	int sockfd;
	struct sockaddr_in serv_addr;
	
	/* create socket descriptor */
	if( (sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))  < 0)
		return 0;
	
	/* set zero serv_addr struct */
	memset(&serv_addr,0,sizeof(struct sockaddr_in));
	/* set parameters serv_addr struct */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(CHATTY_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind new socket */
	if( bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0)
		return 0;

	/* accept connections from new socket */
	if( listen(sockfd,back_log) < 0)
		return 0;
	
	/* call chatty_manag function */
	chatty_manag(sockfd);
	close(sockfd);
	return 1;
}

/* chatty_handle_conn implementation */
static int chatty_handle_conn(int u_sock){
	int f;
	/* child thread */
	if((f = clone(chatty_handle_user,(void *)malloc(65536),CLONE_FILES,&u_sock)) < 0)	
		return 0;
	else
		return f;
}

/* chatty_handle_message implementation */
static void chatty_handle_message(int u_sock){
	int brcvd;
	const int buff_size = 1000;
	char buffer[buff_size];
	int u_sock_arr[CHATTY_MAX_USERS];
	char user_conn[30];
	
	/* loading users list from file */
	if((chatty_load_users_from_file(USERS_FILE,&u_sock_arr[0])) == 0)
		exit(-1);
	/* send server message */
	sprintf(user_conn,"<user%02d> connected\n",u_sock);
	chatty_server_message(&u_sock_arr[0],user_conn,strlen(user_conn));
	/* receive message */
	if( (brcvd = recv(u_sock,buffer,buff_size,0)) < 0)
			exit(-1);
	
	while(brcvd > 0){
		/* loading users list from file */
		if((chatty_load_users_from_file(USERS_FILE,&u_sock_arr[0])) == 0)
				exit(-1);
		/* send user message */
		chatty_user_message(&u_sock_arr[0],u_sock,buffer,brcvd);
		/* continue to receving bytes */
		brcvd = recv(u_sock,buffer,buff_size,0);
	}
	/* delete user from file */
	if(!chatty_delete_user(USERS_FILE,u_sock))
		exit(-1);
	/* loading users list from file */
	if((chatty_load_users_from_file(USERS_FILE,&u_sock_arr[0])) == 0)
				exit(-1);
	/* send server message */
	sprintf(user_conn,"<user%02d> disconnected\n",u_sock);
	chatty_server_message(&u_sock_arr[0],user_conn,strlen(user_conn));
	return;
}


/* chatty_manag implementation */
void chatty_manag(int sockfd){
	int sockaddr_len;
	struct sockaddr_in u_addr;
	int u_sock;

	sockaddr_len = sizeof(struct sockaddr);
	/* zero file */
	if( (chatty_zero_file(USERS_FILE)) == 0){
		// handling error
	}
	while(1){
		if( (u_sock = accept(sockfd,(struct sockaddr *)&u_addr,&sockaddr_len)) < 0){
			/* next user */
			continue;
		}
		/* print current user addr  */
		print_addr(&u_addr);
		/* adding new user to file */
		if(! (chatty_add_user(USERS_FILE,u_sock)) ){
			continue;
		}
		/* handling connection request */
		chatty_handle_conn(u_sock);
	}
	return;
}

/* chatty_handle_user implementation */
int chatty_handle_user(int *u_sock){
	/* say hello */
	chatty_say_welcome(*u_sock,chatty_get_users_online(USERS_FILE));
	/* handle user messages */
	chatty_handle_message(*u_sock);
	/* closing user socket */
	close(*u_sock);
	return 1;
}

/* chatty_zero_file implementation */
static int chatty_zero_file(char *filename){
	FILE *fd;

	if((fd = fopen(filename,"w")) == NULL)
		return 0;
	fclose(fd);
	return 1;
}

/* chatty_add_user implementation */
int chatty_add_user(char *filename,int u_sock){
	FILE *fd;

	if( (fd = fopen(filename,"ab")) == NULL)
		return 0;
	fwrite(&u_sock,sizeof(int),1,fd);
	fflush(fd);
	fclose(fd);
	return 1;

}

/* chatty_delete_user implementation */
int chatty_delete_user(char *filename,int u_sock){
	FILE *fd;
	int supp;
	int val;

	val = 0;
	if( (fd = fopen(filename,"r+b")) == NULL)
		return 0;
	while(fread(&supp,sizeof(int),1,fd) > 0){
		if(supp == u_sock){
			fseek(fd,-4,SEEK_CUR);
			fwrite(&val,sizeof(int),1,fd);
			fclose(fd);
			return 1;
		}
	}
	fclose(fd);
	return 1;
}

/* chatty_broadcast_message implementation */
void chatty_broadcast_message(int *u_sock_arr,char *message,int buff_len){
	int i;
	for(i = 0;i < chatty_get_users_online(USERS_FILE);i++){
		chatty_send_message(*u_sock_arr,message,buff_len);
		u_sock_arr++;
	}
	return;
}
/* chatty_user_message implementation */
void chatty_user_message(int *u_sock_arr,int sender,char *message,int buff_len){
		char user_string[20];	

		/* send user string */
		sprintf(user_string,"[user%02d]:",sender);
		chatty_broadcast_message(&u_sock_arr[0],user_string,strlen(user_string));
		/* send real message */
		chatty_broadcast_message(u_sock_arr,message,buff_len);
		return;
}

/* chatty_server_message implementation */
void chatty_server_message(int *u_sock_arr,char *message,int buff_len){
	char *server_string = "[!Server]";
	/* send server string */
	chatty_broadcast_message(u_sock_arr,server_string,strlen(server_string));
	/* send real message */
	chatty_broadcast_message(u_sock_arr,message,buff_len);
	return;
}

/* chatty_send_message implementation */
static void chatty_send_message(int u_sock,char *message,int buff_len){
	send(u_sock,message,buff_len,0);
	return;
}

/* chatty_say_welcome implementation */
static void chatty_say_welcome(int u_sock,int i){
	char mess[]= "\n \
	###  # #  ###  ### ### # # \n \
	#    ###  #-#   #   #   #  \n \
	###  # #  # #   #   #   #  \n \
			      	   \n \
	##################### \n \
	# WELCOME TO CHATTY # \n \
	##################### \n \n \
	There are %d users online \n";
	sprintf(mess,mess,i);
	send(u_sock,mess,strlen(mess),0);
	return;
}

/* print_addr implementation */
void print_addr(struct sockaddr_in *addr){
	char addr_s[INET_ADDRSTRLEN];
	/* converting binary address in dotted-quad notation address */
	inet_ntop(AF_INET,&addr->sin_addr,addr_s,INET_ADDRSTRLEN);
	printf("%s:%2d\n",addr_s,ntohs(addr->sin_port));
	return;
}

