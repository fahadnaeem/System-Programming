// AHMET TURK - TUGBA OZKAL
// 150120107  - 150120053

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/file.h>  
#include <asm/switch_to.h>
#include <asm/uaccess.h>

#include "messagebox_ioctl.h"

#define MESSAGEBOX_MAJOR 0
#define MESSAGEBOX_MINOR 0
#define MESSAGEBOX_UNREAD_MESSAGE_LIMIT 5

int messagebox_major = MESSAGEBOX_MAJOR;
int messagebox_minor = MESSAGEBOX_MINOR;
int messagebox_unread_message_limit = MESSAGEBOX_UNREAD_MESSAGE_LIMIT;

MODULE_AUTHOR("Ahmet Turk, Tugba Ozkal");
MODULE_LICENSE("Dual BSD/GPL");

struct message {
	char* messageText;
	int size;
	int isRead;
	struct message* next;
};

struct mUser {
	char* name;
	struct message* messages;
	int messageCount;
	int unreadMessageCount;
	struct mUser* next;
};

struct messagebox_dev {
    struct mUser* users;
    int userCount;
    int readMode; // 0 = exclude read (default), 1 = include read
    struct cdev cdev;
};

struct messagebox_dev *messagebox_device;

int messagebox_trim(struct messagebox_dev *dev)
{
    int i, j;

    if (dev->users) {
        for (i = 0; i < dev->userCount; i++) {
            if (dev->users->messages) {
                for (j = 0; j < dev->users->messageCount; j++) {
					struct message* tempMessage = dev->users->messages;
					dev->users->messages = dev->users->messages->next;
					kfree(tempMessage->messageText);
					kfree(tempMessage);
				}
			}
			struct mUser* tempUser = dev->users;
			dev->users = dev->users->next;
			kfree(tempUser);
        }
    }
    dev->userCount = 0;
    dev->readMode = 0;
    return 0;
}


int messagebox_open(struct inode *inode, struct file *filp)
{
    struct messagebox_dev *dev;

    dev = container_of(inode->i_cdev, struct messagebox_dev, cdev);
    filp->private_data = dev;
 
    return 0;
}


int messagebox_release(struct inode *inode, struct file *filp)
{
    return 0;
}

char* getName(char* temp) {
	// temp is like "@joe hello\n"
	char* space = strchr(temp, ' ');
	int nameSize = space - temp - 1; // nameSize is 3 for joe
	char* userName = kmalloc(nameSize + 1, GFP_KERNEL); // joe + \0 = 4 character
	strncpy(userName, temp + 1, nameSize); // copy joe to userName
	userName[nameSize] = '\0';
	return userName;
}

char* getMessage(char* temp, int* userMessageSize, char* senderName) {
	// temp is like "@joe hello\n", userMessageSize is 14, senderName is like "alice"
	char* msg = strchr(temp, ' ') + 1;
	*userMessageSize = strlen(msg) + strlen(senderName) + 3; // alice + ": " + "hello\n" + "\0" = 14
	char* userMessage = kmalloc(*userMessageSize,  GFP_KERNEL);
	strcpy(userMessage, senderName);
	strcat(userMessage, ": ");
	strcat(userMessage, msg);
	userMessage[*userMessageSize] = '\0';
	return userMessage;
}


uid_t convertToUid_t(char* userUid) {
	uid_t uidNumber = 0;
	int i = 0;
	char buf = userUid[i];
	while(buf != '\0') {
		uidNumber *= 10;
		uidNumber += (buf - 48);
		i++;
		buf = userUid[i];
	}
	return uidNumber;
}


