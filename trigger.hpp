#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

#include "speaker.hpp"

/// @brief Trigger Class
/// @details ...
class Trigger: public rtos::task<>{
private:
    /// @brief gpio pin for the led
    hwlib::pin_direct_from_out_t led;
    /// @brief gpio pin for the switch/trigger
    hwlib::pin_direct_from_in_t sw;

    /// @brief SpeakerControl member for enabling the play sound flags
    SpeakerControl& speakerControl;
    run_game& runGame;

    /// @brief delay for timer
    long long int delay;
    /// @brief rtos::timer for periodical checking of input
    rtos::timer trigger_timer;
    /// @brief NOT RIGHT NEEDS TO BE CHANGED
    rtos::flag trigger_pressed_flag;
    //channel

    /// @brief main funtion for the rtos::task<>. Wil run on rtos::run();
    void main(){
        bool switch_last = false;
        while(true){
            if(!sw.read() && !switch_last){
                switch_last = true;
                runGame.enable_trigger_pressed_flag();
                speakerControl.enable_play_shoot_flag(); //moet vervangen worden door "trigger_pressed_flag" als run game gemaakt is.
                led.write(true);
            }else if(sw.read() && switch_last){
                switch_last = false;
                led.write(false);
            }
            trigger_timer.set(delay);
            wait( trigger_timer );
        }
    }

public:
    /// @brief Constructor of the Trigger class.
    /// @details This constructor sets the pins and speakerControl as members.
    /// @param led_ This is a hwlib::pin_out& for the led of trigger.
    /// @param sw_ This is a hwlib::pin_in& for the switch of the trigger.
    /// @param speakerControl_ This is a SpeakerControl for setting flags.
    Trigger(hwlib::pin_out& led_, hwlib::pin_in& sw_, SpeakerControl& speakerControl_, run_game& runGame_):
            task(250, "trigger"),
            led(led_),
            sw(sw_),
            speakerControl(speakerControl_),
            runGame(runGame_),
            delay(30 * rtos::ms),
            trigger_timer( this, "trigger_timer"),
            trigger_pressed_flag(this, "trigger_pressed_flag")
    {}
};

#endif //TRIGGER_HPP
