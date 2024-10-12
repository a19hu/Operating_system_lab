#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>


void producer(int write_fd,int N){

	for(int i=1; i<=N; i++){
		write(write_fd, &i, sizeof(i));
	}

	close(write_fd);
}

void consumer(int read_fd){

	int num;
	int sum=0;
	
	while(read(read_fd, &num, sizeof(num)) > 0){
		if(num % 2 != 0){
			sum +=num;
		}
	}

	close(read_fd);

	printf("sum of odd number: %d", sum);
}




int main(int argc,char *argv[]){
     if(argc =! 2) {
         printf("%s\n", argv[0]);
     }
     int N= atoi(argv[1]); // Convert string to integer

     pid_t producer_p,consumer_p;
	int pipe_fd[2];
	
	if(pipe(pipe_fd) == -1){
		perror("pipe");
		return -1;
	}

     producer_p=fork();
	if(producer_p < 0){
		printf("some error");
		return 1;
	}

	if(producer_p == 0){

		close(pipe_fd[0]);
		producer(pipe_fd[1],N);
		exit(0);
	}

    consumer_p=fork();
	if(consumer_p < 0){
		printf("some error");
		return 1;
	}

	if(consumer_p == 0 ){

		close(pipe_fd[1]);
		consumer(pipe_fd[0]);
		exit(0);
	}

	close(pipe_fd[0]);
	close(pipe_fd[1]);

	return 0;
}
