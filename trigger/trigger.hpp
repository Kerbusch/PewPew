//
// Created by daan on 28-10-20.
//

#ifndef TRIGGER_HPP
#define TRIGGER_HPP

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

    /// @brief delay for timer
    long long int delay;
    /// @brief rtos::timer for periodical checking of input
    rtos::timer trigger_timer;
    /// @brief NOT RIGHT NEEDS TO BE CHANGED
    rtos::flag trigger_pressed_flag;

    /// @brief main funtion for the rtos::task<>. Wil run on rtos::run();
    void main(){
        bool switch_last = false;
        while(true){
            trigger_timer.set(delay);
            if(!sw.read() && !switch_last){
                switch_last = true;
                speakerControl.enable_play_shoot_flag(); //moet vervangen worden door "trigger_pressed_flag" als run game gemaakt is.
                led.write(true);
            }else if(sw.read() && switch_last){
                switch_last = false;
                led.write(false);
            }
            wait( trigger_timer );
        }
    }

public:
    /// @brief Constructor of the Trigger class.
    /// @details This constructor sets the pins and speakerControl as members.
    /// @param led_ This is a hwlib::pin_out& for the led of trigger.
    /// @param sw_ This is a hwlib::pin_in& for the switch of the trigger.
    /// @param speakerControl_ This is a SpeakerControl for setting flags.
    Trigger(hwlib::pin_out& led_, hwlib::pin_in& sw_, SpeakerControl& speakerControl_):
            rtos::task<>(5),
            led(led_),
            sw(sw_),
            speakerControl(speakerControl_),
            delay(30 * rtos::ms),
            trigger_timer( this, "trigger_timer"),
            trigger_pressed_flag(this, "trigger_pressed_flag")
    {}
};

#endif //TRIGGER_HPP
