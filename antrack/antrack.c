#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>

#pragma comment(lib, "modbus.lib")

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	/* Read 5 registers from the address 10 */
	rc = modbus_read_registers(mb, 10, 40, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}

	for (i = 0; i < rc; i = i + 2) {
		printf("reg[%d]=%f (0x%X)\n", i, modbus_get_float(&tab_reg[i]), tab_reg[i]);
	}

	modbus_close(mb);
	modbus_free(mb);
}