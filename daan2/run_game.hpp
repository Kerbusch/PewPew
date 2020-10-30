//
// Created by daan on 29-10-20.
//

#ifndef RUN_GAME_HPP
#define RUN_GAME_HPP

#include "ir.hpp"

class game_parameters{
private:
    tijd_countdown tijdCountdown = {0 ,0};
    gun_data gunData;

public:
    void set_gun_data(const gun_data& input){
        gunData = input;
    }

    void set_tijd_countdown(const tijd_countdown& input){
        tijdCountdown = input;
    }

    game_parameters_stuct get_game_parameters(){
        return game_parameters_stuct{tijdCountdown.tijd,tijdCountdown.countdown,gunData.number,gunData.power,gunData.health};
    }

    bool check_time_input(){
        return tijdCountdown.tijd != 0;
    }
};

class run_game : public rtos::task<>{
private:
    //flag
    rtos::flag clock_done = {this, "clock_done"};
    rtos::channel<game_parameters_stuct , 1024> run_game_parameters_channel;
public:
    run_game():
        run_game_parameters_channel(this, run_game_parameters_channel)
    {}

    gun_data gun_data1 = {1,10,100};



    void enable_clock_done(){
        clock_done.set();
    }

    void add(game_parameters_stuct x){
        run_game_parameters_channel.write(x);
    }
};

class start_game : public rtos::task<>{
private:
    enum state_t {
        //states
        idle,
        check,          //check complete
        get_time,       //get time from ir translater
        get_data        //get struct from entity
    };
    state_t state = data_to_entity;

    //reference menbers
    message_translation& messageTranslation;
    game_parameters& gameParameters;
    run_game& runGame;


    //rtos menbers
    rtos::flag start_game_flag;
    rtos::channel<tijd_countdown , 1024> start_game_channel = {this, start_game_channel};

    void main(){
        switch (state) {
            case idle: {
                //entry
                //transtion
                wait(start_game_flag);
                state = data_to_entity;
                break;
            }case check: {
                //entry
                //transtion
                if(gameParameters.check_time()){
                    state = get_data;
                }else{
                    state = get_time;
                }
                break;
            }case get_time: {
                //entry
                //transtion
                wait(start_game_channel);
                gameParameters.set_tijd_countdown(start_game_channel.read());
                state = get_data;
                break;
            }case get_data: {
                //entry
                runGame.add(gameParameters.get_game_parameters())
                //transtion
                state = idle
                break;
            }
        }
    }

public:
    start_game(message_translation& messageTranslation_, game_parameters& gameParameters_, run_game& runGame):
        messageTranslation(messageTranslation_),
        gameParameters(gameParameters_),
        runGame(runGame_),
        start_game_flag(this, "start_game_flag")
    {}

    void enable_start_game_flag(){
        start_game_flag.set();
    }

    void add(){

    }

};



#endif //RUN_GAME_HPP
