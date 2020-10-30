#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "struct.hpp"
#include "rtos.hpp"
#include "hwlib.hpp"

class DisplayControl{
private:
    //oled
    hwlib::glcd_oled& oled;
    //window
    hwlib::window& w1;
    hwlib::window& w2;
    hwlib::window& w3;
    //font
    hwlib::font_default_8x8 f1;
    //terminal
    hwlib::terminal_from d1; //tijd en countdown
    hwlib::terminal_from d2; //stats en invoer
    hwlib::terminal_from d3; //stats en invoer

    //rtos::timer display_timer = {this, "display_timer"};

public:
    DisplayControl(hwlib::glcd_oled& oled_, hwlib::window& w1_, hwlib::window& w2_, hwlib::window& w3_):
            oled(oled_),
            w1(w1_),
            w2(w2_),
            w3(w3_),
            d1(w1,f1),
            d2(w2,f1),
            d3(w3,f1)
    {
        d1 << "\f" << "";
        d2 << "\f" << "";
        d3 << "\f" << "";
        oled.flush();
    }

    void SetDisplayHit(const hit& input){ //d2
        d2  << "\f" << "HIT! - " << input.damage << "health";
        oled.flush();
        //hwlib::wait_ms(1000);
        /*if(receive_hit == true){

        }else if (gamesettings_receive == true){

        }else{
            d2 << "\f"
               << hwlib::flush;
        }*/
    }

    void SetDisplayStats(const game_parameters_stuct& input) { //d3
        //d2 << "\f" << "Setting received!";
        //oled.flush();
        //health:
        //power:
    }

    void SetDisplayInput(const char* input, const int& n, const char* input2){ //d3
        d3  << "\f" << input << n << input2;
        oled.flush();
    }

    void SetDisplayMessageD1(const char* input){ //d1
        d1  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageD2(const char* input){ //d2
        d2  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageD3(const char* input){ //d3
        d3  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageFadeD2(const char* input/*, long long const int& fade_delay*/){ //d2
        d2  << "\f" << input;
        oled.flush();
        //display_timer.set(fade_delay);
        //wait(display_timer);
        hwlib::wait_ms(1000);
        d2  << "\f" << "";
        oled.flush();
    }

    void SetDisplayTime(const int& input){ //d1
        d1 << "Time left:\n" << input << "\n";
        oled.flush();
    }

    void SetDisplayCountdown(const int& input){ //d1
        d1 << "Start game in:\n" << input << "\n";
        oled.flush();
    }

    /*SetDisplayMessageD2() //d2
    SetDisplayGameStatus(health, time) // d1
    SetShowInvoer() //d2

    //----------------------------

    stats
    //----------------------------
    invoer / message
    hit()

    //----------------------------
    */
};

#endif //DISPLAY_HPP
