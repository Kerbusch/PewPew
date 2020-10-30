//
// Created by daan on 28-10-20.
//

#ifndef TRIGGER_KEYPAD_HPP
#define TRIGGER_KEYPAD_HPP

#include "hwlib.hpp"

#define GAMEMODE_NORMAAL 1;

struct tijd_countdown{
    int tijd;
    int countdown;
};

struct gun_data{
    int number;
    int power;
    int health = 100;
};

class KeypadController{
private:
    hwlib::keypad<16>& keypad1;

public:
    KeypadController(hwlib::keypad<16>& keypad1_):
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

class DisplayControl{
private:
    //oled
    hwlib::glcd_oled& oled;

    //window
    hwlib::window& w1;

    //font
    hwlib::font_default_8x8 f1;


    //terminal
    hwlib::terminal_from d1;

public:
    DisplayControl(hwlib::glcd_oled& oled_, hwlib::window& w1_):
            oled(oled_),
            w1(w1_),
            d1(w1,f1)
    {}

    void run(){
        d1 << "\f"
           << "   Gun number:";
        oled.flush();
    }
};

class init_game{
private:
    KeypadController& keypadController;
    //display

    gun_data gun = {-1,-1,-1};
    tijd_countdown tijd_data = {-1,-1};
    int gamemode = 0;

    int get_int(){
        int n = 0;
        bool loop = true;
        while(loop){
            auto c = keypadController.getc();
            if(c != 'A' && c != 'B' && c != 'C' && c != 'D'){
                if(c == '*'){
                    n = 0;
                    hwlib::cout << "input: " << "\n";
                }else if(c == '#'){
                    loop = false;
                }else{
                    n *= 10;
                    n += c - '0';
                    hwlib::cout << "input: " << n << "\n";
                }
            }
        }
        return n;
    }

    void get_gun_number(){
        while (true){
            hwlib::cout << "\nGun number(0-31): \n";
            gun.number = get_int();
            if(gun.number >= 0 && gun.number <= 31){
                break;
            }
            hwlib::cout << "Wrong input \n\n";
        }
    }

    void get_tijd_countdown(){
        while (true){
            hwlib::cout << "\nGame duration(sec, 0-1023): \n";
            tijd_data.tijd = get_int();
            if(tijd_data.tijd >= 0 && tijd_data.tijd <= 1023){
                break;
            }
            hwlib::cout << "Wrong input \n\n";
        }

        while (true){
            hwlib::cout << "\nCountdown(sec, 0-1023): \n";
            tijd_data.countdown = get_int();
            if(tijd_data.countdown >= 0 && tijd_data.countdown <= 1023){
                break;
            }
            hwlib::cout << "Wrong input \n\n";
        }
    }

    void get_gun_power(){
        while (true){
            hwlib::cout << "\nGun power: \n";
            gun.power = get_int();
            if(gun.power >= 0 && gun.power <= 1023){
                break;
            }
            hwlib::cout << "Wrong input \n\n";
        }
    }

    void get_health(){
        while (true){
            hwlib::cout << "\nHealth: \n";
            gun.health = get_int();
            if(gun.health >= 0 && gun.health <= 1023){
                break;
            }
            hwlib::cout << "Wrong input \n\n";
        }
    }

    void get_gamemode(){
        bool loop = true;
        hwlib::cout << "\nGamemode:\n"
                    << "A: Normal \n"
                    << "B: ...\n"
                    << "C: ...\n"
                    << "D: ...\n";
        while(loop){
            char c = keypadController.getc();
            switch (c) {
                case 'A': {
                    hwlib::cout << "Normal\n";
                    gamemode = GAMEMODE_NORMAAL;
                    loop = false;
                    break;
                }case 'B': {
                    hwlib::cout << "Nothing\n";
                    loop = false;
                    break;
                }case 'C': {
                    hwlib::cout << "Nothing\n";
                    loop = false;
                    break;
                }case 'D': {
                    hwlib::cout << "Nothing\n";
                    loop = false;
                    break;
                }
            }
        }
    }

public:
    init_game(KeypadController& keypadController_):
            keypadController(keypadController_)
    {}

    void setup(){
        get_gun_number();
        if(gun.number == 0){
            get_gamemode();
            get_tijd_countdown();
        }
        get_gun_power();
        get_health();

        hwlib::cout << "gun_number: " << gun.number << "\n";
        hwlib::cout << "tijd: " << tijd_data.tijd << "\n";
        hwlib::cout << "countdown: " << tijd_data.countdown << "\n";
        hwlib::cout << "gun_power: " << gun.power << "\n";
        hwlib::cout << "health: " << gun.health << "\n";
        hwlib::cout << "gamemode " << gamemode << "\n";
    }
};

int main( void ){
    // wait for the PC console to start
    hwlib::wait_ms( 500 );
    hwlib::cout << "init_game demo\n";

    /*//OLED
    auto scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    auto sda = hwlib::target::pin_oc( hwlib::target::pins::sda );

    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );

    auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

    auto w = hwlib::part(oled,hwlib::xy( 0, 0 ),hwlib::xy( 128, 64));

    DisplayControl displayControl = {oled,w};
    displayControl.run();

    //auto w = hwlib::part(oled,hwlib::xy( 0, 0 ),hwlib::xy( 128, 64));
    //auto f1 = hwlib::font_default_8x8();
    //auto d1 = hwlib::terminal_from( w, f1 );
    //END_OLED*/

    //aanmaken pinnen keypad
    auto out0 = hwlib::target::pin_oc( hwlib::target::pins::a0 );
    auto out1 = hwlib::target::pin_oc( hwlib::target::pins::a1 );
    auto out2 = hwlib::target::pin_oc( hwlib::target::pins::a2 );
    auto out3 = hwlib::target::pin_oc( hwlib::target::pins::a3 );

    auto in0  = hwlib::target::pin_in( hwlib::target::pins::a4 );
    auto in1  = hwlib::target::pin_in( hwlib::target::pins::a5 );
    auto in2  = hwlib::target::pin_in( hwlib::target::pins::a6 );
    auto in3  = hwlib::target::pin_in( hwlib::target::pins::a7 );

    auto out_port = hwlib::port_oc_from( out0, out1, out2, out3 );
    auto in_port  = hwlib::port_in_from( in0,  in1,  in2,  in3  );
    auto matrix   = hwlib::matrix_of_switches( out_port, in_port );
    auto keypad1   = hwlib::keypad< 16 >( matrix, "123A456B789C*0#D" );
    //einde aanmaken pinnen keypad

    KeypadController keypadController = {keypad1};

    init_game game = {keypadController};

    game.setup();
}

#endif //TRIGGER_KEYPAD_HPP
