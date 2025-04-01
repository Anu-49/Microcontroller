/* Contains definitions of car black box functions */
#include "main.h"

int events_logged = 0; //to track the number of events recorded, required to navigate while viewing, deleting and downloading logs.
unsigned char clock_reg[3];
char time[7]; //"HHMMSS"
char log[11]; //HHMMSSEVSP
//to track the number of logs. [index of log[] array]
int log_pos = -1;
unsigned char sec; // to wait 60 seconds when all 3 attempts failed, user blocked
unsigned char return_time;
unsigned char *menu[] = {"View Log", "Clear Log", "Download Log", "Set Time", "Change Paswd"};
unsigned char menu_pos; //track index of menu[] array - position.
unsigned char clear_flag; // flag to check if logs present or empty 
char get_log[17]; // array to store data fetched from EEPROM

static void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
//    to store the time in "HHMMSS" format
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0'; 
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
    
}

void display_time()
{
   get_time();
//   HH:MM:SS
   clcd_putch(time[0], LINE2(2));
   clcd_putch(time[1], LINE2(3));
   clcd_putch(':', LINE2(4));
   clcd_putch(time[2], LINE2(5));
   clcd_putch(time[3], LINE2(6));
   clcd_putch(':', LINE2(7));
   clcd_putch(time[4], LINE2(8));
   clcd_putch(time[5], LINE2(9));
   
}

// display dash_board function definition

void display_dashboard(unsigned char event[], unsigned char speed)
{
    clcd_print("TIME     E  SP", LINE1(2));
    //    display time
    display_time();
    //    display event
    clcd_print(event,LINE2(11));
    //    display speed
    clcd_putch(speed/10 + '0', LINE2(14));
    clcd_putch(speed%10 + '0', LINE2(15));   
}

//to store contents of log[] array to the EEPROM
void store_event()
{
    char addr;
    log_pos++;
    if(log_pos == 10){
        log_pos = 0;
    }
    addr = 0x05 + (log_pos * 10);
    if(events_logged < 10)
        events_logged++;
    ext_eeprom_24C02_str_write( addr , log);
    
}

//storing the log events to a log[] array
void log_event(unsigned char event[], unsigned char speed)
{
    get_time();
    strncpy(log, time, 6);
    strncpy(&log[6], event, 2);
    
//    store speed
    log[8] = speed / 10 + '0';
    log[9] = speed % 10 + '0';
    log[10] = '\0'; 
    
    store_event();
    clear_flag = 0;
}

//
unsigned char login(unsigned char key, unsigned char reset_flag)
{
    static char user_password[4]; //store 4 digit password which is static
    static unsigned char i; // index of user_password array
    static unsigned char attempts_left;
    if(reset_flag == RESET_PASSWORD)
    {
        i= 0;
        attempts_left = 3;
        user_password[0] = '\0';
        user_password[1] = '\0';
        user_password[2] = '\0';
        user_password[3] = '\0';
//  previously key will have SW4/SW5 since we pressed SW4/SW5 to enter into LOGIN_SCREEN
//  so we need to reset key by assigning ALL_RELEASED macro to it, so that further if SW4 or SW5 is pressed respective operations can be performed.
        key = ALL_RELEASED;
        return_time = 10;
    }
    
    if(return_time == 0)
    {
//    change to dashboard
        return RETURN_BACK;
    }
//    read input from the user if SW4 pressed 1 is the value, if SW5 pressed 0 is the value.
//    The password is a combination of 1's and 0's.
    if(key == SW4 && i < 4)
    {
       clcd_putch('*', LINE2(4+i));
       user_password[i] = '1';
       i++;
       return_time = 10;
    }
    else if (key == SW5 && i < 4)
    {
        clcd_putch('*', LINE2(4+i));
        user_password[i] = '0';
        i++;
        return_time = 10;
    }
    
//   i==4 : to check if all 4 digit of password input is read or not.
    if(i == 4)
    {
        char stored_pwd[4];
//        read stored password from eeprom
        for(int j=0; j<4; j++)
        {
            stored_pwd[j] = ext_eeprom_24C02_read(j);
        }
//        compare the stored password and user passwords.
        if(strncmp(stored_pwd, user_password, 4) == 0)
        {
            clear_screen();
            clcd_print("LOGIN SUCCESS", LINE1(1));
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            __delay_ms(3000);
//            change to MENU screen
            return LOGIN_SUCCESS;
        }
        else
        {
            attempts_left--;
            if(attempts_left == 0)
            {
//                display the block screen
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are BLOCKED", LINE1(0));
                clcd_print("Wait for", LINE2(0));
//              wait for 60 sec, run timer2 (can enable only while waiting 60sec)
                sec = 60;
                
//                until sec == 0, display sec
                while(sec)
                {
                    clcd_putch(sec/10 + '0', LINE2(9));
                    clcd_putch(sec%10 + '0', LINE2(10));
                    clcd_print("secs", LINE2(12));
                }
                attempts_left = 3;
            }
            else
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("Wrong Password", LINE1(0));
                clcd_print("attempts left", LINE2(2));
                
                clcd_putch(attempts_left + '0', LINE2(0));
                __delay_ms(3000);
            }
            clear_screen();
            
            clcd_print("Enter Password", LINE1(1));
//            To blink the cursor at 4th position in line 2 (instruction mode)
            clcd_write(LINE2(4),INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
        }     
    }
}

