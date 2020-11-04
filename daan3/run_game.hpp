#ifndef RUN_GAME_HPP
#define RUN_GAME_HPP

#include <vector>

#include "rtos.hpp"
#include "struct.hpp"
#include "display.hpp"
#include "speaker.hpp"
#include "ir_receive.hpp"
#include "ir_send.hpp"
#include "manage_time.hpp"
#include "transfer.hpp"

class run_game : public rtos::task<>{
private:
    enum state_t {
        idle,
        normale,
        deduct_Demage,
        sending_message_display,
        save_hit_info,
        countdown,
        send_shoot,
        send_tijd
    };
    state_t state = idle;

    //reference menbers
    DisplayControl& displayControl;
    SpeakerControl& speakerControl;
    message_writing& messageWriting;
    manage_time& manageTime;
    transfer_hits& transferHits;

    //game parameters
    game_parameters_struct gameParametersStruct;

    //last_hit
    hit last_hit = {31,0};

    //rtos things
    rtos::flag start_run_game_flag = {this, "start_run_game_flag"};
    rtos::flag trigger_pressed_flag = {this, "trigger_pressed_flag"};
    rtos::channel<game_parameters_struct , 64> run_game_parameters_channel = {this, "run_game_parameters_channel"};
    rtos::channel<hit , 64> run_game_hit_channel = {this, "run_game_hit_channel"};
    rtos::flag run_game_hit_flag = {this, "run_game_hit_flag"};

    long long int delay_1sec = rtos::ms * 1000;
    rtos::timer countdown_timer = {this, "countdown_timer"};
    rtos::timer manage_time_check_timer = {this, "manage_time_check_timer"};

    /// @brief Main funtion for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        while(true){
            switch (state) {
                case idle: { //wacht op start flag, op exit: gameparameters invoegen
                    //entry
                    //transition
                    wait(start_run_game_flag);
                    gameParametersStruct = run_game_parameters_channel.read();
                    messageWriting.set_player(gameParametersStruct.number);
                    state = countdown;
                    break;
                }case normale: {
                    //entry
                    //displayControl.SetDisplayStats(gameParametersStruct);
                    //displayControl.SetDisplayTime(manageTime.timer);
                    //transition
                    if(gameParametersStruct.number == 0){
                        gameParametersStruct.number = 1;
                    }
                    manage_time_check_timer.set(rtos::ms * 1000);
                    auto evt = wait(manage_time_check_timer/* + run_game_hit_flag*/ + trigger_pressed_flag);
                    if(evt == manage_time_check_timer){
                        if(manageTime.check_done()){
                            state = sending_message_display;
                        }else{
                            displayControl.SetDisplayTime(manageTime.timer);
                        }
                    }else if(evt == run_game_hit_flag) {
                        hwlib::cout << "run_game_hit_flag-run_game\n";
                        state = deduct_Demage;
                    }else if(evt == trigger_pressed_flag){
                        hwlib::cout << "trigger_pressed_flag-run_game\n";
                        state = send_shoot;
                    }
                    break;
                }case deduct_Demage: {
                    //entry
                    last_hit = run_game_hit_channel.read();
                    gameParametersStruct.health -= last_hit.damage;
                    //transition
                    state = save_hit_info;
                    break;
                }case sending_message_display: {
                    //entry
                    displayControl.SetDisplayMessageD1("");
                    displayControl.SetDisplayMessageD2("Game over");
                    displayControl.SetDisplayMessageD3("");
                    //transition

                    state = idle;
                    break;
                }case save_hit_info: {
                    //entry
                    transferHits.add_hit(last_hit);
                    //transition
                    if(gameParametersStruct.health <= 0){
                        manageTime.enable_stop_manage_time_flag();
                        state = sending_message_display;
                    }
                    state = normale;
                    break;
                }case countdown: { //eerste state na idle. hierin zal run_game wachten op de countdown, display updaten en als het player 0 is met de trigger tijd_countdown schieten. aan het einde wordt state normale.
                    //entry
                    countdown_timer.set(delay_1sec);
                    auto evt = wait(trigger_pressed_flag + countdown_timer);
                    if(evt == trigger_pressed_flag){
                        state = send_tijd;
                    }else{ //countdown_timer
                        if(gameParametersStruct.countdown <= 0){
                            displayControl.SetDisplayMessageD1("");
                            manageTime.set_timer(gameParametersStruct.tijd); //start timer
                            manageTime.enable_start_manage_time_flag();
                            state = normale;
                        }else{
                            displayControl.SetDisplayCountdown(gameParametersStruct.countdown);
                            gameParametersStruct.countdown--;
                        }
                    }
                    //transition
                    break;
                }case send_shoot: {
                    //entry
                    //messageWriting.add_shoot(gun_data{gameParametersStruct.number,gameParametersStruct.power,gameParametersStruct.health});
                    //messageWriting.enable_send_shoot_flag();
                    messageWriting.add_tijd_countdown(tijd_countdown{gameParametersStruct.tijd,gameParametersStruct.countdown});
                    messageWriting.enable_send_tijd_countdown_flag();
                    //transition
                    state = normale;
                    break;
                }case send_tijd:{
                    //entry
                    messageWriting.add_tijd_countdown(tijd_countdown{gameParametersStruct.tijd,gameParametersStruct.countdown});
                    messageWriting.enable_send_tijd_countdown_flag();
                    //transition
                    state = countdown;
                    break;
                }
            }
        }
    }

public:
    run_game(DisplayControl& displayControl_, SpeakerControl& speakerControl_, message_writing& messageWriting_, manage_time& manageTime_, transfer_hits& transferHits_):
            task(400, "SpeakerControl"),
            displayControl(displayControl_),
            speakerControl(speakerControl_),
            messageWriting(messageWriting_),
            manageTime(manageTime_),
            transferHits(transferHits_)
    {}
    //gun_data gun_data1 = {1,10,100};

    void enable_start_run_game_flag(){
        start_run_game_flag.set();
    }

    void enable_trigger_pressed_flag(){
        trigger_pressed_flag.set();
    }

    void enable_run_game_hit_flag(){
        run_game_hit_flag.set();
    }

    void add_game_parameters(const game_parameters_struct& x){
        run_game_parameters_channel.write(x);
    }

    void add_hit(const hit& x){
        run_game_hit_channel.write(x);
    }

    int get_player_number(){
        return gameParametersStruct.number;
    }
};

#endif //RUN_GAME_HPP
