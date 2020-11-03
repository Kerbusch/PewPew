//
// Created by daan on 03-11-20.
//

#ifndef PEWPEW_TRANSFER_HPP
#define PEWPEW_TRANSFER_HPP

#include "hwlib.hpp"
#include "struct.hpp"

class transfer_hits{
private:
    int player_number;
    hit hit_array[20];
    int index = 0;

public:
    void set_player_number(const int& number){
        player_number = number;
    }

    void add_hit(const hit& last_hit){
        hit_array[index] = last_hit;
        index++;
    }

    void print(){
        hwlib::wait_ms(1);
        hwlib::cout << "Player: "<< player_number << "\n";
        for ( int i = 0; i < index; ++i) {
            hwlib::cout << "-Hit by player: " << hit_array[i].from << ". With: "<< hit_array[i].damage <<" damage.\n";
        }
    }
};

#endif //PEWPEW_TRANSFER_HPP
