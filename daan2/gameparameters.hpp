#ifndef GAMEPARAMETERS_HPP
#define GAMEPARAMETERS_HPP

#include "struct.hpp"

class game_parameters{
private:
    tijd_countdown tijdCountdown = {0 ,0};
    gun_data gunData;

public:
    void set_gun_data(const gun_data& input){
        hwlib::cout << "game_parameters_number-" << input.number << "\n";
        hwlib::cout << "game_parameters_power-" << input.power << "\n";
        hwlib::cout << "game_parameters_health-" << input.health << "\n";
        gunData = input;
    }

    void set_tijd_countdown(const tijd_countdown& input){
        hwlib::cout << "game_parameters_tijd-" << input.tijd << "\n";
        hwlib::cout << "game_parameters_countdown-" << input.countdown << "\n";
        tijdCountdown = input;
    }

    game_parameters_stuct get_game_parameters(){
        return game_parameters_stuct{tijdCountdown.tijd,tijdCountdown.countdown,gunData.number,gunData.power,gunData.health};
    }

    bool check_time_input(){
        return tijdCountdown.tijd != 0;
    }
};

#endif //GAMEPARAMETERS_HPP
