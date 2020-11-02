#ifndef KEYPAD_HPP
#define KEYPAD_HPP

#include "hwlib.hpp"

class KeypadControl{
private:
    hwlib::keypad<16>& keypad1;

public:
    KeypadControl(hwlib::keypad<16>& keypad1_):
            keypad1(keypad1_)
    {}

    bool check_input(const char& check){
        auto c = keypad1.getc();
        if(c == check){
            return true;
        }else{
            return false;
        }
    }

    char getc(){
        return keypad1.getc();
    }
};

#endif //KEYPAD_HPP
