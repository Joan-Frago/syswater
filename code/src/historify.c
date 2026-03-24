#include "../inc/historify.h"
#include "../inc/logger.h"
#include "../inc/unipi_control.h"
#include "../inc/device.h"

// #define LOG_HISTORIFY

static void update_last_value(device_t *);
static void historify_on_change(device_t *);
static void historify_all(device_t *);

int historify_device(device_t *device){
	if(device->hist.active == 1){
		update_last_value(device);

		if(device->hist.period == 0){
			historify_on_change(device);
		}
		else{
			if(device->hist.remaining_ticks == 1){
				historify_all(device);
				
				// Restart counter
				device->hist.remaining_ticks = device->hist.period;
			}
			else{
				device->hist.remaining_ticks--;
			}
		}
	}
	return 0;
}

static void update_last_value(device_t *device){
	if(device->has_rl == 1){
		device->rl.last_value = device->rl.value;
		device->rl.value = relay_read(&device->rl);
	}

	if(device->has_di == 1){
		device->di.last_value = device->di.value;
		device->di.value = digital_read(&device->di);
	}

	if(device->has_mb == 1){
		int i;
		for(i=0; i<REGISTER_COUNT; i++){
			device->mb.registers[i].last_value = device->mb.registers[i].value;
			device->mb.registers[i].value = modbus_read(device->mb, device->mb.registers[i]);
		}
	}
}

static void historify_on_change(device_t *device){
	// RELAY
	if(device->has_rl == 1){
		if(device->rl.value != device->rl.last_value){
			#ifdef LOG_HISTORIFY
			LOG_DEBUG("Historify device \"%s\" Relay \"%s\". Value = %d.",device->name, device->rl.id_pin, device->rl.value);
			#endif
		}
	}

	// DIGITAL INPUT
	if(device->has_di == 1){
		if(device->di.value != device->di.last_value){
			#ifdef LOG_HISTORIFY
			LOG_DEBUG("Historify device \"%s\" DigitalInput \"%s\". Value = %d.",device->name, device->di.id_pin, device->di.value);
			#endif
		}
	}

	// MODBUS
	if(device->has_mb == 1){
		int i;
		for(i=0; i<REGISTER_COUNT; i++){
			if(device->mb.registers[i].value != device->mb.registers[i].last_value){
				#ifdef LOG_HISTORIFY
				LOG_DEBUG("Historify device \"%s\" Modbus: register \"%d\". Value = %d.",device->name, device->mb.registers[i].id, device->mb.registers[i].value);
				#endif
			}
		}
	}
}

static void historify_all(device_t *device){
	// RELAY
	if(device->has_rl == 1){
		#ifdef LOG_HISTORIFY
		LOG_DEBUG("Historify device \"%s\" Relay \"%s\". Value = %d.",device->name, device->rl.id_pin, device->rl.value);
		#endif
	}

	// DIGITAL INPUT
	if(device->has_di == 1){
		#ifdef LOG_HISTORIFY
		LOG_DEBUG("Historify device \"%s\" DigitalInput \"%s\". Value = %d.",device->name, device->di.id_pin, device->di.value);
		#endif
	}

	// MODBUS
	if(device->has_mb == 1){
		#ifdef LOG_HISTORIFY
			int i;
			for(i=0; i<REGISTER_COUNT; i++){
				LOG_DEBUG("Historify device \"%s\" Modbus: register \"%d\". Value = %d.",device->name, device->mb.registers[i].id, device->mb.registers[i].value);
			}
		#endif
	}
}
