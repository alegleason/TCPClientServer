/*----------------------------------------------------------------

*

* Programación avanzada: Proyecto final

* Fecha: 2-DIC-2019

* Autor: A01703013 ALEJANDRO GLEASON MÉNDEZ

*

*--------------------------------------------------------------*/

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
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

#define SEM	0
#define E1 201
#define E2 202
#define E3 203
#define E4 204
#define E5 205
#define E6 206
#define E7 207
#define W1 301
#define W2 302

void snd_msg(int sfd, int code, char* data) {
	long length = strlen(data);
	write(sfd, &code, sizeof(code));
	write(sfd, &length, sizeof(length));
	write(sfd, data, length * sizeof(char));
}

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

#endif
