
#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
//#include <fcntl.h>
#include <sys/wait.h>
#include<sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/input.h>
//#include <sys/shm.h>  
//#include <sys/ipc.h>

#include <errno.h>



int GetDateTimeStr(char *pOutStr)
{
	time_t timer;//long
	struct tm *pblock;
	timer = time(NULL);
	pblock = localtime(&timer);
	return sprintf(pOutStr,"%04d-%02d-%02d %02d:%02d:%02d",pblock->tm_year + 1900,pblock->tm_mon+1,pblock->tm_mday,pblock->tm_hour,pblock->tm_min,pblock->tm_sec);
}

void gmTrace(const char *format,...)
{	
	va_list arg;
	va_start(arg, format);
	vprintf(format,arg);
	va_end(arg);	
}

int main(int argc, char* argv[]) 
{
//	int ret;
	pid_t pid;
	if(argc<2)
	{
		gmTrace("main->argc[%d]to small-[%s]\r\n",argc,argv[0]);
		return 0;
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return -1;
	}
	gmTrace("1->InitProcessPool getpid[%d]fork[%d]\r\n",getpid(),pid);
	if(pid != 0)
		return 1;
	pid=setsid();
	gmTrace("2->InitProcessPool getpid[%d]setsid[%d]\r\n",getpid(),pid);
	if(pid < 0)
	{
		gmTrace("setsid failed\n");
		return 2;
	}
	
	/*
	chdir("/");
	{
		int fd;
		fd = open ("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2 (fd, STDIN_FILENO);
			dup2 (fd, STDOUT_FILENO);
			dup2 (fd, STDERR_FILENO);
			if (fd > 2)
				close(fd);
		}
	}*/
	//----------------------------------------------------------------
//	umask(0);	//子进程权限	
	while(1) 
	{
		pid=fork();
		if (pid < 0)
		{
			perror("fork");
			return -2;
		}
		if (pid != 0)
		{//---守护进程-------
			pid_t _pid_t;
			_pid_t = wait(&pid); //---等待孙进程结束------
			gmTrace("------reRunAPP-[%d,%d]---------\n",_pid_t,pid);
			sleep(5);
			//return 1;
		} 
		else break; //孙进程，跳出外面执行
	} 
	//------------------孙进程------------------------------------------
	umask(0); //应用进程权限
//	chdir("/");
//	chmod("tree",0777);
	{
		int i;
		char *pArgv[8];
		argc--;
		for(i=0;i<argc;i++)
		{
			pArgv[i]=argv[i+1];
		}
		pArgv[i]=NULL;
		gmTrace("execv->[%s]-%X,%d\r\n",pArgv[0],pArgv[1],i);
		i=execv(pArgv[0] ,pArgv);
		gmTrace("execv->ret[%d][%s]\r\n",i,strerror(errno));
	}
	return 0;
//	return Handle_sever();
}



