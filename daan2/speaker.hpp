#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class SpeakerControl: public rtos::task<> {
private:
    enum state_t{
        Idle,
        Play_hit,
        Play_shoot
    };

    state_t state = Idle;

    hwlib::pin_direct_from_out_t pin;

    rtos::timer beep_timer = {this, "beep_timer"};
    rtos::flag play_shoot_flag = {this, "play_shoot_flag"};
    rtos::flag play_hit_flag = {this, "play_hit_flag"};

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

    void peew(){
        beep(20'000, 200'000, 990 );
    }

    void beep3(){
        for( int i = 0; i < 3; i++ ){
            beep( 1'000, 50'000 );
            beep_timer.set(20'000 * rtos::us);
            wait(beep_timer);
        }
    }

public:
    SpeakerControl(hwlib::pin_out& pin_):
            pin(pin_)
    {}

    void enable_play_shoot_flag(){
        play_shoot_flag.set();
    }

    void enable_play_hit_flag(){
        play_hit_flag.set();
    }
};

#endif //SPEAKER_HPP
