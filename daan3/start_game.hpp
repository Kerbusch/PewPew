#ifndef START_GAME_HPP
#define START_GAME_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

#include "gameparameters.hpp"
#include "run_game.hpp"
#include "display.hpp"

/// @brief start_game class
/// @details Class that sits between the init_game and run_game classes. Start_game puts the number,power,health, and the time and countdown if it's player 0, in a game_parameters class
class start_game : public rtos::task<>{
private:
    /// @brief Enum of the different states
    enum state_t {
        idle,
        check,          //check complete
        get_time,       //get time from ir translator
        get_data        //get struct from entity
    };
    /// @brief Selects the starting state
    state_t state = idle;

    //reference members
    /// @b game_parameters member as reference
    game_parameters& gameParameters;
    /// @b run_game member as reference
    run_game& runGame;
    /// @b DisplayControl member as reference
    DisplayControl& displayControl;

    //rtos members
    /// @brief start_game_flag flag for starting the start_game class.
    rtos::flag start_game_flag = {this, "start_game_flag"};
    /// @brief start_game_channel channel for receiving the time and countdown from the ir receiver.
    //rtos::channel<tijd_countdown , 1024> start_game_channel = {this, "start_game_channel"};

    /// @brief Main funtion for the rtos::task<>.
    /// @details State switcher for the different states
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
                    //wait(start_game_channel);
                    //gameParameters.set_tijd_countdown(start_game_channel.read());
                    state = get_data;
                    break;
                }
                case get_data: {
                    hwlib::cout << "start_game get_data\n";
                    //entry
                    displayControl.SetDisplayMessageFadeD2("Settings received");
                    //runGame.add_game_parameters(gameParameters.get_game_parameters());
                    //transtion
                    state = idle;
                    break;
                }
            }
        }
    }

public:
    /// @b constructor for the start_game class
    /// @details This constructor initializes the reference members.
    /// @param gameParameters_ , a game_parameters class
    /// @param runGame_ , a run_game class
    /// @param displayControl_ , a DisplayControl class
    start_game(game_parameters& gameParameters_, run_game& runGame_, DisplayControl& displayControl_):
            gameParameters(gameParameters_),
            runGame(runGame_),
            displayControl(displayControl_)
    {}

    /// @brief enable funtion for the start_game_flag flag
    void enable_start_game_flag(){
        start_game_flag.set();
    }

    /// @brief add funtion for the start_game_channel channel
    /*void add(tijd_countdown x){
        start_game_channel.write(x);
    }*/
};

#endif //START_GAME_HPP
