#ifndef RUN_GAME_HPP
#define RUN_GAME_HPP

#include "rtos.hpp"
#include "struct.hpp"
#include "display.hpp"
#include "speaker.hpp"
#include "ir_receive.hpp"
#include "ir_send.hpp"
#include "manage_time.hpp"
#include "transfer.hpp"

/// @brief Run_game class
/// @details Main class of the application, this class runs the whole game which combines most of the functions of all classes.
class run_game : public rtos::task<>{
private:
    /// @brief Enum of the different states
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
    /// @brief Selects the starting state
    state_t state = idle;

    //reference menbers
    /// @brief DisplayControl member as reference
    DisplayControl& displayControl;
    /// @brief SpeakerControl member as reference
    SpeakerControl& speakerControl;
    /// @brief message_writing member as reference
    message_writing& messageWriting;
    /// @brief manage_time member as reference
    manage_time& manageTime;
    /// @brief transfer_hits member as reference
    transfer_hits& transferHits;

    //game parameters
    /// @brief game_parameters_struct member
    game_parameters_struct gameParametersStruct;

    //last_hit
    /// @brief last_hit member for saving the last hit
    hit last_hit = {31,0};

    //rtos things
    /// @brief start_run_game_flag flag for starting the run_game class.
    rtos::flag start_run_game_flag = {this, "start_run_game_flag"};
    /// @brief trigger_pressed_flag flag for detecting the trigger.
    rtos::flag trigger_pressed_flag = {this, "trigger_pressed_flag"};
    /// @brief run_game_parameters_channel channel for receiving the game_parameters_struct from start_game.
    rtos::channel<game_parameters_struct , 32> run_game_parameters_channel = {this, "run_game_parameters_channel"};
    /// @brief channel for receiving the hit from message_translation.
    rtos::channel<hit , 32> run_game_hit_channel = {this, "run_game_hit_channel"};
    /// @brief run_game_hit_flag flag for knowing when there's something in the run_game_hit_channel channel
    rtos::flag run_game_hit_flag = {this, "run_game_hit_flag"};

    /// @brief countdown_timer timer.
    long long int delay_1sec = rtos::ms * 1000;
    /// @brief countdown_timer timer.
    rtos::timer countdown_timer = {this, "countdown_timer"};
    /// @brief manage_time_check_timer timer.
    rtos::timer manage_time_check_timer = {this, "manage_time_check_timer"};

    /// @brief Main function for the rtos::task<>.
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
                        messageWriting.set_player(gameParametersStruct.number);

                        hwlib::cout << "player: " << gameParametersStruct.number << "\n";
                    }
                    manage_time_check_timer.set(rtos::ms * 1000);
                    auto evt = wait(manage_time_check_timer + run_game_hit_flag + trigger_pressed_flag);
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
                        //hwlib::cout << "trigger_pressed_flag-run_game\n";
                        state = send_shoot;
                    }
                    break;
                }case deduct_Demage: {

                    //entry
                    last_hit = run_game_hit_channel.read();
                    hwlib::cout << last_hit.damage << ", " << last_hit.from << "\n";
                    gameParametersStruct.health -= last_hit.damage;
                    hwlib::cout << gameParametersStruct.health;
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
                    messageWriting.add_shoot(gun_data{gameParametersStruct.number,gameParametersStruct.power,gameParametersStruct.health});
                    messageWriting.enable_send_shoot_flag();
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
    /// @brief constructor for the run_game class
    /// @details This constructor initializes the reference members.
    /// @param displayControl_ , a DisplayControl class
    /// @param speakerControl_ , a speakerControl class
    /// @param messageWriting_ , a messageWriting class
    /// @param manageTime_ , a manageTime class
    /// @param transferHits_ , a transferHits class
    run_game(DisplayControl& displayControl_, SpeakerControl& speakerControl_, message_writing& messageWriting_, manage_time& manageTime_, transfer_hits& transferHits_):
            task(400, "SpeakerControl"),
            displayControl(displayControl_),
            speakerControl(speakerControl_),
            messageWriting(messageWriting_),
            manageTime(manageTime_),
            transferHits(transferHits_)
    {}
    //gun_data gun_data1 = {1,10,100};

    /// @brief Enable function for the enable_start_run_game_flag flag
    void enable_start_run_game_flag(){
        start_run_game_flag.set();
    }

    /// @brief Enable function for the enable_trigger_pressed_flag flag
    void enable_trigger_pressed_flag(){
        trigger_pressed_flag.set();
    }

    /// @brief Enable function for the enable_run_game_hit_flag flag
    void enable_run_game_hit_flag(){
        run_game_hit_flag.set();
    }

    /// @brief Add input to enable_run_game_hit_flag channel.
    void add_game_parameters(const game_parameters_struct& x){
        enable_run_game_hit_flag.write(x);
    }

    /// @brief Add input to run_game_hit_channel channel.
    void add_hit(const hit& x){
        run_game_hit_channel.write(x);
    }

    /// @brief Get player number from gameParametersStruct.
    int get_player_number(){
        return gameParametersStruct.number;
    }
};

#endif //RUN_GAME_HPP
