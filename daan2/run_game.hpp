#ifndef RUN_GAME_HPP
#define RUN_GAME_HPP

#include "rtos.hpp"
#include "struct.hpp"
#include "display.hpp"
#include "speaker.hpp"
#include "ir_receive.hpp"

class run_game : public rtos::task<>{
private:
    enum state_t {
        idle,
        normale,
        deduct_Demage,
        sending_message_display,
        save_hit_info,
        shutting_down,
        waiting_for_startGame_signaal,
        countdown,
        setting_time,
        send_ir_message
    };
    state_t state = idle;

    DisplayControl& displayControl;
    SpeakerControl& speakerControl;
    message_writing& messageWriting;
    manage_time& manageTime;

    game_parameters_stuct gameParametersStuct;

    //flag
    rtos::flag start_run_game_flag = {this, "start_run_game_flag"};
    rtos::flag manage_time_done = {this, "manage_time_done"};
    rtos::flag trigger_pressed_flag = {this, "trigger_pressed_flag"};
    rtos::channel<game_parameters_stuct , 1024> run_game_parameters_channel = {this, "run_game_parameters_channel"};
    rtos::channel<hit , 1024> run_game_hit_channel = {this, "run_game_hit_channel"};

    void main(){
        while(true){
            switch (state) {
                case idle: {
                    //entry
                    //tansition
                    wait(start_run_game_flag);
                    state = normale;
                    break;
                }case normale: {
                    //entry
                    gameParametersStuct = run_game_parameters_channel.read();
                    displayControl.SetDisplayStats(gameParametersStuct);
                    //tansition
                    auto evt = wait(run_game_hit_channel + )
                    break;
                }case deduct_Demage: {
                    //entry
                    //tansition
                    break;
                }case sending_message_display: {
                    //entry
                    //tansition
                    break;
                }case save_hit_info: {
                    //entry
                    //tansition
                    break;
                }case shutting_down: {
                    //entry
                    //tansition
                    break;
                }case waiting_for_startGame_signaal: {
                    //entry
                    //tansition
                    break;
                }case countdown: {
                    //entry
                    //tansition
                    break;
                }case setting_time: {
                    //entry
                    //tansition
                    break;
                }case send_ir_message: {
                    //entry
                    //tansition
                    break;
                }
            }
        }
    }

public:
    run_game(DisplayControl& displayControl_, SpeakerControl& speakerControl_, message_writing& messageWriting_, manage_time& manageTime_):
            displayControl(displayControl_),
            speakerControl(speakerControl_),
            messageWriting(messageWriting_),
            manageTime(manageTime_)
    {}
    gun_data gun_data1 = {1,10,100};

    void enable_manage_time_done(){
        manage_time_done.set();
    }

    void enable_start_run_game_flag(){
        start_run_game_flag.set();
    }

    void add_game_parameters(const game_parameters_stuct& x){
        run_game_parameters_channel.write(x);
    }

    void add_hit(const hit& x){
        run_game_hit_channel.write(x);
    }
};

class manage_time : public rtos::task<>{
private:
    enum state_t {
        idle,
        counting
    };
    state_t state = idle;

    //reference
    run_game& runGame;

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

#endif //RUN_GAME_HPP
