#ifndef KEYPAD_HPP
#define KEYPAD_HPP

#include "hwlib.hpp"


/// @brief keypad_listener class
/// @details This class adds functions to the hwlib::keypad class.
class keypad_listener{
private:
    /// @brief Defines keypad with 16 keys.
    hwlib::keypad<16>& keypad1;

public:
    /// @brief Constructor of the keypad class.
    /// @details This class provides a function which returns c (pressed key on keyboard)
    /// @param keypad1_ This is a hwlib::keypad<16> class
    keypad_listener(hwlib::keypad<16>& keypad1_):
            keypad1(keypad1_)
    {}

    /// @brief check_input funtion
    /// @details this funtion gets the input from the keypad and checks the output with the input char.
    bool check_input(const char& check){
        auto c = keypad1.getc();
        if(c == check){
            return true;
        }else{
            return false;
        }
    }

    /// @brief getc funtion
    /// @details this funtion gets the input from the keypad.
    char getc(){
        return keypad1.getc();
    }
};

#endif //KEYPAD_HPP
