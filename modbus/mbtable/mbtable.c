#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>

#pragma comment(lib, "modbus.lib")

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	uint8_t bit_reg[256];
	int rc;
	int i;

	if (argc != 2)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 502);
	if (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "modbus connect: %s\n", modbus_strerror(errno));
		modbus_free(mb);
		return -1;
	}

	while (TRUE)
	{
		system("cls");

		/* Read 5 coil registers from the address 0 */
		rc = modbus_read_bits(mb, 0, 5, bit_reg);
		if (rc == -1) {
			fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
			return -1;
		}

		/* Read 32 holding registers from the address 8 */
		rc = modbus_read_registers(mb, 8, 34, tab_reg);
		if (rc == -1) {
			fprintf(stderr, "read registers: %s\n", modbus_strerror(errno));
			return -1;
		}

		printf("%40s %d\n", "Status of spectrometer:", bit_reg[0]);
		printf("%40s %d\n", "Status of thermocontrollers:", bit_reg[1]);
		printf("%40s %d\n", "Availability for external request:", bit_reg[2]);
		printf("%40s %d\n", "Status of Zero test:", bit_reg[3]);
		printf("%40s %d\n\n", "Status of calibration:", bit_reg[4]);

		printf("%30s %f\n", "Elemental mercury:", modbus_get_float_badc(&tab_reg[16]));
		printf("%30s %f\n", "Total mercury:", modbus_get_float_badc(&tab_reg[2]));
		printf("%30s %f\n", "Oxidized mercury:", modbus_get_float_badc(&tab_reg[4]));
		printf("%30s %f\n", "Monitor flow:", modbus_get_float_badc(&tab_reg[6]));
		printf("%30s %f\n", "Vacuum:", modbus_get_float_badc(&tab_reg[8]));
		printf("%30s %f\n", "Dilution pressure:", modbus_get_float_badc(&tab_reg[10]));
		printf("%30s %f\n", "Bypass pressure:", modbus_get_float_badc(&tab_reg[12]));
		printf("%30s %f\n", "Temperature of spectrometer:", modbus_get_float_badc(&tab_reg[14]));
		printf("%30s %f\n", "Code of a current mode:", modbus_get_float_badc(&tab_reg[0]));
		printf("%30s %f\n", "Errors and warnings:", modbus_get_float_badc(&tab_reg[20]));
		printf("%30s %f\n", "Calibration coefficient:", modbus_get_float_badc(&tab_reg[22]));
		printf("%30s %f\n", "PMT current:", modbus_get_float_badc(&tab_reg[24]));
		printf("%30s %f\n", "PMT voltage:", modbus_get_float_badc(&tab_reg[32]));

		Sleep(2000);
	}

	modbus_close(mb);
	modbus_free(mb);
}
