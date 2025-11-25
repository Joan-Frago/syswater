#include <stdlib.h>

#include "../inc/unipi_control.h"

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
