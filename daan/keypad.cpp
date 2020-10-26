#include "hwlib.hpp"

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

    int get_int(){
        int n = 0;
        bool loop = true;
        while(loop){
            auto c = keypad1.getc();
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
};

int main( void ){
    // wait for the PC console to start
    hwlib::wait_ms( 500 );
    hwlib::cout << "Keypad demo\n";
   
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

    KeypadController pad = {keypad1};

    int gun_number = 0;
    int tijd = 0, countdown = 0;

    hwlib::cout << "Give gun number: \n";
    gun_number = pad.get_int();
    if(gun_number == 0){
        hwlib::cout << "Give tijd: \n";
        tijd = pad.get_int();
        hwlib::cout << "Give countdown: \n";
        countdown = pad.get_int();
    }
    hwlib::cout << "gun_number" << gun_number << "\n";
    hwlib::cout << "tijd" << tijd << "\n";
    hwlib::cout << "countdown" << countdown << "\n";
}