//function to define menu_screen operations
unsigned char menu_screen(unsigned char key, unsigned char reset_flag)
{
    if(reset_flag == RESET_MENU)
    {
        menu_pos = 0;
        return_time = 10;
    }
    if(return_time == 0)
    {
//    change to dashboard
        return RETURN_BACK;
    }
    if(key == SW4 && menu_pos > 0)
    {
//        decrement menu position
        clear_screen();
        menu_pos--;
        return_time = 10;
    }
    else if(key == SW5 && menu_pos < 4)
    {
//        increment menu position
        clear_screen();
        menu_pos++;
        return_time = 10;
    }
    if(menu_pos == 4)
    {
        clcd_putch('*', LINE2(1));
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    }
    else
    {
        clcd_putch('*', LINE1(1));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));
    }
    return menu_pos;
}

unsigned char view_log(unsigned char key, unsigned char reset_flag)
{
    static char log_pos = 0;
    if(reset_flag == RESET_VIEW)
    {
        key = ALL_RELEASED;
        reset_flag = RESET_NOTHING;
        log_pos = 0;
    }
        
    if(clear_flag == 0)
    {
        if(key == SW5 && events_logged > 0)
        {
            clear_screen();
            log_pos++; // Increment log_pos 
            log_pos = log_pos%events_logged;
        }
        if(key == SW4 && events_logged <= 10)
        {
            clear_screen();
            if(log_pos == 0)
            {
                log_pos = events_logged; // set log_pos to 9 when it's < 0
            }   
            log_pos--; // else decrement log_pos
            log_pos = log_pos%events_logged;
        }
        clcd_print("L TIME     E  SP", LINE1(0));
        fetch_logs(log_pos);
        clcd_print(get_log, LINE2(0));
    }
    else if(clear_flag == 1 || events_logged == 0){
        clear_screen();
        clcd_print("No logs found", LINE1(1));
        __delay_ms(3000);
        return NO_LOGS;
    }
}

void fetch_logs(char i)
{
    unsigned char addr = 0x05 + i*10;
    get_log[0] = i+'0';
    get_log[1] = ' ';
    get_log[2] = ext_eeprom_24C02_read(addr);
    get_log[3] = ext_eeprom_24C02_read(addr+1);
    get_log[4] = ':';
    get_log[5] = ext_eeprom_24C02_read(addr+2);
    get_log[6] = ext_eeprom_24C02_read(addr+3);
    get_log[7] = ':';
    get_log[8] = ext_eeprom_24C02_read(addr+4);
    get_log[9] = ext_eeprom_24C02_read(addr+5);
    get_log[10] = ' ';
    get_log[11] = ext_eeprom_24C02_read(addr+6);
    get_log[12] = ext_eeprom_24C02_read(addr+7);
    get_log[13] = ' ';
    get_log[14] = ext_eeprom_24C02_read(addr+8);
    get_log[15] = ext_eeprom_24C02_read(addr+9);
    get_log[16] = '\0';
    
}

void clear_log(unsigned char key)
{
    if(clear_flag == 0)
    {
        clear_flag = 1;
        clear_screen();
        char temp[2] = {0xFF, '\0'}; // Buffer to write 0xFF as a string (null-terminated)
        for(unsigned char i = 0; i < 100; i++)
        {
            ext_eeprom_24C02_str_write(0x05 + i, temp);
        }
        clcd_print("Logs Cleared", LINE1(1));
        clcd_print("Successfully", LINE2(1));
        log_pos = 0;
    }
}
void download_log()
{
    if(events_logged == 0 || clear_flag == 1)
    {
        clear_screen();
        clcd_print("NO LOGS", LINE1(2));
        clcd_print("FOUND", LINE2(2));
        __delay_ms(2000);
        clear_screen();
        return;
    }
    puts("Log: \n");
    for(int i=0; i<events_logged; i++)
    {
        fetch_logs(i);
        puts(get_log);
        puts("\n");
    }  
    
}
  
