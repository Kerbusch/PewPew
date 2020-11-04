#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

/// @brief SpeakerControl class
/// @details Class that provides the usage of a speaker with pre assigned sounds.
class SpeakerControl: public rtos::task<> {
private:
    /// @brief Enum of the different states
    enum state_t{
        Idle,
        Play_hit,
        Play_shoot
    };

    /// @brief Selects the starting state
    state_t state = Idle;

    /// @brief
    hwlib::pin_direct_from_out_t pin;

    ///@brief Timer for pin low and high (frequenty).
    rtos::timer beep_timer = {this, "beep_timer"};
    ///@brief Flag for playing sound when player takes a shot.
    rtos::flag play_shoot_flag = {this, "play_shoot_flag"};
    ///@brief Flag for playing sound when hits are received.
    rtos::flag play_hit_flag = {this, "play_hit_flag"};

    ///@brief Main function for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        while (true) {
            switch (state) {
                case Idle: {
                    //entry
                    //normal
                    auto evt = wait(play_shoot_flag + play_hit_flag);
                    if (evt == play_shoot_flag) {
                        state = Play_shoot;
                    } else {
                        state = Play_hit;
                    }
                    break;
                }case Play_shoot: {
                    //entry
                    peew();
                    //normal
                    state = Idle;
                    break;
                }case Play_hit: {
                    //entry
                    peew(); //nog geen hit geluid gemaakt
                    //normal
                    state = Idle;
                    break;
                }
            }
        }
    }

    ///@brief Function for assigning the sound
    ///@details The function uses the input to create an audiosignal which is playable via the speaker.
    void beep( int f, int d, int fd = 1000 ){
        auto end = hwlib::now_us() + d;
        while( end > hwlib::now_us() ){
            auto p = 500'000 / f;
            f = f * fd / 1000;
            pin.write( 1 );
            pin.flush();
            beep_timer.set(p * rtos::us);
            wait(beep_timer);
            pin.write( 0 );
            pin.flush();
            beep_timer.set(p * rtos::us);
            wait(beep_timer);
        }
    }

    ///@brief Function for assigning the sound of peew sound
    void peew(){
        beep(20'000, 200'000, 990 );
    }
    
    ///@brief Function for assigning the sound of beep3 sound
    void beep3(){
        for( int i = 0; i < 3; i++ ){
            beep( 1'000, 50'000 );
            beep_timer.set(20'000 * rtos::us);
            wait(beep_timer);
        }
    }

public:
    /// @b constructor for the SpeakerControl class
    /// @details This constructor initializes the reference members.
    /// @param pin_ , a hwlib::pin_out pin
    SpeakerControl(hwlib::pin_out& pin_):
            rtos::task<>(150, "SpeakerControl"),
            pin(pin_)
    {
        WDT->WDT_MR = WDT_MR_WDDIS;
    }
    
    ///@brief Function for setting the play shoot flag
    void enable_play_shoot_flag(){
        play_shoot_flag.set();
    }
    
    ///@brief Function for setting the play hit flag
    void enable_play_hit_flag(){
        play_hit_flag.set();
    }
};

#endif //SPEAKER_HPP
