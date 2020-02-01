#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int sem_wait(int semid, int sem_num, unsigned int val) {
	struct sembuf op;

	op.sem_num = sem_num;
	op.sem_op = -val;
	op.sem_flg = 0;
	return semop(semid, &op, 1);
}

int sem_signal(int semid, int sem_num, unsigned int val) {
	struct sembuf op;

	op.sem_num = sem_num;
	op.sem_op = val;
	op.sem_flg = 0;
	return semop(semid, &op, 1);
}

void snd_msg(int sfd, int code, char* data) {
	long length = strlen(data);
	//printf("%s\n", data);
	//printf("%li\n", length);
	write(sfd, &code, sizeof(code));
	write(sfd, &length, sizeof(length));
	write(sfd, data, length * sizeof(char));
}
#endif