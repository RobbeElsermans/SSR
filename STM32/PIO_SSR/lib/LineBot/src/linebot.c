#include "linebot.h"

uint8_t delay_time = 1;
delay_callback_t _delay_callback;
void lineBotDelayCallback(delay_callback_t dc_fp) {
  _delay_callback = dc_fp;
}

void testLineBot() {
    setupLineBot();

    HAL_Delay(500);
    driveLineBot(FORWARD);
    HAL_Delay(1000);
    driveLineBot(STOP);
    HAL_Delay(500);
    driveLineBot(LEFT);
    HAL_Delay(1000);
    driveLineBot(STOP);
    HAL_Delay(500);
    driveLineBot(RIGHT);
    HAL_Delay(1000);
    driveLineBot(STOP);
    HAL_Delay(500);
    driveLineBot(BACKWARD);
    HAL_Delay(1000);
    driveLineBot(STOP);
}

void setupLineBot() {
    HAL_GPIO_WritePin(LB_S_GPIO_Port, LB_S_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_F_GPIO_Port, LB_F_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_B_GPIO_Port, LB_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_L_GPIO_Port, LB_L_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_R_GPIO_Port, LB_R_Pin, GPIO_PIN_SET);
}

void driveLineBot(DRIVE_STATE action) {
    // Pins are active low
    switch (action) {
    case STOP:
        HAL_GPIO_WritePin(LB_S_GPIO_Port, LB_S_Pin, GPIO_PIN_RESET);
        _delay_callback(delay_time);
        HAL_GPIO_WritePin(LB_S_GPIO_Port, LB_S_Pin, GPIO_PIN_SET);
        break;

    case FORWARD:
        HAL_GPIO_WritePin(LB_F_GPIO_Port, LB_F_Pin, GPIO_PIN_RESET);
        _delay_callback(delay_time);
        HAL_GPIO_WritePin(LB_F_GPIO_Port, LB_F_Pin, GPIO_PIN_SET);
        break;

    case BACKWARD:
        HAL_GPIO_WritePin(LB_B_GPIO_Port, LB_B_Pin, GPIO_PIN_RESET);
        _delay_callback(delay_time);
        HAL_GPIO_WritePin(LB_B_GPIO_Port, LB_B_Pin, GPIO_PIN_SET);
        break;

    case LEFT:
        HAL_GPIO_WritePin(LB_L_GPIO_Port, LB_L_Pin, GPIO_PIN_RESET);
        _delay_callback(delay_time);
        HAL_GPIO_WritePin(LB_L_GPIO_Port, LB_L_Pin, GPIO_PIN_SET);
        break;

    case RIGHT:
        HAL_GPIO_WritePin(LB_R_GPIO_Port, LB_R_Pin, GPIO_PIN_RESET);
        _delay_callback(delay_time);
        HAL_GPIO_WritePin(LB_R_GPIO_Port, LB_R_Pin, GPIO_PIN_SET);
        break;

    default:
        break;
    }
}