//
// Created by daan on 28-10-20.
//

#ifndef TRIGGER_STRUCT_HPP
#define TRIGGER_STRUCT_HPP

//moet in eigen hpp
struct tijd_countdown{
    int tijd;
    int countdown = 60;
};

struct gun_data{
    int number;
    int power;
    int health = 100;
};

struct hit{
    int from;
    int demage;
};

struct game_parameters_stuct{
    //tijd_countdown
    int tijd;
    int countdown = 60;
    //gun_data
    int number;
    int power;
    int health = 100;
};

#endif //TRIGGER_STRUCT_HPP
