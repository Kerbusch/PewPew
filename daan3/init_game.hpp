#ifndef INIT_GAME_HPP
#define INIT_GAME_HPP

#include "rtos.hpp"
#include "struct.hpp"

#include "keypad.hpp"
#include "display.hpp"
#include "gameparameters.hpp"
#include "start_game.hpp"

class init_game : public rtos::task<>{
private:
    enum state_t{
        idle,
        normale,
        getting_time,
        getting_gunpower,
        getting_player,
        startgame
    };
    state_t state = normale;

    KeypadControl& keypadControl;
    DisplayControl& displayControl;
    game_parameters& gameParameters;
    start_game& startGame;

    gun_data gun = {0,10}; //default
    tijd_countdown tijd_data = {12}; //default

    rtos::flag restart_init_game_flag = {this, "restart_init_game_flag"};
    long long int delay = {rtos::ms * 200};
    rtos::timer wrong_timer = {this, "wrong_timer"};

    void main(){
        while(true) {
            switch (state) {
                case idle: {
                    //entry
                    //transition
                    wait(restart_init_game_flag);
                    state = normale;
                    break;
                }
                case normale: {
                    //entry
                    print_ui();
                    //transition
                    bool loop = true;
                    while (loop) {
                        char c = keypadControl.getc();
                        if (c == 'A') {
                            state = getting_player;
                            loop = false;
                        } else if (c == 'B') {
                            state = getting_gunpower;
                            loop = false;
                        } else if (c == 'C') {
                            state = getting_time;
                            loop = false;
                        } else if (c == '#') {
                            state = startgame;
                            loop = false;
                        }
                    }
                    break;
                }
                case getting_time: {
                    //entry
                    input_tijd();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case getting_gunpower: {
                    //entry
                    input_gunpower();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case getting_player: {
                    //entry
                    input_number();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case startgame: {
                    hwlib::cout << "startgame\n";
                    //entry
                    if(gun.number == 0){
                        gameParameters.set_tijd_countdown(tijd_data);
                    }
                    gameParameters.set_gun_data(gun);
                    startGame.enable_start_game_flag();
                    clear_ui();
                    state = idle;
                    //transition
                    break;
                }
            }
        }
    }

    void print_ui(){
        displayControl.SetDisplayMessageD1("Settings:");
        displayControl.SetDisplayMessageD2("A: player number\nB: gun power");
        displayControl.SetDisplayMessageD3("C: Tijd\n#: Done");
    }

    void clear_ui(){
        displayControl.SetDisplayMessageD1("");
        displayControl.SetDisplayMessageD2("");
        displayControl.SetDisplayMessageD3("");
    }

    int get_int(){
        displayControl.SetDisplayMessageD3("Input:\n");
        int n = 0;
        bool loop = true;
        while(loop){
            auto c = keypadControl.getc();
            if(c != 'A' && c != 'B' && c != 'C' && c != 'D'){
                if(c == '*'){
                    n = 0;
                    displayControl.SetDisplayMessageD3("Input:\n");
                }else if(c == '#'){
                    loop = false;
                    displayControl.SetDisplayMessageD3("");
                }else{
                    n *= 10;
                    n += c - '0';
                    displayControl.SetDisplayInput("Input: ",n,"\n");
                }
            }
        }
        return n;
    }

    void input_number(){
        while (true){
            displayControl.SetDisplayMessageD2("Gun number:\n(0-31)");
            gun.number = get_int();
            if(gun.number >= 0 && gun.number <= 31){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

    void input_tijd(){
        while (true){
            displayControl.SetDisplayMessageD2("Game duration\n(sec, 0-1023)");
            tijd_data.tijd = get_int();
            if(tijd_data.tijd >= 0 && tijd_data.tijd <= 1023){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

    void input_gunpower(){
        while (true){
            displayControl.SetDisplayMessageD2("Gun power:\n(5-15)");
            gun.power = get_int();
            if(gun.power >= 5 && gun.power <= 15){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

public:
    init_game(KeypadControl& keypadControl_, DisplayControl& displayControl_, game_parameters& gameParameters_, start_game& startGame_):
            keypadControl(keypadControl_),
            displayControl(displayControl_),
            gameParameters(gameParameters_),
            startGame(startGame_)
    {}

    /*void setup(){
        get_gun_number();
        if(gun.number == 0){
            get_gamemode();
            input_tijd_countdown();
        }
        get_gun_power();
        get_health();

        hwlib::cout << "gun_number: " << gun.number << "\n";
        hwlib::cout << "tijd: " << tijd_data.tijd << "\n";
        hwlib::cout << "countdown: " << tijd_data.countdown << "\n";
        hwlib::cout << "gun_power: " << gun.power << "\n";
        hwlib::cout << "health: " << gun.health << "\n";
        hwlib::cout << "gamemode " << gamemode << "\n";
    }*/

    void enable_restart_init_game_flag(){
        restart_init_game_flag.set();
    }
};

#endif //INIT_GAME_HPP
