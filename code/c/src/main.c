#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "../inc/tcp_server.h"
#include "../inc/unipi_control.h"
#include "../inc/config.h"

int set_devices(void);
void exit_handler();

struct Device devices[MAX_DEVICES];

int main(){
	signal(SIGINT, exit_handler);
	signal(SIGTERM, exit_handler);

	printf("Application PID is %ld\n\n",(long)getpid());

	if(load_config() == -1){
		perror("load_config returned -1. Error loading configuration\n");
		return -1;
	}

	// Set all devices before running (read from db and init Devices (Relay or Digital Input or Both))
	set_devices();

	// Start a new thread for each functionality
	pthread_t core_thread;
	pthread_create(&core_thread, NULL, core, NULL);

	pthread_t tcp_server_thread;
	pthread_create(&tcp_server_thread, NULL, start_tcp_server, NULL);

	pthread_join(core_thread, NULL);
	pthread_join(tcp_server_thread, NULL);

	return 0;
}

void exit_handler(){
	printf("\nProgram terminated\n");
	printf("Exiting application...\n");
	exit(0);
}

/*
 * Set all devices before running.
 * Read from db and init Devices.
 * Each Device:
 * 		1 Relay.
 * 		1 Digital Input.
 * 		1 Relay and 1 Digital Input.
 */
int set_devices(){
	printf("Setting devices...\n");
	for(int i = 0; i < MAX_DEVICES; i++){
		devices[i].rl = malloc(sizeof(struct Relay));
		devices[i].di = malloc(sizeof(struct DigitalInput));

		if(i <= 8){
			//char *id_pin = malloc(sizeof("RO2.1"));
			//char *pin = malloc(sizeof("2.1"));
			//pin = ""
			devices[i].rl->id_pin = "RO2.1";
			devices[i].di->id_pin = "DI1.1";
		}
		else if(i <= 16){
			devices[i].di->id_pin = "DI2.1";
		}
	}

	printf("All devices have been set.\n");
	return 0;
}

/*
 * Infinite loop handling each Device's properties.
 * E.g. Historify and Date scheduling.
 * Only Devices must be used.
 * A single Relay or DigitalInput must not be used
 * since it has no logic attached to it.
 */
void *core(void* arg){
	printf("Core Thread ID is %lu\n",(unsigned long)pthread_self());

	for(int i = 0; i < MAX_DEVICES; i++){
		if(devices[i].rl->id_pin != NULL){
			relay_write(devices[i].rl, 0);
		}
		if(devices[i].di->id_pin != NULL){
			int di_status = 0;
			digital_read(devices[i].di, &di_status);
		}
	}

	return 0;
}
