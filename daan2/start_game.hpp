#ifndef START_GAME_HPP
#define START_GAME_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

#include "gameparameters.hpp"
#include "run_game.hpp"
#include "display.hpp"

class start_game : public rtos::task<>{
private:
    enum state_t {
        idle,
        check,          //check complete
        get_time,       //get time from ir translator
        get_data        //get struct from entity
    };
    state_t state = idle;

    //reference members
    game_parameters& gameParameters;
    run_game& runGame;
    DisplayControl& displayControl;

    //rtos members
    rtos::flag start_game_flag = {this, "start_game_flag"};
    rtos::channel<tijd_countdown , 1024> start_game_channel = {this, "start_game_channel"};

    void main(){
        while(true) {
            switch (state) {
                case idle: {
                    hwlib::cout << "start_game idle\n";
                    //entry
                    //transtion
                    auto evt = wait(start_game_flag);
                    if (evt == start_game_flag) {
                        state = check;
                    }
                    break;
                }
                case check: {
                    hwlib::cout << "start_game check\n";
                    //entry
                    //transtion
                    if (gameParameters.check_time_input()) {
                        state = get_data;
                    } else {
                        state = get_time;
                    }
                    break;
                }
                case get_time: {
                    hwlib::cout << "start_game get_time\n";
                    //entry
                    //transtion
                    wait(start_game_channel);
                    gameParameters.set_tijd_countdown(start_game_channel.read());
                    state = get_data;
                    break;
                }
                case get_data: {
                    hwlib::cout << "start_game get_data\n";
                    //entry
                    displayControl.SetDisplayMessageFadeD2("Settings received");
                    runGame.add_game_parameters(gameParameters.get_game_parameters());
                    //transtion
                    state = idle;
                    break;
                }
            }
        }
    }

public:
    start_game(game_parameters& gameParameters_, run_game& runGame_, DisplayControl& displayControl_):
            gameParameters(gameParameters_),
            runGame(runGame_),
            displayControl(displayControl_)
    {}

    void enable_start_game_flag(){
        start_game_flag.set();
    }

    void add(tijd_countdown x){
        start_game_channel.write(x);
    }
};

#endif //START_GAME_HPP
