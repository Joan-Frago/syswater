#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNIPI_SYS_BASE_DIR "/sys/unipi-sys/"

struct Relay{
	char *id_pin; // e.g. RO2.1
	char *pin; // e.g. 2.1
};

struct DigitalInput{
	char *id_pin;
	char *pin;
};

int relay_write(struct Relay *, int);
int digital_read(struct DigitalInput *, int *);

int main(){
	// Relay
	struct Relay rl;
	rl.id_pin = "RO2.1";

	relay_write(&rl,1);

	// Digital Input
	struct DigitalInput di;
	di.id_pin = "DI1.4";
	di.pin = "1.4";

	int *di_status = (int *)malloc(sizeof(int));
	digital_read(&di, di_status);

	return 0;
}

int relay_write(struct Relay *relay, int newState){
	int ret_val = 0;

	size_t path_len = strlen(UNIPI_SYS_BASE_DIR) + strlen(relay->id_pin) + strlen("/value") + 1;

	// Allocate memory for the file path
	char *file_path = (char *)malloc(path_len);
	if(file_path == NULL){
		perror("Could not allocate memory for file path");
		ret_val = -1;
	}
	else{
		strcpy(file_path, UNIPI_SYS_BASE_DIR);
		strcat(file_path, relay->id_pin);
		strcat(file_path, "/value");

		printf("%s\n",file_path);
		//FILE *fp = fopen(fileDir,"w");
	}
	
	free(file_path);
	
	return ret_val;
}

int digital_read(struct DigitalInput *di, int *dest){
	int ret_val = 0;

	size_t path_len = strlen(UNIPI_SYS_BASE_DIR) + strlen(di->id_pin) + strlen("/value") + 1;

	// Allocate memory for the file path
	char *file_path = (char *)malloc(path_len);
	if(file_path == NULL){
		perror("Could not allocate memory for file path");
		ret_val = -1;
	}
	else{
		strcpy(file_path, UNIPI_SYS_BASE_DIR);
		strcat(file_path, di->id_pin);
		strcat(file_path, "/value");

		printf("%s\n",file_path);
		//FILE *fp = fopen(fileDir,"w");
	}
	
	free(file_path);

	*dest = 1;

	return ret_val;
}
