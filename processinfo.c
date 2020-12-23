#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include<linux/syscalls.h>
#include<linux/init.h>

#define PARAM_LEN 2048
// MAKE FILE 		obj-y := processinfo.o
// syscall_64.tbl	335	common	processinfo	__x64_sys_processinfo
// syscalls.h		asmlinkage long sys_processinfo(char __user *, char __user *,char __user *);
void cstringcpy(char *src, char * dest)
{
	while (*src)
	{
		*(dest++) = *(src++);
	}
	*dest = '\0';
}

SYSCALL_DEFINE3(processinfo, char __user *, charg, char

__user *, chpid,char __user *, usr_write_back_dst)

{	

	int return_val = 0;
	int res;
	char *all="-all";
	char *p="-p";
	char *k="-k";
	char *noarg="noarg";
	char param_0[PARAM_LEN] = {'\0'};
	char param_1[PARAM_LEN] = {'\0'};
	char write_back_all[PARAM_LEN] = {'\0'};
	char write_back_p[PARAM_LEN] = {'\0'};
	char write_back_k[PARAM_LEN] = {'\0'};
	char write_back_noarg[PARAM_LEN] = {'\0'};
	int processid;
	
	//getting parameters from userspace 
	if(copy_from_user(param_0, chpid, PARAM_LEN) ==0)
	{
		printk(KERN_INFO " %s was copied from user",param_0);
	}
	res=kstrtoint(param_0,10,&processid);

	if(copy_from_user(param_1, charg, PARAM_LEN) ==0)
	{
		printk(KERN_INFO "%s was copied from user",param_1);
	}
	
	// if argument is -all 
	if(strcmp(param_1,all)==0)
	{	
		
		struct task_struct *proces;
		char *table="uid\t ppid\t pid\t name\t\t state\n";
		cstringcpy(table, write_back_all);
    		for_each_process(proces) 
        	{	
			//this loop iterates over all process 
			char buffer[100];
			// snprintf is used here to convert multiple types to string 
			snprintf(buffer, 100, "%llu \t %ld \t %ld \t %s\t %ld\n", proces->cred->uid, task_pid_nr(proces->parent), task_pid_nr(proces),proces->comm,(char)proces->state);
			strcat(write_back_all,buffer);
			
	    				
		}
		//informations are sended to userspace 
		return_val = copy_to_user(usr_write_back_dst, write_back_all,PARAM_LEN);
		if(return_val == 0)
		{
			printk(KERN_INFO "%s",write_back_all);
		}

	}
	// if argument is -p pid
	if(strcmp(param_1,p)==0)

	{
		
	    	struct file *f;
	    	char buf[128];
	    	mm_segment_t fs;
	    	int i;
		
	    	char proc[]="/proc/";
    		char rest[]="/cmdline";


		
		char path[PARAM_LEN] = {'\0'};
		
  		struct pid *pid_struct = find_get_pid(processid);
		struct task_struct *task = pid_task(pid_struct,PIDTYPE_PID);
		//check if path is exist , for example /proc/0/cmdline is never exist
		if (task == NULL)
		{
   		 	
			char *message="error finding process";
			printk (KERN_INFO "Process with pid %d is not running or not exist \n",processid);
			cstringcpy(message, write_back_p);
			return_val = copy_to_user(usr_write_back_dst,write_back_p,
PARAM_LEN);	
		
			if(return_val == 0)
			{
				printk(KERN_INFO "%s\n",write_back_p);
			}
			return -1;
		}
    		cstringcpy(proc, path);
    		strcat(path,param_0); // proc will be /proc/pid/cmdline
		strcat(path,rest);
		
		//reading the file
	    	for(i=0;i<128;i++)
		{
			buf[i] = 0;
		}
		   	
		f = filp_open(path, O_RDONLY, 0);
	    	if(f == NULL)
		{	
			char *fileerrormessage="there is no process with that pid";
			cstringcpy(fileerrormessage, write_back_p);
			printk(KERN_ALERT "filp_open error!!.\n");	
			
		}	    	
		else
		{	
			char *tab="	";
			// Get current segment descriptor
			fs = get_fs();
			// Set segment descriptor associated to kernel space
			set_fs(get_ds());
			// Read the file
			f->f_op->read(f, buf, 128, &f->f_pos);
			// Restore segment descriptor
			set_fs(fs);
			cstringcpy(buf, write_back_p);
			strcat(write_back_p,tab);
			strcat(write_back_p,param_0);
			
			
	    	}
		return_val = copy_to_user(usr_write_back_dst,write_back_p,
PARAM_LEN);	
		if(return_val == 0)
		{
			printk(KERN_INFO "%s\n",write_back_p);
		}
	    	filp_close(f,NULL);
	}
	//if argument is -k pid
	if(strcmp(param_1,k)==0)

	{
		
		
		
		int signum = SIGKILL, sig_ret;
		struct siginfo info;
		char *kill_message=".Process was killed";
		struct pid *pid_struct = find_get_pid(processid);
		struct task_struct *task = pid_task(pid_struct,PIDTYPE_PID);
		//check if process is exist
		if (task == NULL)
		{
			char *message="error sending signal";
			printk (KERN_INFO "Process with pid %d is not running \n",processid);
			cstringcpy(message, write_back_k);
			return_val = copy_to_user(usr_write_back_dst,write_back_k,
PARAM_LEN);	
		
			if(return_val == 0)
			{
				printk(KERN_INFO "%s",write_back_k);
			}
			return -1;
		}
		
		memset(&info, '\0', sizeof(struct siginfo));
		info.si_signo = signum;
		//send a SIGKILL to the daemon
		sig_ret = send_sig_info(signum, &info, task);
		if (sig_ret < 0)
		{	
			char *message="error sending signal";
			cstringcpy(message, write_back_k);
			printk(KERN_INFO "error sending signal\n");
			return -1;
		}
		
		cstringcpy(param_0, write_back_k);
		strcat(write_back_k,kill_message);
		return_val = copy_to_user(usr_write_back_dst,write_back_k,
PARAM_LEN);	
		
		if(return_val == 0)
		{
			printk(KERN_INFO "%s",write_back_k);
		}
		
		
	}
	//if there is no argument 
	if(strcmp(param_1,noarg)==0)
	{	
		char *noarg_message="Right Usage:\n -all prints some information (process id and its\n argument/s) about all processes\n -p takes process id and prints the details of it\n -k takes process id\n";
		cstringcpy(noarg_message, write_back_noarg);
		return_val = copy_to_user(usr_write_back_dst,write_back_noarg,
PARAM_LEN);	
		
		if(return_val == 0)
		{
			printk(KERN_INFO "%s",write_back_noarg);
		}
		
	}
	return return_val;

}


