/*
 * File:   main.c
 * Author: Anusha C
 *
 * Created on 12 March, 2025, 12:20 PM
 * 
 * DOCUMENTATION:
 * 
 * The CAR BLACK BOX project:
 * Implemented to record all the events occuring in a car, any event change i.e, gear change, time change, speed etc is recorded and stored in the EEPROM memory of the system.
 * The proposed solution is to log all the critical events like the gear shifts with current speed, the engine temperature, fuel consumption per trip, trip distance etc.,
 * The system should allow a password based access to the transport managers to view or download the log to PC if required.
 * 
 * FUNCTIONALITY:
 * Note: SP - short press, LP - long press
 * 1. DASHBOARD SCREEN: The default screen where time, gear, speed is displayed on the CLCD.
 * Press the DKP switch for: 
 *      SP SW1 - collision
 *      SP SW2 - increment gear
 *      SP SW3 - decrement gear
 *      SP SW4 or SW5 - Enter Login screen
 * 2. LOGIN SCREEN: To view main menu we should login successfully by entering correct password which is a combination of 1's and 0's.
 *      SP SW4 - to enter '1' as password digit
 *      SP SW5 - to enter '0' as password digit
 *      When password entered successfully you will be navigated to MAIN MENU screen. Will be having 3 attempts to enter the password.
 *      If failed to enter correct password you will be blocked for a minute.
 * 3. MAIN MENU SCREEN: 
 * Menu with "view logs", "clear logs", "download logs", "set time", "change password" options is displayed.
 * Use SW4 and SW5 to hover/move up and down the options respectively.
 * LP SW5 to select the option and you will be navigated to the selected option screen.
 *  -> VIEW LOGS screen: SP SW4 to view previous log and SP SW5 to view next log. LP SW4 to go to Main menu screen.
 *  -> CLEAR LOGS screen: LP SW5 to clear logs.
 *  -> DOWNLOAD LOGS screen: After downloading logs main menu will be displayed.
 *  -> SET TIME screen: SP SW4 to increment the field values of SS or MM or HH filed.
 *      SP SW5 to move to next field either SS or MM or HH.
 *      SP SW3 to save time. LP SW4 to dashboard screen.
 *  -> CHANGE PASSWORD SCREEN: SP SW4 for '1' and SW5 for '0' password to change the password.
 * 
 * Note: If there's inactivity for longer time then default screen will be displayed.
 */

#include "main.h"
#pragma config WDTE = OFF

void init_config()
{
//   initialize i2c
    init_i2c(10000);
//   initialize rtc
    init_ds1307();
//    configure adc
    init_adc();
//    initialize clcd
    init_clcd();
//    initialize DKP
    init_digital_keypad();
//    initialize uart
    init_uart(9600);
//    initialize timer2
    init_timer2();
    /* Peripheral Interrupt Enable Bit (For Timer2) */
    PEIE = 1;
    /* Enable all the Global Interrupts */
    GIE = 1;
}


