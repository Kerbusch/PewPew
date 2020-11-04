#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "struct.hpp"
#include "rtos.hpp"
#include "hwlib.hpp"

/// @brief DisplayControl Class
/// @details Class for controlling an oled display.
class DisplayControl{
private:
    //oled
    /// @brief Reference of hwlib::glcd_oled as a menber
    hwlib::glcd_oled& oled;
    //window
    /// @brief A Reference hwlib::window member of the top part of the display
    hwlib::window& w1;
    /// @brief A Reference hwlib::window member of the middle part of the display
    hwlib::window& w2;
    /// @brief A Reference hwlib::window member of the bottom part of the display
    hwlib::window& w3;
    //font
    /// @brief A hwlib::font_default_8x8 for use with the terminal
    hwlib::font_default_8x8 f1;
    //terminal
    /// @brief A hwlib::terminal_from member of the top part of the display
    hwlib::terminal_from d1; //tijd en countdown
    /// @brief A hwlib::terminal_from member of the top middle of the display
    hwlib::terminal_from d2; //stats en invoer
    /// @brief A hwlib::terminal_from member of the top bottom of the display
    hwlib::terminal_from d3; //stats en invoer

public:
    /// @brief Constructor for the DisplayControl class
    /// @details This constructor initializes the reference members, makes the terminals and clears the display on construction
    /// @param oled_ A hwlib::glcd_oled class
    /// @param w1_ A hwlib::window of the top of the display
    /// @param w2_ A hwlib::window of the middle of the display
    /// @param w3_ A hwlib::window of the bottom of the display
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

    /// @brief Function for flushing a message which shows the received damage.
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

    /// @brief Function for printing the stats on the display.
    void SetDisplayStats(const game_parameters_struct& input) { //d3
        d3 << "Health: " << input.health << "\nPower: " << input.power << "Number: " << input.number;
        oled.flush();
    }

    /// @brief Function for printing the Input on the display.
    void SetDisplayInput(const char* input, const int& n, const char* input2){ //d3
        d3  << "\f" << input << n << input2;
        oled.flush();
    }

    /// @brief Function for printing a message on the display part d1.
    void SetDisplayMessageD1(const char* input){ //d1
        d1  << "\f" << input;
        oled.flush();
    }

    /// @brief Function for printing a message on the display part d2.
    void SetDisplayMessageD2(const char* input){ //d2
        d2  << "\f" << input;
        oled.flush();
    }

    /// @brief Function for printing a message on the display part d3.
    void SetDisplayMessageD3(const char* input){ //d3
        d3  << "\f" << input;
        oled.flush();
    }

    /// @brief Function for printing a message on the display part d2 that fades after 1 second.
    void SetDisplayMessageFadeD2(const char* input/*, long long const int& fade_delay*/){ //d2
        d2  << "\f" << input;
        oled.flush();
        //display_timer.set(fade_delay);
        //wait(display_timer);
        hwlib::wait_ms(1000);
        d2  << "\f" << "";
        oled.flush();
    }

    /// @brief Function for printing the time on the display.
    void SetDisplayTime(const int& input){ //d1
        d1 << "Time left:\n" << input << "\n";
        oled.flush();
    }

    /// @brief Function for printing the countdown on the display.
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
