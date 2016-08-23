#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int socket(int domain, int type, int protocol);
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main ()
{

	int sfd,cfd = 0;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;
	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sfd <= 0)
	{
        handle_error("socket");
	}

    memset(&my_addr, 0, sizeof(struct sockaddr_un)); /* Clear structure */
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, "/tmp/mcdc.sock", sizeof(my_addr.sun_path) - 1);

	if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1)
	{
		handle_error("bind");
	}
	
	unlink("tmp/mcdc.sock");

	if (listen(sfd, 150) == -1)
		handle_error("listen");

/* Now we can accept incoming connections one
 *               at a time using accept(2) */

	peer_addr_size = sizeof(struct sockaddr_un);
	char buf[1024] = {0};
	for(;;)
	{
		cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
		if (cfd == -1)
			handle_error("accept");

		printf("accept success\n");

 		for(;;)
		{
			memset(buf, 0x00, sizeof(buf));
			ssize_t rnum = read(cfd, buf, sizeof(buf) - 1);	
			printf("read success\n");
			printf("read num = %d\n", rnum);
			char tmp_buf[16]={0};
			for(int i=0; i<rnum; i++)
			{
				//printf("%c", buf[i]);
				printf("%02x ", buf[i]);
				tmp_buf[i%16]=buf[i];

				if((i+1)%16==0 || i==rnum-1)
				{
					for(int j=0; j<=(i%16); j++)
					{
						if(tmp_buf[j] >= 32 && tmp_buf[j] <= 126)
						    printf("%c", tmp_buf[j]);
						else
						    printf("\033[4m%02x\033[m", tmp_buf[j]);
								
					}
					printf("\n");
				}
			}
		
			if(rnum == 0)
			{
				close(cfd);
				break;
			}
		}
	}

	return 0;
}
