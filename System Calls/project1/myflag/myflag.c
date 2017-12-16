//Ahmet Türk	150120107
//Tuğba Özkal	150120053
//--------------------------------------------------
// system call to set myFlag
#include <linux/sched.h> 	// find_task_by_vpid
#include <linux/errno.h> 	// EACCES, EINVAL, ESRCH

asmlinkage long set_myFlag(pid_t pid, int flag){
	if((current->cred)->uid == 0){	
		if(flag == 0 || flag == 1){
			struct task_struct *task;
			task = find_task_by_vpid(pid);
			if(task == NULL){
				return ESRCH;
			}
			task->myFlag = flag;
		} 
		else {
			return EINVAL;
		}
	}
	else {
		return EACCES ;
	}
	return 0;
}
