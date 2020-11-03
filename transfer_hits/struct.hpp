//
// Created by daan on 28-10-20.
//

#ifndef TRIGGER_STRUCT_HPP
#define TRIGGER_STRUCT_HPP

/// @brief tijd_countdown struct
/// @details Struct that contains the time and countdown of the game.
/// @see game_parameters_struct
struct tijd_countdown{
    /// @brief Game time
    int tijd;
    /// @brief Countdown till start game
    int countdown = 10;
};

/// @brief gun_data struct
/// @details Struct that contains the player number, power and health
/// @see game_parameters_struct
struct gun_data{
    /// @brief Player number
    int number;
    /// @brief Gun power / damage
    int power;
    /// @brief Player health
    int health = 100;
};

/// @brief hit struct
/// @details Struct that contains the from and damage of the hit
struct hit{
    /// @brief Number of the player who shot
    int from;
    /// @brief Damage of the hit
    int damage;
};

/// @brief game_parameters_struct struct
/// @see gun_data tijd_countdown
struct game_parameters_struct{
    /// @brief Game time
    int tijd;
    /// @brief Countdown till start game
    int countdown = 60;
    /// @brief Player number
    int number;
    /// @brief Gun power / damage
    int power;
    /// @brief Player health
    int health = 100;
};

#endif //TRIGGER_STRUCT_HPP
