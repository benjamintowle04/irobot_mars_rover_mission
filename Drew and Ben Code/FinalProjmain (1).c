/**
 * Final Project Main
 * @author Andrew Vick
 * 4/17/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"
#include "uart-interrupt.h" //used for sending and receiving through PuTTy
#include "adc.h"    //Better PING and IR values
#include "open_interface.h" //CyBot API
#include "movement.h"   //Controls the movement of the cyBot and object checks
#include "servo.h"
#include "ping_template.h"
#include "scan.h"

#define NOTE_E4 52
#define NOTE_G6 67
#define NOTE_F5 66
#define NOTE_D3 50
unsigned char Love_SosaNotes[] = { NOTE_E4, NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5,
                                   NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5, NOTE_E4,
                                   NOTE_D3, NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5,
                                   NOTE_E4, NOTE_F5, NOTE_G6, NOTE_F5, NOTE_E4,
                                   NOTE_F5, NOTE_E4, NOTE_D3, NOTE_G6, NOTE_E4,
                                   NOTE_F5, NOTE_E4, NOTE_F5, NOTE_G6, NOTE_F5,
                                   NOTE_E4, NOTE_F5, NOTE_E4, NOTE_D3, NOTE_G6,
                                   NOTE_F5, NOTE_E4, NOTE_F5 }; // NOTE_E4, NOTE_F5,NOTE_G6, NOTE_F5,NOTE_E4, NOTE_F5, NOTE_E4, NOTE_D3, NOTE_D3, NOTE_G6, NOTE_F5,NOTE_E4, NOTE_F5}; //end of thrid segment
unsigned char duration[] = { 20, 20, 20, 60, 40, 20, 20, 20, 20, 40, 40, 20, 20,
                             20, 20, 40, 40, 20, 20, 20, 20, 40, 40, 40, 20, 20,
                             40, 40, 20, 20, 20, 20 }; //, 49, 49, 20, 20, 20, 20, 49, 20, 20, 20, 20, 20, 20};

int song_index = 0;
int num_notes = 38; //51



main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
//    lcd_init();
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    uart_interrupt_init();
    adc_init();
    ping_init();
    servomotor_init();
    servomotor_move(90);

    /**
     *
     *Switch Scan to use our servo code and not scan library
     *
     */

    while (true)
    {
        if (command_flag == 1)
        {

            if (command_byte == 'w')
            {

                move_forward_checks(200, sensor_data); //moves forward correctly with the correct distance given(bug: if i call it multiple time i.e. hitting 'w' after movement is complete eventually it'll move for longer than suppose to)
                command_flag = 0;
            }
            else if (command_byte == 's')
            {
                backup(sensor_data, 80, 0);
                command_flag = 0;
            }
            else if (command_byte == 'a') //'a' and 'd' are used to turn the cybot in 15 degree chunks
            {
                turn_left(sensor_data, 45);
                command_flag = 0;
            }
            else if (command_byte == 'd')
            {
                turn_right(sensor_data, 45);
                command_flag = 0;
            }
            else if (command_byte == 'e')
            { //'e' and 'q' will be used to turn the cybot a finite amount for lining up entry into final goal
                turn_right(sensor_data, 1);
                command_flag = 0;
            }
            else if (command_byte == 'q')
            {
                turn_left(sensor_data, 1);
                command_flag = 0;
            }
            else if (command_byte == 't')
            {
                scan_Objects();
                command_flag = 0;
            }
            else if (command_byte == 'p')
            {
                oi_loadSong(song_index, num_notes, Love_SosaNotes, duration); //Loads love sosa
                oi_play_song(song_index);
                command_flag = 0;
            }

        }
        else
        {
            continue;
        }
    }

    oi_free(sensor_data);


}





