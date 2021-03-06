#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

void sighup_handler(int);

void sigabrt_handler(int);

void sigsegv_handler(int);

int main(int argc, char *argv[]){


	struct sigaction *action = new ( struct sigaction );
	action->sa_handler = sighup_handler;
	sigemptyset(&(action->sa_mask));
	
	if(sigaction(SIGHUP, action, NULL) < 0){
		perror("Error setting up SIGHUP handler");
		delete action;
		exit(EXIT_FAILURE);
	}
	
	struct sigaction *action2 = new( struct sigaction );
	action2->sa_handler = sigabrt_handler;
	sigemptyset(&(action2->sa_mask));
	if(sigaction(SIGABRT, action2, NULL) < 0){
		perror("Error setting up SIGABRT handler");
		delete action2;
		exit(EXIT_FAILURE);
	}

	struct sigaction *action3 = new( struct sigaction );
	action3->sa_handler = sigsegv_handler;
	sigemptyset(&(action3->sa_mask));
	if(sigaction(SIGSEGV, action3, NULL) < 0){
		perror("Error setting up SIGSEGV handler");
		delete action3;
		exit(EXIT_FAILURE);	
	}

	pid_t pid, ppid;
	ppid = getpid();
	pid = fork();
	
	if(pid < 0){
		perror("Error with fork()");
		exit(EXIT_FAILURE);
	}

	if(pid == 0){
		//child process
		kill(ppid, SIGHUP);
		kill(ppid, SIGABRT);
		
		kill(ppid, SIGABRT);
		kill(ppid, SIGABRT);
		kill(ppid, SIGSEGV);
			
	}else{
		//parent process

		int status;
		//wait on child process
		int w;
		do{
			w = waitpid(pid, &status, 0);
		}while(!WIFEXITED(status));
		//child exited without killing paqqrent
		if(WIFEXITED(status)){
			exit(EXIT_SUCCESS);
		//error with waitpid()
		}else if(w == -1){
			perror("Error with waitpid()");
			exit(EXIT_FAILURE);
		//child did not exit properly
		}else{
			perror("Child did not exit properly");
			exit(EXIT_FAILURE);
		}
	}

	delete action;
	delete action2;
	delete action3;

}

void sighup_handler(int sig){
	signal(SIGHUP, sighup_handler);
	cout<<"sighup_handler() called"<<endl;
}

void sigabrt_handler(int sig){
	signal(SIGABRT, sighup_handler);
	cout<<"sigabrt_handler() called"<<endl;
}

void sigsegv_handler(int sig){
	signal(SIGSEGV, sighup_handler);
	cout<<"sigsegv_handler() called"<<endl;
}
