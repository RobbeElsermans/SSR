#include "wio5e.h"

uint8_t delay_time_lora = 1;
delay_callback_t _delay_callback;
void loraDelayCallback(delay_callback_t dc_fp) {
  _delay_callback = dc_fp;
}

char command[64] = {0};
char devAddr[] = {"26:0B:2A:9A"};
char appsKey[] = {"2E4BBC85B2AAB5B31FCC5BB0A16B07BC"};
char nwksKey[] = {"28D5A8BCD4F80E9DFE8640F5B7072C49"};
char devEui [] = {"70:B3:D5:7E:D0:06:C5:CD"};

void testLoRa() { 

}

void setupLoRa() {
    write_read_command("AT\r\n", "OK");
    write_read_command("AT+MODE=\"LWABP\"\r\n", "LWABP");
    write_read_command("AT+DR=EU868\r\n", "EU868");
    write_read_command("AT+CH=num,0\r\n", "NUM, 0");

    sprintf(command, "AT+ID=DevAddr,\"%s\"\r\n", devAddr);
    write_read_command(command, devAddr);
    memset(command,0,strlen(command));

    sprintf(command, "AT+KEY=APPSKEY,\"%s\"\r\n", appsKey);
    write_read_command(command, appsKey);
    memset(command,0,strlen(command));

    sprintf(command, "AT+KEY=NWKSKEY,\"%s\"\r\n", nwksKey);
    write_read_command(command, nwksKey);
    memset(command,0,strlen(command));

    sprintf(command, "AT+ID=DevEui,\"%s\"\r\n", devEui);
    write_read_command(command, devEui);
    memset(command,0,strlen(command));

    sprintf(command, "AT+ID=AppEui,\"%s\"\r\n", devEui);
    write_read_command(command, devEui);
    memset(command,0,strlen(command));

    write_read_command("AT+MODE=\"LWABP\"\r\n", "LWABP");
    write_read_command("AT+DR=7\r\n", "DR7");
    write_read_command("AT+CH=0\r\n", "0,868100000");
}

void send_data_over_lora(ssr_data_t data) {
    sprintf(command, "AT+MSGHEX=\"%04X%04X%02X%04X%04X%02X%02X%02X\"\r\n", 
        data.seq_number, data.env_temperature, data.env_humidity, data.env_lux,
        data.dev_voltage, data.gyro_x, data.gyro_y, data.gyro_z);
    // serial_print(command);
    write_read_command(command, devEui);
    memset(command,0,strlen(command));
    
}

void write_read_command(char* command, char* check) {
    // Transmit
    HAL_UART_Transmit(&huart1, (uint8_t*) command, strlen(command), 100);

    // Receive
    char r_buf[100] = {0};
    HAL_UART_Receive(&huart1, (uint8_t*) r_buf, sizeof(r_buf), 1000);

    // Check reception
    if (strstr(r_buf, check) != NULL) {
        serial_print(r_buf);
    } else {
        // Error:
        serial_print("Error:\r\n");
        serial_print("\tCommand:\t");   serial_print(command);
        serial_print("\tCheck:\t\t");   serial_print(check);    serial_print("\r\n");
        serial_print("\tOutput:\t\t");  serial_print(r_buf);    serial_print("\r\n");
    }
}