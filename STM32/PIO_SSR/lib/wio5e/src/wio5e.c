#include "wio5e.h"

char command[64] = {0};
char devAddr[] = {"26:0B:88:CA"};
char appsKey[] = {"7121B71152E792B5F6A08E5A9D232ECD"};
char nwksKey[] = {"5BE481EA8F66B4D3B047F6FE2A71A176"};
char devEui [] = {"70:B3:D5:7E:D0:06:CE:E2"};

uint8_t counter = 1;
#ifdef DEBUG
uint8_t buffy[16] = {0};
#endif

// Setup commands take 100ms --> 127 ms
// Send commands take 90ms   --> 127 ms

void testLoRa() {
    if (counter > 64) {
       // After this error is given the module has succesfully send the data to TTN
        sleepLoRa(1);
        _delay_callback(7*60*1000); // 7 minutes needed between transmissions
        sleepLoRa(0);
        counter = 0;
    }

    if (counter == 0) {
        _delay_callback(27);
        initLoRa();
    }
    

    #ifdef DEBUG
    sprintf(buffy, "Loop: %d\r\n", counter);
    serial_print(buffy);
    #endif

    ssr_data_t data;
    data.seq_number = (uint16_t)counter;  // Range from 0 to 511 (8 bits total usage)
    data.env_temperature = 300; // Range from -327.68 to 327.67 °C (val/100=°C)
    data.env_humidity = 37;     // Range from -0-100%
    data.env_lux = 448;         // Range from 0 to 1000
    data.dev_voltage = 6245;    // Range from 0-6.5535V (val/10000=V) (val/10=mV)
    data.dev_gyro_x = 21;       // Range from -250 to 250 (val*2=°)
    data.dev_gyro_y = 21;       // Range from -250 to 250 (val*2=°)
    data.dev_gyro_z = 21;       // Range from -250 to 250 (val*2=°)

    // 3200 ms needed between transmissions
    send_data_over_lora(data);  // High power sleep (27)
    sleepLoRa(1);
    _delay_callback(3200);      // Low power sleep (3173)
    sleepLoRa(0);
    
    counter++;
}

void initLoRa() {
    write_command("AT\r\n");
    write_command("AT+MODE=\"LWABP\"\r\n");
    write_command("AT+DR=EU868\r\n");
    write_command("AT+CH=num,0-2\r\n");
    //write_read_command("AT\r\n", "OK", 2000);
    //write_read_command_1000("AT+MODE=\"LWABP\"\r\n", "LWABP");
    //write_read_command_1000("AT+DR=EU868\r\n", "EU868");
    //write_read_command_1000("AT+CH=num,0-2\r\n", "NUM, 0-2");

    sprintf(command, "AT+ID=DevAddr,\"%s\"\r\n", devAddr);
    write_command(command);
    //write_read_command_1000(command, devAddr);
    memset(command,0,strlen(command));

    sprintf(command, "AT+KEY=APPSKEY,\"%s\"\r\n", appsKey);
    write_command(command);
    //write_read_command_1000(command, appsKey);
    memset(command,0,strlen(command));

    sprintf(command, "AT+KEY=NWKSKEY,\"%s\"\r\n", nwksKey);
    write_command(command);
    //write_read_command_1000(command, nwksKey);
    memset(command,0,strlen(command));

    sprintf(command, "AT+ID=DevEui,\"%s\"\r\n", devEui);
    write_command(command);
    //write_read_command_1000(command, devEui);
    memset(command,0,strlen(command));

    sprintf(command, "AT+ID=AppEui,\"%s\"\r\n", devEui);
    write_command(command);
    //write_read_command_1000(command, devEui);
    memset(command,0,strlen(command));

    write_command("AT+MODE=\"LWABP\"\r\n");
    write_command("AT+DR=7\r\n");
    write_command("AT+CH=0\r\n");
    //write_read_command_1000("AT+MODE=\"LWABP\"\r\n", "LWABP");
    //write_read_command_1000("AT+DR=7\r\n", "DR7");          // FSK 50kbps
    //write_read_command_1000("AT+CH=0\r\n", "0,868100000");
}
void sleepLoRa(uint8_t sleep) {
    if (sleep) {
        #ifdef DEBUG
        serial_print("Sleep\r\n");
        #endif
        //write_read_command_1000("AT+LOWPOWER\r\n", "SLEEP");
        write_command("AT+LOWPOWER\r\n");
        // _delay_callback(3*60*1000); // 3 minutes needed between transmissions
    } else {
        #ifdef DEBUG
        serial_print("Wake-Up\r\n");
        #endif
        //write_read_command_1000("AT\r\n", "WAKEUP");
        write_command("AT\r\n");
        _delay_callback(5);     // After waking up, wait 5 ms
    }
}

void send_data_over_lora(ssr_data_t data) {
    sprintf(command, "AT+MSGHEX=\"%04X%04X%02X%04X%04X%02X%02X%02X\"\r\n", 
        data.seq_number, data.env_temperature, data.env_humidity, data.env_lux,
        data.dev_voltage, data.dev_gyro_x, data.dev_gyro_y, data.dev_gyro_z);
    #ifdef DEBUG
    serial_print(command);
    #endif
    write_command(command);
    //write_read_command(command, "Done", 3200);
    memset(command,0,strlen(command));
}

void write_read_command_1000(char* command, char* check) {
    write_read_command(command, check, 1000);
}
void write_read_command(char* command, char* check, uint16_t delay) {
    // Transmit
    write_command(command);

    // Receive
    char r_buf[100] = {0};
    HAL_UART_Receive(&huart1, (uint8_t*) r_buf, sizeof(r_buf), delay);

    #ifdef DEBUG
    // Check reception
    if (strstr(r_buf, check) != NULL) {
        serial_print(r_buf);
    } else if (strstr(r_buf, "+MSGHEX: FPENDING") != NULL) {
        // Data has been send
    } else {
        // Error:
        #ifdef DEBUG
        serial_print("Error:\r\n");
        serial_print("\tCommand:\t");   serial_print(command);
        serial_print("\tCheck:\t\t");   serial_print(check);    serial_print("\r\n");
        serial_print("\tOutput:\t\t");  serial_print(r_buf);    serial_print("\r\n");
        #endif
    }
    #endif
}
void write_command(char* command) {
    HAL_UART_Transmit(&huart1, (uint8_t*) command, strlen(command), 100);
    _delay_callback(27);
}