char* getUserNameFromUid(kuid_t uid) 
{
	char* kernel_buf_name = kmalloc(21, GFP_KERNEL);
	char* kernel_buf_uid = kmalloc(21, GFP_KERNEL);
	char __user *userName = (__force char __user *)kernel_buf_name; 
	char __user *userUid = (__force char __user *)kernel_buf_uid; 
	mm_segment_t oldfs = get_fs(); 
	set_fs(KERNEL_DS);
	
	int i, j = 0;
	char buf[1];
	struct file * filp;
	filp = filp_open("/etc/passwd", O_RDONLY, 0);
	filp->f_pos = 0;
	
	while (j < 1000) {
		for (i = 0; i < 20; i++) {
			vfs_read(filp, buf, 1, &filp->f_pos);
			if (buf[0] == ':') {
				break;
			}
			userName[i] = buf[0];
		}
		userName[i] = '\0';
		
		filp->f_pos += 2;
		
		for (i = 0; i < 20; i++) {
			vfs_read(filp, buf, 1, &filp->f_pos);
			if (buf[0] == ':') {
				break;
			}
			userUid[i] = buf[0];
		}
		userUid[i] = '\0';
		
		uid_t uidNumber = convertToUid_t(userUid);
		
		if (uidNumber == uid.val) {
			set_fs(oldfs);
			return userName;
		}
		
		while (buf[0] != '\n') {
			vfs_read(filp, buf, 1, &filp->f_pos);
		}
		j++;
	}
	set_fs(oldfs);
	return "noname";
}


ssize_t messagebox_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int i, j;
	kuid_t uid;
	loff_t k = *f_pos;
	struct messagebox_dev *dev = filp->private_data;
	struct mUser* ptrUser;
	struct message* ptrMessage;
	ssize_t retval = 0;
	char* userName;
	
	uid = get_current_user()->uid;
	userName = getUserNameFromUid(uid);
	
	if (dev->users) {
		ptrUser = dev->users;
		for (i = 0; i < dev->userCount; i++) {
			if (strcmp(ptrUser->name, userName) == 0) {
				if (ptrUser->messages) {
					ptrMessage = ptrUser->messages;
					for(j = 0; j < ptrUser->messageCount; j++) {
						if (dev->readMode || !(ptrMessage->isRead)) {
							if (k > 0) {
								k--;
								ptrMessage = ptrMessage->next;
							}
							else {
								if (copy_to_user(buf, ptrMessage->messageText, ptrMessage->size)) {
									retval = -EFAULT;
									goto out;
								}
								retval = ptrMessage->size;
								ptrMessage->isRead = 1;
								ptrUser->unreadMessageCount--;
								break;
							}
						}
						else {
							ptrMessage = ptrMessage->next;
						}
					}	
				}
				break;
			}
		}
	}

	if (dev->readMode) {
		(*f_pos)++;
	}

	out:
    return retval;
}


ssize_t messagebox_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int i, userMessageSize, hasUser = 0;
	kuid_t uid;
	char* userName;
	char* userMessage;
	char* senderName;
	struct mUser* ptrUser;
	struct message* ptrMessage;
	struct messagebox_dev *dev = filp->private_data;
	ssize_t retval = -ENOMEM;

	char* temp = kmalloc(count, GFP_KERNEL);

	if (copy_from_user(temp, buf, count)) {
        retval = -EFAULT;
        goto out;
    }
	*f_pos += count;
    retval = count;

	uid = get_current_user()->uid;
	senderName = getUserNameFromUid(uid);

	userName = getName(temp); 
	userMessage = getMessage(temp, &userMessageSize, senderName);

	kfree(temp);
	
	if (dev->users) {
		ptrUser = dev->users;
		for (i = 0; i < dev->userCount; i++) {
			// compare to users
			if (strcmp(ptrUser->name, userName) == 0) {
				// found the user
				printk(KERN_ALERT "messagebox_unread_message_limit %d\n", messagebox_unread_message_limit);
				printk(KERN_ALERT "ptrUser->unreadMessageCount %d\n", ptrUser->unreadMessageCount);
				if (ptrUser->unreadMessageCount < messagebox_unread_message_limit) {
					ptrUser->messageCount++;
					ptrUser->unreadMessageCount++;
					hasUser = 1;
					// add first message to ptrMessage
					ptrMessage = ptrUser->messages;
					ptrUser->messages = kmalloc(sizeof(struct message), GFP_KERNEL);
					ptrUser->messages->size = userMessageSize;
					ptrUser->messages->isRead = 0;
					ptrUser->messages->messageText = userMessage;
					// add the previous first message to next of new message
					ptrUser->messages->next = ptrMessage;
				}
				else {
					// unreadMessageCount is full
					retval = -ENOMEM;
					goto out;
				}
			} 
			else {
				ptrUser = ptrUser->next;
			}
		}
	}
	
	if (!hasUser) {
		if (dev->userCount > 0) {
			ptrUser = dev->users;
		}
		dev->userCount++;
		dev->users = kmalloc(sizeof(struct mUser), GFP_KERNEL);
		dev->users->name = userName;
		dev->users->messageCount = 1;
		dev->users->unreadMessageCount = 1;
		dev->users->messages = kmalloc(sizeof(struct message), GFP_KERNEL);
		dev->users->messages->size = userMessageSize;
		dev->users->messages->isRead = 0;
		dev->users->messages->messageText = userMessage;
		dev->users->next = ptrUser;
	}
	
	out:
    return retval;
}

