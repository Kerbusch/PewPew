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

/*class Loghit{
public:
    std::vector<hit> hits = {};

    void setHit(hit h){
        hits.push_back(h);
    }
};*/

class run_game : public rtos::task<>{
private:
    /*enum state_t {
        idle,
        normale,
        deduct_Demage,
        sending_message_display,
        save_hit_info,
        countdown,
        send_shoot,
        send_tijd
    };
    state_t state = idle;*/

    //reference menbers
    DisplayControl& displayControl;
    SpeakerControl& speakerControl;
    message_writing& messageWriting;
    manage_time& manageTime;
    //Loghit& loghit;

    //game parameters
    game_parameters_struct gameParametersStruct;

    //last_hit
    //hit last_hit = {31,0};

    //rtos things
    rtos::flag start_run_game_flag = {this, "start_run_game_flag"};
    rtos::flag trigger_pressed_flag = {this, "trigger_pressed_flag"};
    //rtos::channel<game_parameters_struct , 256> run_game_parameters_channel = {this, "run_game_parameters_channel"};
    //rtos::channel<hit , 256> run_game_hit_channel = {this, "run_game_hit_channel"};
    long long int delay_1sec = rtos::ms * 1000;
    rtos::timer countdown_timer = {this, "countdown_timer"};
    rtos::timer manage_time_check_timer = {this, "manage_time_check_timer"};

    /// @brief Main funtion for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        while(true){
            countdown_timer.set(rtos::ms * 100);
            wait(countdown_timer);
            hwlib::cout << "loop-run_game\n";
            /*switch (state) {
                case idle: { //wacht op start flag, op exit: gameparameters invoegen
                    hwlib::cout << "idle-run_game\n";
                    //entry
                    //transition
                    wait(start_run_game_flag);
                    gameParametersStruct = run_game_parameters_channel.read();
                    state = countdown;
                    break;
                }case normale: {
                    hwlib::cout << "normale-run_game\n";
                    //entry
                    displayControl.SetDisplayStats(gameParametersStruct);
                    //transition
                    manage_time_check_timer.set(delay_1sec);
                    auto evt = wait(run_game_hit_channel + trigger_pressed_flag + manage_time_check_timer);
                    if(evt == run_game_hit_channel){
                        state = deduct_Demage;
                    }else if(evt == trigger_pressed_flag){
                        state = send_shoot;
                    }else if(evt == manage_time_check_timer){
                        if(manageTime.check_done()){
                            state = sending_message_display;
                        }
                    }
                    break;
                }case deduct_Demage: {
                    hwlib::cout << "deduct_Demage-run_game\n";
                    //entry
                    last_hit = run_game_hit_channel.read();
                    gameParametersStruct.health -= last_hit.damage;
                    //transition
                    state = save_hit_info;
                    break;
                }case sending_message_display: {
                    hwlib::cout << "sending_message_display-run_game\n";
                    //entry
                    displayControl.SetDisplayMessageD1("");
                    displayControl.SetDisplayMessageD2("Game over");
                    displayControl.SetDisplayMessageD3("");
                    //transition
                    state = idle;
                    break;
                }case save_hit_info: {
                    hwlib::cout << "save_hit_info-run_game\n";
                    //entry
                    //loghit.setHit(last_hit);
                    //transition
                    //if(gameParametersStruct.health <= 0){
                    //    state = sending_message_display;
                    //}
                    //state = normale;
                    break;
                }case countdown: { //eerste state na idle. hierin zal run_game wachten op de countdown, display updaten en als het player 0 is met de trigger tijd_countdown schieten. aan het einde wordt state normale.
                    hwlib::cout << "countdown-run_game\n";
                    //entry
                    countdown_timer.set(delay_1sec);
                    auto evt = wait(trigger_pressed_flag + countdown_timer);
                    if(evt == trigger_pressed_flag){
                        state = send_tijd;
                    }else{ //countdown_timer
                        if(gameParametersStruct.countdown <= 0){
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
                    hwlib::cout << "send_shoot-run_game\n";
                    //entry
                    messageWriting.add_shoot(gun_data{gameParametersStruct.number,gameParametersStruct.power,gameParametersStruct.health});
                    //transition
                    state = normale;
                    break;
                }case send_tijd:{
                    hwlib::cout << "send_tijd-run_game\n";
                    //entry
                    messageWriting.add_tijd_countdown(tijd_countdown{gameParametersStruct.tijd,gameParametersStruct.countdown});
                    //transition
                    state = countdown;
                    break;
                }
            }*/
        }
    }

public:
    run_game(DisplayControl& displayControl_, SpeakerControl& speakerControl_, message_writing& messageWriting_, manage_time& manageTime_):
            rtos::task<>(300, "SpeakerControl"),
            displayControl(displayControl_),
            speakerControl(speakerControl_),
            messageWriting(messageWriting_),
            manageTime(manageTime_)
    {}
    //gun_data gun_data1 = {1,10,100};

    /*void enable_start_run_game_flag(){
        start_run_game_flag.set();
    }

    void add_game_parameters(const game_parameters_struct& x){
        run_game_parameters_channel.write(x);
    }

    void add_hit(const hit& x){
        run_game_hit_channel.write(x);
    }*/

    int get_player_number(){
        return gameParametersStruct.number;
    }
};

#endif //RUN_GAME_HPP
