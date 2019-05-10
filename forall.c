#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdarg.h>
#include<math.h>
#include<sys/sysinfo.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<signal.h>
#include<bits/sigaction.h>
#include<errno.h>
#include<stdbool.h>
void emptyArray(char *arr[],int x)
{
    for(int i=0;i<x;i++)
    {
        arr[i]=NULL;
    }
}

void printArray(char *arr[],int size)
{
    for(int i=0;i<size;i++)
    {
        printf("arr[%d] is %s \n",i,arr[i]);

    }
}
void prepArray(char *arr[],int size,char *argv[])
{
    int ct=0;
    emptyArray(arr,size);
    for(int i=2;i<size;i++)
    {
        
        arr[i-2]=argv[i];
        ct=i;

    }
    ct++;
    
    arr[ct]=NULL;
    //printArray(arr,size);
}

void prepName(char *arr[],int size)
{
    emptyArray(arr,size);
    char buffer[sizeof("10.out")+1];
    for(int i=0;i<size;i++)
    {
        sprintf(buffer,"%d.out",i+1);
        arr[i]=strdup(buffer);//idea to use strdup from stackoverflow for copying string to array
    }
    //printArray(arr,size);

}
static int pid=1;
static bool interrupted=false;
static char *command="NULL";
static char *arg="NULL";
static int Signal=0;
static void sigHandler(int signo)
{
    Signal=signo;
}


int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);// syntax gotten from Professor's example in class
    sa.sa_handler=sigHandler;
    int saveOut = dup(STDOUT_FILENO);//idea to save fd from stackoverflow
    int saveErr = dup(STDERR_FILENO);
    bool quit=false;
    if(sigaction(SIGINT,&sa,NULL)==-1) // syntax gotten from Professor's example in class
    {
        perror("Sigaction SIGINT");
        exit(errno);
    }

     if(sigaction(SIGQUIT,&sa,NULL)==-1)// syntax gotten from Professor's example in class
    {
        perror("Sigaction SIGQUIT");
        exit(errno);
    }
    
    char *arr[argc+1];
    int ct=0;
    int exitCode=1;
    char *name[argc-2];
    prepName(name,argc-2);
    
    
            prepArray(arr,argc,argv);
            command=argv[1];
           
             for(int i=2;i<argc;i++)
            {
                
                arg=argv[i];
                
                
                if(pid>0)
                {
                    pid=fork();
                    if(pid<0)
                    {
                        printf("Error, unable to fork exiting now\n");
                        exit(0);
                    }
                    if(pid>0)
                    {
                        
                        waitpid(pid,&exitCode,0);
                        if(Signal==SIGINT)
                        {
                            printf("Signaling %d\n",pid);
                            fflush(stdout);// flushing stdout necessary to make code work learned from stackoverflow
                            //learned that flushing stdout forces that fd to be written
                            //ensuring write happens correctly for my code
                            interrupted=true;
                            kill(pid,SIGINT);
                            Signal=0;
                        }
                        else if(Signal==SIGQUIT)
                        {
                            interrupted=true;
                            printf("Signaling %d\n",pid);
                            printf("Exiting due to quit signal\n");        
                            fflush(stdout);
                            kill(pid,SIGQUIT);
                            Signal=0;
                            //quit=true;
                            exit(0);

                        }
                        close(0);
                        close(1);
                        int fd=open(name[i-2],O_CREAT|O_WRONLY|O_APPEND,S_IRWXU|S_IRWXG);
                        dup(fd);
                        dup2(fileno(stdout),fileno(stderr));
                        //waitpid(pid,&exitCode,0);
                        if(interrupted)
                        {
                            if(!quit)
                            {
                                printf("Stopped executing %s %s signal = %d\n",command,arg,SIGINT);
                                fflush(stdout);
                                interrupted=false;

                            }
                            
                            
                            
                        }
                        else if(exitCode==0)
                        {
                            printf("Finished executing %s %s with exitcode = %d\n \n",command,arg,exitCode);
                            fflush(stdout);
                        }
                        
                        
                        
                        close(0);//as mentioned above saving stdout stderr idea from stackoverflow
                        close(1);
                        dup2(saveOut,STDOUT_FILENO);
                        dup2(saveErr,STDERR_FILENO);
                        
                    }
                    else if(pid==0)
                    {
                        close(0);
                        close(1);
                        int fd=open(name[i-2],O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU|S_IRWXG);
                        dup(fd);
                        dup2(fileno(stdout),fileno(stderr));//gotten from stackoverflow to dup stderr to same file as stdout
                        printf("Executing %s %s\n",argv[1],argv[i]); 
                        fflush(stdout);
                        execlp(argv[1],argv[1],arr[i-2],NULL);
                    }
                    
                }

                
            }
            


    
}
