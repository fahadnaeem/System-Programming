// AHMET TURK - TUGBA OZKAL
// 150120107  - 150120053

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>

#include "messagebox_ioctl.h"

int main()
{
	int fd, retval, limit;
	char name[21];
	
	fd = open("/dev/messagebox", 0);
	if (fd == -1) {
		printf("Device cannot be accessed!\n");
	}
	
	printf("1 : INCLUDE READ - ALL MESSAGES\n");
	printf("2 : EXCLUDE READ - UNREAD MESSAGES\n");
	printf("3 : CHANGE UNREAD MESSAGE LIMIT\n");
	printf("4 : DELETE MESSAGES OF A USER\n");
	
	int choice;
	scanf("%d", &choice);
	
	switch(choice) {
		case 1:
			retval = ioctl(fd, INCLUDE_READ);
			break;
		case 2:
			retval = ioctl(fd, EXCLUDE_READ);
			break;
		case 3:
			printf("New limit: ");
			scanf("%d", &limit); 
			if (limit > 0) {
				retval = ioctl(fd, UNREAD_MESSSAGE_LIMIT, limit);
			}
			else {
				printf("Limit must be greater than zero\n");
			}
			break;
		case 4:
			printf("User name: ");
			scanf("%s", &name); 
			retval = ioctl(fd, DELETE_MESSAGES, name);
			break;
		default:
			printf("Sorry I could not understand you\n");
	}
	
	if (retval == -EPERM) {
		printf("You need to be root!\n");
	}
	
	close(fd);
	return 0;
}