// function to set time as per user's choice
unsigned char set_time(unsigned char key, unsigned char reset_flag)
{
    static unsigned char field = 2;
    static unsigned char blink = 0;
    static char settime[9];
    
    if(reset_flag == RESET_TIME)
    {
        get_time();
        strncpy(settime, time, 6);
        settime[2] = ':';
        settime[5] = ':';
        settime[8] = '\0';
        field = 2;
        blink = 0;
        key = ALL_RELEASED;
    }
    
    blink = !blink;
    
    clear_screen(); 
    clcd_print("SET TIME:", LINE1(0));
    for(int i=0; i<8; i++)
    {
        if(field == 2 && i>=6 && i<=7 && blink)
        {
            clcd_putch(' ',LINE2(i));
        }
        else if(field == 1 && i >= 3 && i<=4 && blink)
        {
            clcd_putch(' ', LINE2(i));
        }
        else if (field == 0 && i>=0 && i<=1 && blink)
        {
            clcd_putch(' ', LINE2(i));
        }
        else
        {
            clcd_putch(settime[i], LINE2(i));
        }    
    }
    if(key == SW4)
    {
        if(field == 2)
        {
            unsigned char sec_high = (settime[6]-'0')*10;
            unsigned char sec_low = settime[7]-'0';
            unsigned char sec = sec_high + sec_low;
            sec++;
            if(sec>59)
            {
                sec=0;
            }
            settime[6] = (sec/10)+'0';
            settime[7] = (sec%10)+'0';
        }
        else if(field == 1)
        {
            unsigned char min_high = (settime[3]-'0')*10;
            unsigned char min_low = settime[4]-'0';
            unsigned char min = min_high+min_low;
            min++;
            if(min>59)
            {
                min = 0;
            }
            settime[3] = (min/10)+'0';
            settime[4] = (min%10)+'0';
        }
        else if (field==0)
        {
           unsigned char hour_high = (settime[0] - '0') * 10;
            unsigned char hour_low = settime[1] - '0';
            unsigned char hour = hour_high + hour_low;
            hour++;
            if (hour > 23)
            {
                hour = 0;
            }
            settime[0] = (hour/10)+'0';
            settime[1] = (hour%10)+'0';
        }      
    }
    else if(key == SW5)
    {
        field--;
        if(field>2)
        {
            field = 2;
        }
    }
    else if(key == SW3){
        save_time(settime);
        clear_screen();
        clcd_print("Time Saved!", LINE1(2));
        __delay_ms(2000);
        return TIME_SET;
    }
    __delay_ms(10);
    return 0;
}
//save time 
void save_time(char *ntime)
{
    unsigned char hour, minute, second;
    hour = ((ntime[0]-'0')*10)+(ntime[1]-'0');
    minute = ((ntime[3]-'0')*10)+(ntime[4]-'0');
    second = ((ntime[6]-'0')*10)+(ntime[7]-'0');
    
    unsigned char bcdh = ((hour/10)<<4)|(hour%10);
    unsigned char bcdm = ((minute/10)<<4)|(minute%10);
    unsigned char bcds = ((second/10)<<4)|(second%10);
    
    write_ds1307(HOUR_ADDR, bcdh);
    write_ds1307(MIN_ADDR, bcdm);
    write_ds1307(SEC_ADDR, bcds);
}

//function to change password
unsigned char change_pwd(unsigned char key, unsigned char reset_flag)
{
    static char i = 0, new_pwd[4], reenter_pwd[4], clear = 0;
    if(reset_flag == RESET_PWD)
    {
        for(int i=0; i<4; i++)
        {
            new_pwd[i] = '\0';
            reenter_pwd[i] = '\0';
        }
        key = ALL_RELEASED;   
        clear = 1;
        i = 0;
        reset_flag = RESET_NOTHING;
        clcd_write(0xC2 , INST_MODE);
        __delay_us(100);                           
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
    }
    
    if(key == SW4)
    {
        if(i<4){
            new_pwd[i] = '1';
        }
        else{
            reenter_pwd[i%4] = '1';
        }
            clcd_putch('*',0xC2+(i%4));
            i++;
        
    }
    
    if(key == SW5)
    {
        if(i<4){
            new_pwd[i] = '0';
        }
        else{
            reenter_pwd[i%4] = '0';
        }
            clcd_putch('*',0xC2+(i%4));
            i++;
        
    }
    if(i==4 && clear){
        clear = !clear;
        clear_screen();
        clcd_print("RE-ENTER PASS :" , LINE1(0));         
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);   
        __delay_us(100);
        clcd_write(0xC2 , INST_MODE);
        __delay_us(100);                           
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
    }
    if(i==8){
        clear_screen();
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);   
        __delay_us(100);
        if(strncmp(new_pwd, reenter_pwd,4)==0){
            for(int i=0; i<4; i++){
                ext_eeprom_24C02_str_write(i,&new_pwd[i]);
            }
            clcd_print("Password change", LINE1(0));
            clcd_print("successful..",LINE2(0));
            __delay_ms(200);
            return PWD_CHANGE_SUCCESS;      
        }
        else
        {
           clcd_print("NOT MATCHED..",LINE1(0));        
           __delay_ms(2000);
           return PWD_CHANGE_NS;
        }
        clear_screen();
    }       
}

//function to clear the clcd screen
void clear_screen()
{
    clcd_write(CLEAR_DISP_SCREEN,INST_MODE);
    __delay_us(500);
}
    