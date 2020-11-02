#ifndef MANAGE_TIME_HPP
#define MANAGE_TIME_HPP

#include "rtos.hpp"
#include "run_game.hpp"

class manage_time : public rtos::task<>{
private:
    enum state_t {
        idle,
        counting
    };
    //reference
    run_game& runGame;

    state_t state = idle;


    int timer = 0;

    //rtos members
    rtos::flag start_manage_time_flag = {this, "start_manage_time_flag"};
    long long int delay = 1000 * rtos::ms;
    rtos::timer manage_time_timer = {this, "manage_time_timer"};

    void main(){
        switch (state) {
            case idle: {
                //entry
                //transistion
                wait(start_manage_time_flag);
                state = counting;
                break;
            }case counting: {
                //entry
                count();
                runGame.enable_manage_time_done();
                //transistion
                state = idle;
                break;
            }
        }
    }

    void count(){
        while (timer > 0){
            manage_time_timer.set(delay);
            wait(manage_time_timer);
            timer--;
        }
    }

public:
    manage_time(run_game& runGame_):
            runGame(runGame_)
    {}

    void set_timer(const int& x){
        timer = x;
    }

    int get_time(){
        return timer;
    }

    void enable_start_manage_time_flag(){
        start_manage_time_flag.set();
    }
};

#endif //MANAGE_TIME_HPP
