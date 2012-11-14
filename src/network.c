#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
//#include <signal.h>
#include <netdb.h>
//#include <sys/time.h>
/*#include <arpa/inet.h>*/
#include <fcntl.h>


#include "tetris.h"

#define LISTEN_PORT 10011
#define LISTEN_BACKLOG 1
#define START_POS 4

static const int MSG_LEN = sizeof(PlrState)+START_POS;
static const int SYNC_MSG_LEN = sizeof(Options)+20;


/* It is single socket for connection with your friend */
int sock; 
/* Listen socket for incoming connection */
int listen_sock;


int accept_connection()
{
	//socklen_t accept_len;
	//accept_len = sizeof(accept_name);
    /* accept connection */
    if((sock = accept(listen_sock,NULL,NULL/*(struct sockaddr *) (&accept_name), &accept_len*/)) == -1)
	{
		//printf("errno %d\n", errno);
		if (errno != EWOULDBLOCK)
		{
    		perror("accept");
	        return -1;
 		}
        return 1;
	}

	/* Close listen socket, it's doesn't need any more */
	if (close(listen_sock) == -1) 
    {
        perror("close");
        return -1;
    }  
    listen_sock = 0;
    
    /* Send sync. message  */
    char message[SYNC_MSG_LEN]; // buffer
    message[0] = 1; //version of tgp (tetris game protocol)
    //message[1] = 1; // type of message
      
    memcpy(&message[1], &options, sizeof(options));
    
    if(send(sock, message, SYNC_MSG_LEN, 0) == -1)
    {
        //perror("send");
        close_connection();
    	return -1;
    }
    
    /* Make Nonblocking socket */
 	if ( fcntl(sock, F_SETFL, O_NONBLOCK) == -1) 
    {
        perror("fcntl");
        return -1;
    }
	return 0;
}

/* Create server and wait for connection.
 * Return 0 if no errors. */
int create_game()
{
    struct sockaddr_in listen_name;
 	sock=0;
 	listen_sock = 0;
    /* creates a socket, protocol's family Internet, with connection */
    /* protocol TCP */
    if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        return -1;
    }

    /*fill data structure with socket address  */
    memset(&listen_name, 0, sizeof(listen_name));
    listen_name.sin_family = AF_INET;
    listen_name.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_name.sin_port = htons(LISTEN_PORT);
 
    /* bind socket address from upper structure listen_name */
    if (bind(listen_sock, (struct sockaddr *) (&listen_name), sizeof(listen_name)) == -1) 
    {
        perror("bind");
        return -1;
    }
 	

    /* engage listening of connection */
    /* maximum lenth of incoming queue - LISTEN_BACKLOG */
    if (listen(listen_sock, LISTEN_BACKLOG) == -1) 
    {
        perror("listen");
        return -1;
    }
    
    /*struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
 	if(getsockname(listen_sock,(struct sockaddr *) (&name),&namelen) ==-1)
 	{
        perror("getsockname");
        return -1;
    }
    char* out;
 	out = inet_ntoa(name.sin_addr);
    printf("Addr is %s\n",out);*/
        
    /* Make Nonblocking listener */
 	if ( fcntl(listen_sock, F_SETFL, O_NONBLOCK) == -1) 
    {
        perror("fcntl");
        return -1;
    }
	
	return 0;
}

/* recieving the first address from host name */
u_int32_t get_addr(const char *name)
{
    struct hostent *he;

    /*recieving host from text address */
    if((he = gethostbyname(name)) == NULL)
    {
        perror("gethostbyname");
		return -1;
    }

    /* checking, if at least one address is exists or not */
    if(he->h_addr_list[0] == NULL)
    {
        fprintf(stderr, "can't find any address for %s\n", name);
		return -1;
    }

    /* chesk of usage IPv4 addresses (32-bit) */
    if((he->h_addrtype != AF_INET) || (he->h_length != sizeof(u_int32_t)))
    {
        fprintf(stderr, "using not-IPv4 address\n");
		return -1;
    }

    /* return first address in host byte order */
    return ntohl(*((u_int32_t *)(he->h_addr_list[0])));
}

/* Connect your to host. "addr" contains name or ip of host. 
 * Return 0 if no errors.*/

int connect_to_friend(const char *addr)
{
	u_int32_t ip; // host ip
	sock = 0;
	if((ip = get_addr(addr)) == -1)
		return -1;

	struct sockaddr_in remote_name;
    
    /* far socket address */
    /* also using internet address */
    remote_name.sin_family      = AF_INET;
    remote_name.sin_addr.s_addr = /*htonl(INADDR_LOOPBACK);*/htonl(ip);
    remote_name.sin_port        = htons(LISTEN_PORT);

    /* create a socket */
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }

    if(connect(sock, (struct  sockaddr *)(&remote_name), sizeof(remote_name)) == -1)
    {
        perror("connect");
        return -1;
    }

	/* Receive sync. message */
	char message[SYNC_MSG_LEN]; // buffer
	int res;
    if ((res = recv(sock, message, SYNC_MSG_LEN, 0)) == -1) 
	{
		close_connection();
    	return -1;
	}
	if (message[0] != 1) 
	{
		printf("Wrong protocol version!");
		close_connection();
    	return -1;
	}
	memcpy(&options, &message[1], sizeof(options));
	
    /* Make Nonblocking socket */
 	if ( fcntl(sock, F_SETFL, O_NONBLOCK) == -1) 
    {
        perror("fcntl");
        return -1;
    }
	return 0;	
}

int close_connection()
{
	if(listen_sock > 0 && close(listen_sock) == -1)
    {
        perror("close");
        return -1;
    }
    //printf("=====1====\n");
    if(sock > 0 && close(sock) == -1)	
    {
        perror("close");
        return -1;
    }
    //printf("=====2====\n");
    listen_sock = sock = 0;
	return 0;
}

/* send message from one player to another 
 * Return 0 if no errors.*/
int send_message(gint8 rows, gint8 disconnect)
{
	if(sock<=0) return -2;
    char message[MSG_LEN]; // buffer
	//int i, j; // counters
    message[0] = 1; //version of tgp (tetris game protocol)
    message[1] = 0; // type of message
    message[2] = (char)rows;
    message[3] = (char)disconnect;
    
    memcpy(&message[START_POS], &player1, sizeof(player1));
    
    if(send(sock, message, MSG_LEN, 0) == -1)
    {
        //perror("send");
        close_connection();
    	return -2;
    }
	if(disconnect) 
    {   	
        close_connection();
        //game_over_init(&player1);
    }
    return 0;
}
/* recive messages from your friend 
 * Return 0 if no errors.
 * -2 - connection closed*/
int recv_message()
{
	if(sock<=0) return -2;
    char buffer[MSG_LEN]; 
	while (TRUE)
	{
		int res;
		if ((res = recv(sock, buffer, MSG_LEN, 0)) == -1) 
		{
			//printf("errno %d\n", errno);
			if (errno != EWOULDBLOCK)
    		{	/* There was 'real' error */
    			close_connection();
    			return -2;
	        }
	        return 0;
		}
		if(buffer[0]!=1)
			printf("Wrong protocol version\n");
		if(!res /*|| buffer[3] */|| buffer[0]!=1) return -2;
        /*14.11.2008 15:38:29 Note: res = 236 (byte)*/

		/* Add "bad" rows to your(player1) game well */
		if(options.easy_mode == TRUE) buffer[2]--;
		int i;
	    for(i=0; i<buffer[2]; i++) add_row(&player1);
		    
		memcpy(&tmp_plr,&buffer[START_POS] , sizeof(player2));  
	    
	}


    return 0;
}