void main()
{
    unsigned char control_flag = DASHBOARD_SCREEN; //control_flag: to track the Screen, Key: to read DKP input
    unsigned char reset_flag; // to check for reset.
    unsigned char menu_position; // tracks Menu options [menu array position]
    unsigned char event[3] = "ON"; // read gear
    unsigned char speed = 0; // read speed from potentiometer]
    unsigned char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4", "G5"}; // gear values
//                            0     1     2     3     4     5     6
    unsigned char gr = 0;
    unsigned char key = ALL_RELEASED, pressed_key = ALL_RELEASED, stored;
    unsigned long int delay = 0;
    
    init_config();
    
    log_event(event,speed); // to track and store logs
    ext_eeprom_24C02_str_write(0x00, "1010");  // store the password in eeprom
    
    while(1)
    {
        speed = read_adc() / 10.3; //  ADC has 10 bit resolution (range 1-1023) to convert this to 0-99 range of speed : /10.3.
        __delay_ms(10);
        key = read_digital_keypad(LEVEL);
        
        if(key != ALL_RELEASED)
        {
            stored = key;
            delay++;
        }
        else
        {
            pressed_key = stored;
            stored = ALL_RELEASED;
//            delay = 0; //added
        }
        for(int i=0; i<3000; i++);
            
        if(key == ALL_RELEASED)
        {
            if(pressed_key == SW1)
            {
//            collision
                gr = 0;
                strcpy(event, "CO");
                log_event(event,speed);
            }
            else if(pressed_key == SW2 && gr < 7)
            {
//            increment the gear
                strcpy(event, gear[gr]);
                gr++;
                log_event(event,speed);            
            }
            else if(pressed_key == SW3 && gr > 0)
            {
//            decrement the gear
                gr--;
                strcpy(event, gear[gr]);
                log_event(event,speed);
            }
            else if( (pressed_key == SW4 || pressed_key == SW5) && control_flag == DASHBOARD_SCREEN )
            {
                control_flag = LOGIN_SCREEN;
//            To clear the screen before asking user to for password
                clcd_write(CLEAR_DISP_SCREEN,INST_MODE);
                __delay_us(500);
                clcd_print("Enter Password", LINE1(1));
//            To blink the cursor at 4th position in line 2 (instruction mode)
                clcd_write(LINE2(4),INST_MODE);
                clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                __delay_us(100);
                reset_flag = RESET_PASSWORD;
                TMR2ON = 1;
            }
            else if(pressed_key == SW4 && delay > 5)
            {
                if(control_flag == MAIN_MENU_SCREEN)
                {
                    clear_screen();
                    control_flag = DASHBOARD_SCREEN;
                }
                else if(control_flag == VIEW_LOG_SCREEN){
                    clear_screen();
                    control_flag = MAIN_MENU_SCREEN;
                    reset_flag = RESET_MENU;
                }
                else if(control_flag == SET_TIME_SCREEN)
                {
                    clear_screen();
                    control_flag = DASHBOARD_SCREEN; //return to dashboard.
                    reset_flag = RESET_NOTHING;
                }   
            }
            else if(pressed_key == SW5 && control_flag == MAIN_MENU_SCREEN && delay > 5)
            {
                    switch(menu_position)
                    {
                        case 0:
                            control_flag = VIEW_LOG_SCREEN;
                            reset_flag = RESET_VIEW;
                            clear_screen();
                            break;
                        case 1:
                            control_flag = CLEAR_LOG_SCREEN;
                            break;
                        case 2:
                            control_flag = DOWLOAD_LOG_SCREEN;
                            clear_screen();
                            clcd_print("DOWNLOADING....." , LINE1(0));
                            __delay_ms(1500);
                            break;
                        case 3:
                            control_flag = SET_TIME_SCREEN;
                            clear_screen(); //clear before entering set_time screen
                            reset_flag = RESET_TIME;
                            break;
                        case 4:
                            control_flag = CHANGE_PASSWORD_SCREEN;
                            clear_screen();
                            clcd_print("ENTER NEW PASS :" , LINE1(0));
                            reset_flag = RESET_PWD;  
                            break; 
                    }
            }  
            else if((pressed_key == SW5 || pressed_key == SW4) && control_flag == CLEAR_LOG_SCREEN)
            {
                control_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_MENU;
            }
            delay = 0;
        }
        
        switch(control_flag)
        {
            case DASHBOARD_SCREEN:
                display_dashboard(event, speed);
                break;
            case LOGIN_SCREEN:
                switch(login(pressed_key, reset_flag))
                {
                    case RETURN_BACK:
                        clear_screen();
                        control_flag = DASHBOARD_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        TMR2ON = 0;
                        break;
                    case LOGIN_SUCCESS:
                        clear_screen();
                        control_flag = MAIN_MENU_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        reset_flag = RESET_MENU;
                        continue;
                }
                break;
            case MAIN_MENU_SCREEN:
                switch(menu_position = menu_screen(pressed_key, reset_flag))
                {
                    case RETURN_BACK:
                        clear_screen();
                        control_flag = DASHBOARD_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        continue;
                }
                break;
            case VIEW_LOG_SCREEN:
                switch(view_log(pressed_key,reset_flag))
                {
                    case NO_LOGS:
                        clear_screen();
                        control_flag = MAIN_MENU_SCREEN;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        continue;
                }
                break;
            case CLEAR_LOG_SCREEN:
                clear_log(pressed_key);
                __delay_ms(2000);
                clear_screen();
                control_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_MENU; //add reset menu flag.
                break;
            case DOWLOAD_LOG_SCREEN:
                download_log();
                control_flag = MAIN_MENU_SCREEN;
                reset_flag = RESET_MENU;
                break;
            case SET_TIME_SCREEN:
                clear_screen();
                if(set_time(pressed_key, reset_flag) == TIME_SET){
                    control_flag = DASHBOARD_SCREEN;
                    reset_flag = RESET_NOTHING; // reset flag.
                }   
                break;
            case CHANGE_PASSWORD_SCREEN:
                if(change_pwd(pressed_key, reset_flag) == PWD_CHANGE_SUCCESS){
                    clear_screen();
                    control_flag = DASHBOARD_SCREEN;
                    reset_flag = RESET_MENU;
                }
                else if(change_pwd(pressed_key, reset_flag) == PWD_CHANGE_NS){
                    reset_flag = RESET_MENU;                          
                    control_flag = MAIN_MENU_SCREEN;
                    clear_screen();
                }
                break;
        }
        reset_flag = RESET_NOTHING;
    }
}