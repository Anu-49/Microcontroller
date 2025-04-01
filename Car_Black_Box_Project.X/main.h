/* 
 * File:   main.h
 * Author: Anusha C
 *
 * Created on 12 March, 2025, 12:20 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include <xc.h>
#include "car_black_box_def.h"
#include "EEprom.h"
#include "string.h"
#include "timers.h"
#include "uart.h"

#define DASHBOARD_SCREEN        0X00
#define LOGIN_SCREEN            0X01
#define MAIN_MENU_SCREEN        0X02

#define RESET_PASSWORD           0x11
#define RESET_NOTHING            0x22
#define NO_LOGS                  0x23
#define RETURN_BACK              0x33
#define LOGIN_SUCCESS            0x44
#define RESET_MENU               0x55
#define RESET_VIEW               0x56
#define RESET_TIME               0x57
#define RESET_PWD                0x58

#define TIME_SET                 0x60
#define VIEW_LOG_SCREEN          0x61
#define CLEAR_LOG_SCREEN         0x62
#define DOWLOAD_LOG_SCREEN       0x63
#define SET_TIME_SCREEN          0x64
#define CHANGE_PASSWORD_SCREEN   0x65

#define PWD_CHANGE_SUCCESS       0x70
#define PWD_CHANGE_NS            0x71

#endif	/* MAIN_H */

