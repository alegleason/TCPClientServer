#include "header.h"

#define MAX 2
#define BUFSIZE 1024 
	
void read_socket(int sfd, int length, char* data) {
	int current, data_read;
	char *aux;
	
	aux = (char*) malloc(BUFSIZE * sizeof(char));
	current = 0;
	while (current != length) {
		data_read = read(sfd, aux, BUFSIZE);
		memcpy((data + current), aux, data_read);
		current += data_read; 
	}
	free(aux);
}

/*********************** FILES ***********************/
void test_dont_read_file(int sfd) {
	char* arr[] = {"/imagen4_n.jpg", "/dir1/imagen8_n.jpg", "/dir1/dir3/imagen7_n.jpg"};
	int code, num = (rand() % 3);
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 101, arr[num]);
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST %s - expected code = 201 - received code = %i\n", arr[num], code);
	
	free(data);
}

void test_dont_exist_file(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 101, "/noexiste.jpg");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"/noexiste.jpg\" - expected code = 202 - received code = %i\n", code);
	
	free(data);
}

void test_wrong_name(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 101, "imagen2_y.jpg");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"imagen2_y.jpg\" - expected code = 203 - received code = %i\n", code);
	
	free(data);
}

void test_not_a_file(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 101, "/dir1");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"/dir1\" - expected code = 205 - received code = %i\n", code);
	
	free(data);
}

void test_correct_file(int sfd) {
	char* arr[] = {"/imagen2_y.jpg", "/dir1/imagen1_y.jpg", "/dir1/dir3/imagen3_y.jpg"};
	char current_dir[PATH_MAX + NAME_MAX + 1];
	char filename[PATH_MAX + NAME_MAX + 1];
	int fd, code, num = (rand() % 3);
	long length, data_read, current;
	char *data, *aux;

	snd_msg(sfd, 101, arr[num]);
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	getcwd(current_dir, NAME_MAX);
	sprintf(filename, "%s/%s", current_dir, arr[num]);
	fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	write(fd, data, length);
	close(fd);
	
	printf("TEST %s - expected code = 301 - received code = %i\n", arr[num], code);
	
	free(data);
}

void test_file(int sfd) {
	int i, num;
	
	for (i = 0; i < 15; i++) {
		num = (rand() % 5);
		switch (num) {
			case 0  : test_dont_read_file(sfd); break;
			case 1  : test_dont_exist_file(sfd); break;
			case 2  : test_wrong_name(sfd); break;
			case 3  : test_not_a_file(sfd); break;
			default : test_correct_file(sfd); break;
		}
		//sleep((rand() % 3) + 1);
	}
}
/*********************** FILES ***********************/

/*********************** DIRECTORIES ***********************/
void test_wrong_name_dir(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 102, "dir1");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"dir1\" - expected code = 203 - received code = %i\n", code);
	
	free(data);
}

void test_dont_exist_dir(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 102, "/noexiste");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"/noexiste\" - expected code = 206 - received code = %i\n", code);
	
	free(data);
}

void test_not_a_dir(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 102, "/imagen2_y.jpg");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"/dir1\" - expected code = 207 - received code = %i\n", code);
	
	free(data);
}

void test_correct_dir(int sfd) {
	char* arr[] = {"/", "/dir1", "/dir1/dir3"};
	int code, num = (rand() % 3);
	long length, data_read, current;
	char* data, *aux;

	snd_msg(sfd, 102, arr[num]);
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	data[length] = '\0';
	printf("%s\n", data);
	
	printf("TEST %s - expected code = 302 - received code = %i\n", arr[num], code);
	
	free(data);
}

void test_dir(int sfd) {
	int i, num;
	
	for (i = 0; i < 12; i++) {
		num = (rand() % 4);
		switch (num) {
			case 0  : test_wrong_name_dir(sfd); break;
			case 1  : test_dont_exist_dir(sfd); break;
			case 2  : test_not_a_dir(sfd); break;
			default : test_correct_dir(sfd); break;
		}
		//sleep((rand() % 3) + 1);
	}
}
/*********************** DIRECTORIES ***********************/

void test_hello(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"init msg\" - expected code = 1 - received code = %i\n", code);
	sleep((rand() % 3) + 1);
	
	free(data);
}

void test_dont_command(int sfd) {
	int code;
	long length, data_read;
	char* data;
	
	snd_msg(sfd, 555, "NO EXISTE");
	
	read(sfd, &code, sizeof(code));
	read(sfd, &length, sizeof(length));
	data = (char*) malloc(length * sizeof(char));
	read_socket(sfd, length, data);
	
	printf("TEST \"NO EXISTE\" - expected code = 204 - received code = %i\n", code);
	
	sleep((rand() % 3) + 1);
	
	free(data);
}

void all_test(char* ip, int port, char* program) {
	int sfd, code, semid;
	struct sockaddr_in server_info;
	key_t key;

	if ( (key = ftok("/dev/null", 127)) < 0 ){
		perror("ftok");
		exit(-1);
	}

	if ( (semid = semget(key, 1, 0666 | IPC_CREAT)) < 0 ) {
		perror("semget");
		exit(-1);
	}
	
	if ( (sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror(program);
		exit(-1);
	}
	
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = inet_addr(ip);
	server_info.sin_port = htons(port);
	if ( connect(sfd, (struct sockaddr *) &server_info, sizeof(server_info)) < 0 ) {
		perror(program);
		exit(-1);
	}
	
	srand(getpid());
	test_hello(sfd);
	
	test_dont_command(sfd);
	
	test_file(sfd);
	
	test_dir(sfd);
		
	snd_msg(sfd, 103, "Adios");
	close(sfd);

}

int main(int argc, char* argv[]) {
	int port, pid, i;
	
	if (argc != 3) {
	    printf("usage: %s ip port\n", argv[0]);
	    return -1;
	}
	
	port = atoi(argv[2]);
	if (port < 5000) {
		printf("%s: The port must be greater than 5000.\n", argv[0]);
		return -1;
	}
	
	for (i = 0; i < MAX; i++) {
		if ( (pid = fork()) < 0 ) {
			perror(argv[0]);
			exit(-1);
		} else if (pid == 0) {
			all_test(argv[1], port, argv[0]);
		} else {
			// do nothing
		}
	}
	while (i > 0) {
		wait(NULL);
		i--;
	}
	return 0;
}
