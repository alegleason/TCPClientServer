/*----------------------------------------------------------------

*

* Programación avanzada: Proyecto final

* Fecha: 2-DIC-2019

* Autor: A01703013 ALEJANDRO GLEASON MÉNDEZ

*

*--------------------------------------------------------------*/

#include "header.h"

typedef char uchar;
typedef unsigned long ulong;

void serves_client(int nsfd, char* program, char* ip, char* root) {
	int code, flag = 0, semid;
	long length, data_read;
	char* data;
	key_t key;
	if ( (key = ftok("/dev/null", 127)) < 0 ) {
		perror("ftok");
		exit(-1);
	}

	if ( (semid = semget(key, 1, 0666 | IPC_CREAT)) < 0 ) {
		perror("semget");
		exit(-1);
	}

	while(1){
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		FILE * fp;
	    int i;
	    
		if(flag == 0){/*First connection detected, respond with hello*/
			snd_msg(nsfd, 1, "hola");
			flag = 1;
			/* open the file for writing*/
			sem_wait(semid, SEM, 1);
		    fp = fopen ("log.txt","a");
		    fprintf(fp, "Se ha establecido conexión con el cliente: %d-%d-%d %d:%d:%d %s \n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip);
		    fclose (fp);
		    sem_signal(semid, SEM, 1);
		}

		read(nsfd, &code, sizeof(code));
		read(nsfd, &length, sizeof(length));
		//if data == null, 203
		data = (char*) malloc((length + 1) * sizeof(char));
		data[length] = '\0';
		if (data == NULL)
		{
			snd_msg(nsfd, E3, "Error interno");
			close(nsfd);
			exit(-1);
		}
		read(nsfd, data, length * sizeof(char));
		sem_wait(semid, SEM, 1);
		fp = fopen ("log.txt","a");
	    fprintf(fp, "Comando recibido por el cliente: %d-%d-%d %d:%d:%d %s código: %i parámetro: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, code, data);
	    fclose (fp);
	    sem_signal(semid, SEM, 1);
		if (code == 101 || code == 102)
		{
			if(code == 101){/*Ruta del archivo*/
				if (data[0] != '/' || strstr(data, "..") != NULL)
				{
					snd_msg(nsfd, E3, "La ruta debe empezar con / sin secuencias de ..");
					sem_wait(semid, SEM, 1);
					fp = fopen ("log.txt","a");
				    fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E3, "La ruta debe empezar con / sin secuencias de ..");
				    fclose (fp);
				    sem_signal(semid, SEM, 1);
				}else{
					struct stat info;
					char dir_name[PATH_MAX + NAME_MAX + 1];
					char *directory;
					char new[PATH_MAX + NAME_MAX + 1];
					int aux, source;
					
					getcwd(dir_name, NAME_MAX);
			        directory = dir_name;

			        sprintf(new, "%s/%s", directory, root);
			        sprintf(new, "%s%s", new, data);

				    aux = lstat(new, &info);

				    if(aux == 0){/*succesful, exists*/
					    if (S_ISREG(info.st_mode)) { /* FILES */
						    if ((source = open(new, O_RDONLY)) > 0 )
						    {
						    	long size = lseek(source, 0, SEEK_END);

							    char* data;
							    data = (char*)malloc(sizeof(char) * size);
							    int code = W1;

							    lseek(source, 0, SEEK_SET);
							    long aux = read(source, data, size);
							    
							  	write(nsfd, &code, sizeof(code));
								write(nsfd, &aux, sizeof(aux));
								write(nsfd, data, aux);

							    free(data);
							    close(source);
							    sem_wait(semid, SEM, 1);
			            		fp = fopen("log.txt","a");
							    fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, W1, new);
							    fclose(fp);
							    sem_signal(semid, SEM, 1);
						    }else{
						    	snd_msg(nsfd, E1, "Permiso denegado");
						    	sem_wait(semid, SEM, 1);
			            		fp = fopen ("log.txt","a");
							    fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E1, "Permiso denegado");
							    fclose (fp);
							    sem_signal(semid, SEM, 1);
						    }
		            	}else if(S_ISDIR(info.st_mode)){
		            		snd_msg(nsfd, E5, "La ruta es un directorio");
		            		sem_wait(semid, SEM, 1);
		            		fp = fopen ("log.txt","a");
						    fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E5, "La ruta es un directorio");
						    fclose (fp);
						    sem_signal(semid, SEM, 1);
		            	}
				    }else{/*does not exists*/
				    	snd_msg(nsfd, E2, "Archivo no encontrado");
				    	sem_wait(semid, SEM, 1);
				    	fp = fopen ("log.txt","a");
						fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E2, "Archivo no encontrado");
						fclose (fp);
						sem_signal(semid, SEM, 1);
				    }
				}
			}else{/*Ruta del directorio*/
				if (data[0] != '/' || strstr(data, "..") != NULL)
				{
					snd_msg(nsfd, E3, "La ruta debe empezar con / sin secuencias de ..");
					sem_wait(semid, SEM, 1);
					fp = fopen ("log.txt","a");
					fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E3, "La ruta debe empezar con / sin secuencias de ..");
					fclose (fp);
					sem_signal(semid, SEM, 1);
				}else{
					DIR *dir;
					struct dirent* direntry;
					struct stat info;
					char dir_name[PATH_MAX + NAME_MAX + 1];
					char *directory;
					char new[PATH_MAX + NAME_MAX + 1];
					int aux, fd, flag2 = 0;
					char buf[2048] = "";
					getcwd(dir_name, NAME_MAX);
			        directory = dir_name;

			        sprintf(new, "%s/%s", directory, root);
			        sprintf(new, "%s%s", new, data);

				    aux = lstat(new, &info);

				    if(aux == 0){/*succesful, exists*/
					    if(S_ISDIR(info.st_mode)){
							if ( (dir = opendir(new)) == NULL ) {
								perror(program);
								exit(-1);
							}
							while ( (direntry = readdir(dir)) != NULL ) {
						        if (strcmp(direntry->d_name, ".") != 0 &&
						            strcmp(direntry->d_name, "..") != 0) {
						        	sprintf(buf, "%s %s", buf, direntry->d_name);
						        }
						    }

						    snd_msg(nsfd, W2, buf);
						    sem_wait(semid, SEM, 1);
		            		fp = fopen ("log.txt","a");
							fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, W2, new);
							fclose (fp);
							sem_signal(semid, SEM, 1);
		            	}else{
		            		snd_msg(nsfd, E7, "La ruta no es un directorio");
		            		sem_wait(semid, SEM, 1);
		            		fp = fopen ("log.txt","a");
							fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E7, "La ruta no es un directorio");
							fclose (fp);
							sem_signal(semid, SEM, 1);
		            	}
				    }else{/*does not exists*/
				    	snd_msg(nsfd, E6, "Directorio no encontrado");
				    	sem_wait(semid, SEM, 1);
				    	fp = fopen ("log.txt","a");
						fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E6, "Directorio no encontrado");
						fclose (fp);
						sem_signal(semid, SEM, 1);
				    }
				}
			}
		}else if(code == 103){ /*Cut connection*/
			close(nsfd);
			exit(0);
		}else{
			snd_msg(nsfd, E4, "Comando no conocido");
			sem_wait(semid, SEM, 1);
			fp = fopen ("log.txt","a");
			fprintf(fp, "Respuesta enviada al cliente: %d-%d-%d %d:%d:%d %s código: %i mensaje: %s\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ip, E4, "Comando no conocido");
			fclose (fp);
			sem_signal(semid, SEM, 1);
		}
	}
}

