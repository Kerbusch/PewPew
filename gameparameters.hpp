#ifndef GAMEPARAMETERS_HPP
#define GAMEPARAMETERS_HPP

#include "struct.hpp"

/// @brief game_parameters class
/// @details This class functions as the entity of game_parameters
class game_parameters{
private:
    /// @brief tijd_countdown member
    tijd_countdown tijdCountdown = {0 ,0};
    /// @brief gun_data member
    gun_data gunData;

public:
    /// @brief Set_gun_data function
    /// @details This function sets the internal gunData member to the value of the input
    /// @param input A gun_data struct
    void set_gun_data(const gun_data& input){
        hwlib::cout << "game_parameters_number-" << input.number << "\n";
        hwlib::cout << "game_parameters_power-" << input.power << "\n";
        hwlib::cout << "game_parameters_health-" << input.health << "\n";
        gunData = input;
    }

    /// @brief Set_tijd_countdown function
    /// @details This function sets the internal tijdCountdown member to the value of the input
    /// @param input A tijd_countdown struct
    void set_tijd_countdown(const tijd_countdown& input){
        hwlib::cout << "game_parameters_tijd-" << input.tijd << "\n";
        hwlib::cout << "game_parameters_countdown-" << input.countdown << "\n";
        tijdCountdown = input;
    }

    /// @brief Get_game_parameters function
    /// @details This function returns a game_parameters_struct struct of the member variables.
    game_parameters_struct get_game_parameters(){
        return game_parameters_struct{tijdCountdown.tijd,tijdCountdown.countdown,gunData.number,gunData.power,gunData.health};
    }

    /// @brief Check_time_input function
    /// @details This funtion returns a bool. It checks if the players has entered the time.
    bool check_time_input(){
        return tijdCountdown.tijd != 0;
    }
};

#endif //GAMEPARAMETERS_HPP
