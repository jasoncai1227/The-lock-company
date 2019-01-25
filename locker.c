#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "locker.h"


struct list{
	int size;
	struct locker_t locker;
	struct list* next;
};

struct list* head;
struct list* current_p;
int number_locker=0;

int locked=1;

struct locker_t create(int num_locker,int r_fd,int w_fd,pid_t pid){
	struct locker_t new;
	new.id=num_locker;
	new.locked=1;
	new.owned=0;
	new.read_fd=r_fd;
	new.write_fd=w_fd;
	new.pid=pid;
	return new;
}

void delete(int index){
	struct list* current=head;
	struct list* current_1=head;
	struct list* pre;
	char* buffer="DELETE";
	int msg_len=strlen(buffer)+1;
	
	while(current_1->next!=NULL){
		if(current_1->next->locker.id==index){
			break;
		}
		current_1=current_1->next;
	}
	if(current_1->next==NULL){
		printf("Locker Does Not Exist\n");
	}
	
	while(current->next!=NULL){
		if(current->next->locker.id==index){
			//close(current->next->locker.read_fd);
			ssize_t nwritten = write(current->next->locker.write_fd,buffer,msg_len );
			if (-1 == nwritten) {
				perror("soemthing really wrong here");
			}
			//close(current->next->locker.write_fd);
			pre=current->next;
			current->next=pre->next;
			current_p=current;
			free(pre);
			printf("Locker %d Removed\n",index);
			printf("\n");
			number_locker--;
			break;
		}
		current=current->next;
	}
	return;
}

void query(int index){
	struct list* current=head;
	char* buffer="QUERY";
	int msg_len=strlen(buffer)+1;
	char buffer_in[100];
	while(current->next!=NULL){

		if(current->next->locker.id==index){
			ssize_t nwritten = write(current->next->locker.write_fd,buffer,msg_len );
			if (-1 == nwritten) {
				perror("soemthing really wrong here");
			}
			read(current->next->locker.read_fd,buffer_in,100);
			if(strcmp(buffer_in,"locked")==0){current->next->locker.locked=1;}
			if(strcmp(buffer_in,"unlocked")==0){current->next->locker.locked=0;}

			printf("Locker ID: %d\n",index);
			if(current->next->locker.locked==0){printf("Lock Status: unlocked\n");}
			else{printf("Lock Status: locked\n");}
			if(current->next->locker.owned==0){printf("Owner: unowned\n");}
			else{printf("Owner: %d\n",current->next->locker.user_id);}
			printf("\n");
			break;
		}
		current=current->next;
	}
	if(current->next==NULL){
		printf("Locker Does Not Exist\n");
	}
	return;
}

void queryall(){
	struct list* current=head;
	char* buffer="QUERY";
	int msg_len=strlen(buffer)+1;
	char buffer_in[100];
	while(current->next!=NULL){
			ssize_t nwritten = write(current->next->locker.write_fd,buffer,msg_len );
			if (-1 == nwritten) {
				perror("soemthing really wrong here");
			}
			read(current->next->locker.read_fd,buffer_in,100);
			if(strcmp(buffer_in,"locked")==0){current->next->locker.locked=1;}
			if(strcmp(buffer_in,"unlocked")==0){current->next->locker.locked=0;}

			printf("Locker ID: %d\n",current->next->locker.id);
			if(current->next->locker.locked==0){printf("Lock Status: unlocked\n");}
			else{printf("Lock Status: locked\n");}
			if(current->next->locker.owned==0){printf("Owner: unowned\n");}
			else{printf("Owner: %d\n",current->next->locker.user_id);}
			printf("\n");

			current=current->next;
	}
	return;
}

void attach(int owner_id){
	if(owner_id>255){
		printf("Maximum owner id is 255\n");
		return;
	}
	struct list* current=head;
	while(current->next!=NULL){
		if(current->next->locker.owned==0){
			current->next->locker.owned=1;
			current->next->locker.user_id=owner_id;
			printf("Locker %d Owned By %d\n",current->next->locker.id,owner_id);
			printf("\n");
			break;
		}
		current=current->next;
	}
	if(current->next==NULL){
		printf("No Lockers Available\n");
	}
	return;
}