void server(char* ip, int port, char* program, char* root) {
	int sfd, nsfd, pid;
	struct sockaddr_in server_info, client_info;

	if ( (sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror(program);
		exit(-1);
	}

	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = inet_addr(ip);

	server_info.sin_port = htons(port);
	if ( bind(sfd, (struct sockaddr *) &server_info, sizeof(server_info)) < 0 ) {
		perror(program);
		exit(-1);
	}

	listen(sfd, 1);
	while (1) {
		int len = sizeof(client_info);
		if ( (nsfd = accept(sfd, (struct sockaddr *) &client_info, &len)) < 0 ) {
			perror(program);
			exit(-1);
		}

		char *ip = inet_ntoa(client_info.sin_addr);
		/*Concurrent*/
		if ((pid = fork()) < 0)
		{
			perror(program);
		}else if(pid == 0){
			close(sfd);
			serves_client(nsfd, program, ip, root);
			exit(0);
		}else{
			close(nsfd);
		}
		
	}
}

int main(int argc, char* argv[]) {
	char ip[15];
	int port, semid;
	key_t key;

	if ( (key = ftok("/dev/null", 127)) < 0 ){
		perror("ftok");
		return -1;
	}

	if ( (semid = semget(key, 1, 0666 | IPC_CREAT)) < 0 ) {
		perror("semget");
		return -1;
	}

	semctl(semid, SEM, SETVAL, 1);

	if (argc != 4) {
		printf("usage: %s ip port raiz\n", argv[0]);
		return -1;
	}

	port = atoi(argv[2]);
	if (port < 5000) {
		printf("%s: The port must be greater than 5000.\n", argv[0]);
		return -1;
	}

	/*if (strcmp("raiz", argv[3]) != 0)
	{
		printf("%s: The root file is called 'raiz', please enter that name.\n", argv[0]);
		return -1;
	}*/

	strcpy(ip, argv[1]);

	server(ip, port, argv[0], argv[3]);

	return 0;
}
