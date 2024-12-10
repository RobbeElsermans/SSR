#include "linebot.h"

void driveLineBot(DRIVE_STATE action) {
    HAL_GPIO_WritePin(LB_B_GPIO_Port, LB_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_R_GPIO_Port, LB_R_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_F_GPIO_Port, LB_F_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_L_GPIO_Port, LB_L_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LB_S_GPIO_Port, LB_S_Pin, GPIO_PIN_SET);

    // Pins are active low
    switch (action) {
    case STOP:
        HAL_GPIO_WritePin(LB_S_GPIO_Port, LB_S_Pin, GPIO_PIN_RESET);
        break;

    case FORWARD:
        HAL_GPIO_WritePin(LB_F_GPIO_Port, LB_F_Pin, GPIO_PIN_RESET);
        break;

    case BACKWARD:
        HAL_GPIO_WritePin(LB_B_GPIO_Port, LB_B_Pin, GPIO_PIN_RESET);
        break;

    case LEFT:
        HAL_GPIO_WritePin(LB_L_GPIO_Port, LB_L_Pin, GPIO_PIN_RESET);
        break;

    case RIGHT:
        HAL_GPIO_WritePin(LB_R_GPIO_Port, LB_R_Pin, GPIO_PIN_RESET);
        break;

    default:
        break;
    }
}