long messagebox_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int i, j, retval = 0;
	char* deleteMessagesUserName;
	struct messagebox_dev *dev = filp->private_data;
		
	switch(cmd) {
		case EXCLUDE_READ:
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			dev->readMode = 0;
			break;
		case INCLUDE_READ:
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			dev->readMode = 1;
			break;
		case UNREAD_MESSSAGE_LIMIT:
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			messagebox_unread_message_limit = arg;
			break;
		case DELETE_MESSAGES:
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			deleteMessagesUserName = kmalloc(21, GFP_KERNEL);
			strncpy_from_user(deleteMessagesUserName, arg, 20);
			deleteMessagesUserName[20] = '\0';
			if (dev->users) {
				struct mUser* ptrUser = dev->users;
				for (i = 0; i < dev->userCount; i++) {
					if (strcmp(ptrUser->name, deleteMessagesUserName) == 0) {
						if (ptrUser->messages) {
							struct message* ptrMessage = ptrUser->messages;
							for(j = 0; j < ptrUser->messageCount; j++) {
								struct message* tmp = ptrMessage;
								ptrMessage = ptrMessage->next;
								kfree(tmp->messageText);
								kfree(tmp); 
							}
						}
						ptrUser->messageCount = 0;
						ptrUser->unreadMessageCount = 0;
						break;	
					}
					else {
					ptrUser = ptrUser->next;
					}
				} 
			}
			break;
	}
	return retval;
}

struct file_operations messagebox_fops = {
    .owner =    THIS_MODULE,
    .read =     messagebox_read,
    .write =    messagebox_write,
    .unlocked_ioctl = 	messagebox_ioctl,
    .open =     messagebox_open,
    .release =  messagebox_release,
};

void messagebox_cleanup_module(void) 
{
	dev_t devno = MKDEV(messagebox_major, messagebox_minor);
	
	if (messagebox_device) {
		messagebox_trim(messagebox_device);
		cdev_del(&messagebox_device->cdev);
    }
	kfree(messagebox_device);

	unregister_chrdev_region(devno, 1);
}

int messagebox_init_module(void)
{
	int result;
	int err;
	dev_t devno = 0;
	struct messagebox_dev *dev;
	
	if (messagebox_major) {
        devno = MKDEV(messagebox_major, messagebox_minor);
        result = register_chrdev_region(devno, 1, "messagebox");
    } else {
        result = alloc_chrdev_region(&devno, messagebox_minor, 1, "messagebox");
        messagebox_major = MAJOR(devno);
    }
    
    if (result < 0) {
        printk(KERN_WARNING "messagebox: can't get major %d\n", messagebox_major);
        return result;
    }
    
    messagebox_device = kmalloc(sizeof(struct messagebox_dev), GFP_KERNEL);
    if (!messagebox_device) {
        result = -ENOMEM;
        goto fail;
    }
    memset(messagebox_device, 0, sizeof(struct messagebox_dev));

	/* Initialize device. */
	dev = messagebox_device;
	dev->userCount = 0;
	dev->readMode = 0; // exclude read (default)
	cdev_init(&dev->cdev, &messagebox_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &messagebox_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)        
		printk(KERN_NOTICE "Error %d adding messagebox", err);


	return 0; /* succeed */

	fail:
    messagebox_cleanup_module();
    return result;
}

module_init(messagebox_init_module);
module_exit(messagebox_cleanup_module);
