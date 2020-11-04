//
// Created by daan on 03-11-20.
//

#ifndef TRANSFER_HPP
#define TRANSFER_HPP

#include "hwlib.hpp"
#include "struct.hpp"

/// @brief transfer_hits class
/// @details This class stores the hits and prints them
class transfer_hits{
private:
    /// @brief Player_number member
    int player_number;
    /// @brief Hit array with received hits
    hit hit_array[20];
    /// @brief Index of the array for knowing where to add to the array.
    int index = 0;

public:
    /// @brief Set_player_number function for setting the player number member
    void set_player_number(const int& number){
        player_number = number;
    }

    /// @brief Add_hit function for adding a hit to the hit_array array.
    void add_hit(const hit& last_hit){
        hit_array[index] = last_hit;
        index++;
    }

    /// @brief Print function for printing the stored hits.
    void print(){
        hwlib::wait_ms(1);
        hwlib::cout << "Player: "<< player_number << "\n";
        for ( int i = 0; i < index; ++i) {
            hwlib::cout << "-Hit by player: " << hit_array[i].from << ". With: "<< hit_array[i].damage <<" damage.\n";
        }
    }
};

#endif //TRANSFER_HPP
