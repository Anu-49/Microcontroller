/* 
 * File:   car_black_box_def.h
 * Author: Anusha C
 *
 * Created on 12 March, 2025, 1:35 PM
 */

#ifndef CAR_BLACK_BOX_DEF_H
#define	CAR_BLACK_BOX_DEF_H

void display_dashboard(unsigned char event[], unsigned char speed);
void log_event(unsigned char event[], unsigned char speed);
unsigned char login(unsigned char key, unsigned char reset_flag);
void clear_screen();
unsigned char menu_screen(unsigned char key, unsigned char reset_flag);
unsigned char view_log(unsigned char key, unsigned char reset_flag);
void fetch_logs(char i);
void clear_log(unsigned char key);
void download_log();
unsigned char set_time(unsigned char key, unsigned char reset_flag);
void save_time(char *ntime);
unsigned char change_pwd(unsigned char key, unsigned char reset_flag);

#endif	/* CAR_BLACK_BOX_DEF_H */