void detach(int index){
	struct list* current=head;
	while(current->next!=NULL){
		if(current->next->locker.id==index){
			current->next->locker.owned=0;
			current->next->locker.user_id=0;
			printf("Locker %d Unowned\n",current->next->locker.id);
			printf("\n");
			break;
		}
		current=current->next;
	}
	if(current->next==NULL){
		printf("Locker Does Not Exist\n");
	}
	return;
}

void quit(){
	struct list* current=head;
	struct list* pre;
	char* buffer="QUIT";
	int msg_len=strlen(buffer)+1;
	while(current->next!=NULL){
		//close(current->next->locker.read_fd);
		ssize_t nwritten = write(current->next->locker.write_fd,buffer,msg_len );
		if (-1 == nwritten) {
			perror("soemthing really wrong here");
		}
		//close(current->next->locker.write_fd);
		pre=current;
		current=pre->next;
		free(pre);
	}
	return;
}

void hand(int sig){
	locked=1;
	return;
}

void handler(int sig){
	locked=0;
	return;
}

void wait_children(int signo) {
  int status;
  while(waitpid(-1, &status, WNOHANG) > 0);
}

int main() {
	head=(struct list*)malloc(sizeof(struct list));
	while(1){
		char input_1[30];
		char* input;
		char* index_1;
		int index;
		fgets(input_1,30,stdin);
		//scanf("%s" "%d",input,&index);
		input=strtok(input_1," \n");
		index_1=strtok(NULL," \n");
		if(index_1!=NULL){
		index=atoi(index_1);
		}
		pid_t pid;
		if(strcmp(input,"CREATE")==0){
			number_locker+=1;
			
			int fd[2];
			fd[0]=-1;
			fd[1]=-1;
			int result;
			result=pipe(fd);
			if(result==-1){printf("faled to create pipe\n");}

			int rfd[2];
			rfd[0]=-1;
			rfd[1]=-1;
			result=pipe(rfd);
			if(result==-1){printf("faled to create pipe\n");}

			fflush(stdout);
			pid=fork();
			if(pid==-1){printf("Cannot Build Locker\n");}
			else if(pid==0){
				//close(fd[1]);
				char buffer_in[100];
				char* buffer;
				while(1){
					signal(SIGUSR1,hand);
					signal(SIGUSR2,handler);
					ssize_t nread = read( fd[0], buffer_in, 100);
					if (-1 == nread) {
						perror("read failed");
					}
					if(nread>0){
						if(strcmp(buffer_in,"QUERY")==0){
							if(locked==1){
								buffer="locked";
								int length=strlen(buffer)+1;
								write(rfd[1],buffer,length);
							}else{
								buffer="unlocked";
								int length=strlen(buffer)+1;
								write(rfd[1],buffer,length);
							}
						}
					
						if(strcmp(buffer_in,"DELETE")==0){
							break;
						}
						if(strcmp(buffer_in,"QUIT")==0){
							break;
						}
					}
				}
				exit(0);
				//close(fd[0]);
			}
			else if(pid>0){

				signal(SIGCHLD, wait_children);
				struct locker_t new;
				struct list* node=(struct list*)malloc(sizeof(struct list));
				new=create(number_locker,rfd[0],fd[1],pid);
				node->locker=new;
				node->next=NULL;
				node->size=number_locker;
				if(number_locker==1){
					head->next=node;
					current_p=node;
				}else{
					current_p->next=node;
					current_p=node;
				}
				printf("New Locker Created: %d\n",number_locker);
				printf("\n");
			}
		}
		else if(strcmp(input,"QUIT")==0){
			quit();
			break;
		}
		else if(strcmp(input,"DELETE")==0){
			delete(index);
		}
		else if(strcmp(input,"QUERY")==0){
			query(index);
		}
		else if(strcmp(input,"QUERYALL")==0){
			queryall();
		}
		else if(strcmp(input,"ATTACH")==0){
			attach(index);
		}
		else if(strcmp(input,"DETACH")==0){
			detach(index);
		}
	}
	return 0;
}

