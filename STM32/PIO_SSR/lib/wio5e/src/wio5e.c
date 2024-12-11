#include "wio5e.h"

char command[64] = {0};
char devAddr[] = {"26:0B:2A:9A"};
char appsKey[] = {"2E4BBC85B2AAB5B31FCC5BB0A16B07BC"};
char nwksKey[] = {"28D5A8BCD4F80E9DFE8640F5B7072C49"};
char devEui [] = {"70:B3:D5:7E:D0:06:C5:CD"};

void testLoRa() {
  setupLoRa();
  ssr_data_t data;

  data.seq_number = 100;      // Range from 0 to 511 (8 bits total usage)
  data.env_temperature = 300; // Range from -327.68 to 327.67 °C (val/100=°C)
  data.env_humidity = 37;    // Range from -0-100%
  data.env_lux = 448;         // Range from 0 to 1000
  data.dev_voltage = 6245;     // Range from 0-6.5535V (val/10000=V) (val/10=mV)
  data.dev_gyro_x = 21;          // Range from -250 to 250 (val*2=°)
  data.dev_gyro_y = 21;          // Range from -250 to 250 (val*2=°)
  data.dev_gyro_z = 21;          // Range from -250 to 250 (val*2=°)

  send_data_over_lora(data);
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
        data.dev_voltage, data.dev_gyro_x, data.dev_gyro_y, data.dev_gyro_z);
    #ifdef DEBUG
    serial_print(command);
    #endif
    write_read_command(command, devEui);
    memset(command,0,strlen(command));
    
}

void write_read_command(char* command, char* check) {
    // Transmit
    HAL_UART_Transmit(&huart1, (uint8_t*) command, strlen(command), 100);

    // Receive
    char r_buf[100] = {0};
    HAL_UART_Receive(&huart1, (uint8_t*) r_buf, sizeof(r_buf), 1000);

    #ifdef DEBUG
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
    #endif
}