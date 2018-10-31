#include <syslog.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/sysinfo.h>

#define M_BYTE 1024*1024
struct sysinfo info;

void sig_handler(int signo)
{
  if(signo == SIGTERM)
  {
    syslog(LOG_INFO, "SIGTERM has been caught! Exiting...");
    if(remove("run/daemon.pid") != 0)
    {
      syslog(LOG_ERR, "Failed to remove the pid file. Error number is %d", errno);
      exit(1);
    }
    exit(0);
  }
}

void handle_signals()
{
  if(signal(SIGTERM, sig_handler) == SIG_ERR)
  {
    syslog(LOG_ERR, "Error! Can't catch SIGTERM");
    exit(1);
  }
}

void daemonise()
{
  pid_t pid, sid;
  FILE *pid_fp;

  syslog(LOG_INFO, "Starting daemonisation.");

  //First fork
  pid = fork();
  if(pid < 0)
  {
    syslog(LOG_ERR, "Error occured in the first fork while daemonising. Error number is %d", errno);
    exit(1);
  }

  if(pid > 0)
  {
    syslog(LOG_INFO, "First fork successful (Parent)");
    exit(0);
  }
  syslog(LOG_INFO, "First fork successful (Child)");

  //Create a new session
  sid = setsid();
  if(sid < 0) 
  {
    syslog(LOG_ERR, "Error occured in making a new session while daemonising. Error number is %d", errno);
    exit(1);
  }
  syslog(LOG_INFO, "New session was created successfuly!");

  //Second fork
  pid = fork();
  if(pid < 0)
  {
    syslog(LOG_ERR, "Error occured in the second fork while daemonising. Error number is %d", errno);
    exit(1);
  }

  if(pid > 0)
  {
    syslog(LOG_INFO, "Second fork successful (Parent)");
    exit(0);
  }
  syslog(LOG_INFO, "Second fork successful (Child)");

  pid = getpid();

  //Change working directory to Home directory
  if(chdir(getenv("HOME")) == -1)
  {
    syslog(LOG_ERR, "Failed to change working directory while daemonising. Error number is %d", errno);
    exit(1);
  }

  //Grant all permisions for all files and directories created by the daemon
  umask(0);

  //Redirect std IO
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  if(open("/dev/null",O_RDONLY) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stdin while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(open("/dev/null",O_WRONLY) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stdout while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(open("/dev/null",O_RDWR) == -1)
  {
    syslog(LOG_ERR, "Failed to reopen stderr while daemonising. Error number is %d", errno);
    exit(1);
  }

  //Create a pid file
  mkdir("run/", 0777);
  pid_fp = fopen("run/daemon.pid", "w");
  if(pid_fp == NULL)
  {
    syslog(LOG_ERR, "Failed to create a pid file while daemonising. Error number is %d", errno);
    exit(1);
  }
  if(fprintf(pid_fp, "%d\n", pid) < 0)
  {
    syslog(LOG_ERR, "Failed to write pid to pid file while daemonising. Error number is %d, trying to remove file", errno);
    fclose(pid_fp);
    if(remove("run/daemon.pid") != 0)
    {
      syslog(LOG_ERR, "Failed to remove pid file. Error number is %d", errno);
    }
    exit(1);
  }
  fclose(pid_fp);
}

int sizeOfFile(FILE *buffer)
{
  int size, curPos;
  curPos = ftell(buffer);
  fseek(buffer, 0, SEEK_END);
  size = ftell(buffer);
  fseek(buffer, curPos, SEEK_SET);
  return size;
}

int randomTime()
{
  //syslog(LOG_INFO, "randTime");
  int time = clock();
  return time; 
}

/*int urandom()
{
  int ur= rand();
  int uRandomFile = open("/dev/urandom", O_RDONLY);
  if (uRandomFile == -1)
  {
    syslog(LOG_INFO, "can't open urandom file");
    return ur;
  }
  else
  {
    syslog(LOG_INFO, "from urandom file: %i", uRandomFile);
    return uRandomFile;
  }
}*/

int randomInterval(int min, int max)
{
  return (min + rand() * (max - min));
}

int rndmgnrt()
{
  //рандом
  syslog(LOG_INFO, "rndmgnrt");
  int randTime, randLib, bufferRam, procs, uptime, randInt;
  
  randLib = rand();
  //randFile = urandom();
  //syslog(LOG_INFO, "rand = %i", rNum); 
  uptime = (int)info.uptime;
  procs = (int)info.procs;
  bufferRam = (int)info.bufferram;
  syslog(LOG_INFO, "procs = %i", procs); 
  //bufferRam = info->loads[0];
  //syslog(LOG_INFO, "bufferram = %lu", bufferRam); 
  //randInt = randomInterval(0, 10000);
   
  randTime = (int)randomTime();
  //return (procs * (randTime ^ (bufferRam & randLib)));
  return ((randLib  ^ randTime) * (bufferRam ^ procs)); 
}


void fillData(FILE *buffer)
{
  
  syslog(LOG_INFO, "fillData()"); 
  int k = 0;
  
  while (sizeOfFile(buffer) < 5 * M_BYTE)
  {
    k = rndmgnrt();
    //syslog(LOG_INFO, "k = %i", k); 
    fwrite(&k, sizeof(int), 1, buffer);
  } 
}


void randomize()
{
  FILE *buffer;
  int size;
  chdir(getenv("HOME"));
  if(mkdir("random", S_IRWXU | S_IRWXG | S_IRWXO))
  {
    syslog(LOG_INFO, "dir is already create");
  }
  if((buffer = fopen("random/buffer", "rb")) == NULL)
  {
    syslog(LOG_INFO, "not exist buffer file, create new file");
    buffer = fopen("random/buffer", "wb");
    size = sizeOfFile(buffer);
    syslog(LOG_INFO, "size of empty file is %i byte", size);
    fillData(buffer);
  }
  else
  {
    syslog(LOG_INFO, "exist buffer file");
    buffer = fopen("random/buffer", "ab");
    size = sizeOfFile(buffer);
    syslog(LOG_INFO, "size is %i", size);
    if (size < 5 * M_BYTE )
    {
      fillData(buffer);
      syslog(LOG_INFO, "size is less than 5 mb");
    }
    else
    {
      syslog(LOG_INFO, "size is more than 5 mb, do nothing");  
      //do nothing    
    }
    fclose(buffer);
  }
  
  
}

int main(int argc,char *argv[])
{
  
  pid_t pid;
  int fd, len;
  FILE *pid_fp;
  char pid_buf[16];
  sysinfo(&info);
  srand(time(NULL));
  if (argc == 2) 
  {
    //syslog(LOG_INFO, "2 arg");
    if(!strcmp(argv[1], "start"))
    {
      //syslog(LOG_INFO, "start");
      chdir(getenv("HOME"));
      if((pid_fp = fopen("run/daemon.pid", "r")) == NULL)
      {
        //syslog(LOG_INFO, "create daemon.pid");
        daemonise();
        handle_signals();
      }
      else
      {
        //syslog(LOG_INFO, "daemon.pid is exist");
        fclose(pid_fp);
        exit(0);
      }
    }
    if(!strcmp(argv[1], "stop"))
    {
       //syslog(LOG_INFO, "Stop");
       chdir(getenv("HOME"));
       if((pid_fp = fopen("run/daemon.pid", "r")) == NULL)
       {
          //syslog(LOG_INFO, "already killed");
          exit(0); 
       }
       else
       {
         //syslog(LOG_INFO, "find pid");
         fscanf(pid_fp, "%i", &pid);
         kill(pid, SIGTERM);
         fclose(pid_fp);
         exit(0);
       }
     }
  }
  else
  {
    exit(0);
  }

  while(1)
  {
    randomize();
    sleep(5);
  }
  return 0;
}
