/*
	[file]
	@author:	REmaxer
	@last_mod:	07/02/13
	@lang:		C
	@desc:		chatty is a simple socket-chat in C
	@vers:		0.1
*/
/*
	###  # #  ###  ### ### # #
	#    ###  #-#   #   #   #
	###  # #  # #   #   #   #

*/
#ifndef _CHATTY_H
#define _CHATTY_H	1

#include <netinet/in.h>

#define	CHATTY_MAX_USERS	10

#define	CHATTY_PORT		3000


/*
 	[function]
	@desc:		initialize chatty system
	@ret:		1 on success , 0 else
*/
extern int chatty_init();

/*
 	[function]
	@desc:		print a sockaddr_in struct in a human-readable format
	@arg0:		struct sockaddr_in to print
*/
extern void print_addr(struct sockaddr_in *);

/*
	[function]
	@desc:		manage chatty socket
	@arg0:		chatty socket descriptor returned by chatty_init()
*/
extern void chatty_manag(int);

/*
	[function]
	<internal>
	@desc:	send hello message to a connected user
	@arg0:	user socket
	@arg1:	users online
*/
static void chatty_say_welcome(int,int);

/*
	[function]
	<internal>
	@desc:	send message
	@arg0: user socket
	@arg1: message
	@arg2: current user id
	@arg3: buffer len
*/
static void chatty_send_message(int,char *,int);

/*
	[function]
	@desc:	send a broadcast message
	@arg0: socket descriptors array
	@arg1: message
	@arg2: buffer len
*/
extern void chatty_broadcast_message(int *,char *,int);

/*
	[function]
	@desc: send a server message
	@arg0: socket descriptors array
	@arg1:message
	@arg2:buffer len
*/
extern void chatty_server_message(int *,char *,int);

/*
	[function]
	@desc: send an user message
	@arg0: socket descriptors array
	@arg1: sender
	@arg2: message 
	@arg3: buffer len
*/
extern void chatty_user_message(int *,int,char *,int);

/*
	[function]
	<internal>
	@desc:	write users list on file
	@ret:	1 in success , 0 else
	@arg0:	filename
	@arg1:	pointer to socket descriptors array
	@arg2:	users num
*/
static int chatty_save_users_on_file(char *,int *,int);

/*
	[function]
	<internal>
	@desc:	get users list from file
	@ret:	1 in success, 0 else
	@arg0: filename
	@arg1: pointer to existing socket descriptors array
*/
static int chatty_load_users_from_file(char *,int *);

/*
	[function]
	<internal>
	@desc: handle a user connection
	@ret:	1 in success , 0 else
	@arg0: user socket descriptor pointer
*/
extern int chatty_handle_user(int *);

/*
 	[function]
	<internal>
	@desc:	get users currently online
	@ret:	users online, 0 else
	@arg0:	filename
*/
static int chatty_get_users_online(char *);


/*
	[function]
	<internal>
	@desc:  handle user message
	@arg0: user socket
*/
static void chatty_handle_message(int);

/*
	[function]
	@desc:	handle incoming connections
	@ret:	Thread ID on success, 0 else
	@arg0:	user socket descriptor
*/
static int chatty_handle_conn(int);

/*
	[function]
	@desc:	add user to file
	@ret:	1 on success , 0 else
	@arg0:  filename
	@arg1:  user socket
*/
extern int chatty_add_user(char *,int);

/*
	[function]
	@desc:	zero file
	@ret:	1 on success, 0 else
	@arg0:	filename
*/
static int chatty_zero_file(char *);

/*
	[function]
	@desc: delete an user from file
	@ret:	1 on success , 0 else
	@arg0:	filename
	@arg1:	user socket
*/
extern int chatty_delete_user(char *,int);

#endif	/* chatty.h */
