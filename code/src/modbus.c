#include "../inc/unipi_control.h"
#include "../inc/logger.h"
#include <modbus.h>

void analyzer_set_registers(reg_t registers[REGISTER_COUNT]){
	registers[0].name = "Tension Fase"; registers[0].id = 0;
	// ...
	// ...
	registers[1].name = "Temperatura";  registers[1].id = 80;
}

uint32_t modbus_read(mb_t modbus, reg_t reg){
	modbus_t *mb;
	uint16_t tab_reg[2];

	if(modbus.connection_type == TCP){
		mb = modbus_new_tcp(modbus.tcp_addr, modbus.tcp_port);
	}

	modbus_set_slave(mb, modbus.slave);
	modbus_connect(mb);

	modbus_read_registers(mb, reg.id, 2, tab_reg);

	uint32_t value = (tab_reg[0] << 16) | tab_reg[1];

	modbus_close(mb);
	modbus_free(mb);

	return value;
}
