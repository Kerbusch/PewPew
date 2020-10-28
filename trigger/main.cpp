#include "hwlib.hpp"
#include "rtos.hpp"

#include "speaker.hpp"
#include "ir.hpp"
#include "trigger.hpp"

//def data types
//#define tijd_countdown 2;

struct tijd_countdown{
    int tijd;
    int countdown;
};

struct gun_data{
    int number;
    int power;
    int health;
};

/*class Speaker{
private:
    hwlib::pin_direct_from_out_t pin;
public:
    Speaker(hwlib::pin_out& pin_):
            pin(pin_)
    {}

    void await( long long unsigned int t ){
        while( t > hwlib::now_us() ){}
    }

    void beep( int f, int d, int fd = 1000 ){
        auto t = hwlib::now_us();
        auto end = t + d;
        while( end > hwlib::now_us() ){
            auto p = 500'000 / f;
            f = f * fd / 1000;
            pin.write( 1 );
            pin.flush();
            await( t += p );
            pin.write( 0 );
            pin.flush();
            await( t += p );
        }
    }
};*/

class ir_recieve{
private:
    hwlib::pin_direct_from_in_t signal;

public:
    ir_recieve(hwlib::pin_in& signal_):
        signal(signal_)
    {}
};

//run_game
int main( void ){
    //make mutex
    auto m1 = rtos::mutex( "m1" );
   
    // IR output LED
    auto ir = hwlib::target::d2_36kHz();

    //ir code
    ir_send s = {ir,m1};
    message_writing m = {s};
    runner r = {m, 500 * rtos::ms};

    //speaker gpio
    auto speaker_pin = hwlib::target::pin_out( hwlib::target::pins::d7 );

    //speaker control code
    SpeakerControl speakerControl = {speaker_pin};

    //trigger gpio
    auto led_yellow = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto sw0 = hwlib::target::pin_in( hwlib::target::pins::d3 );

    //trigger control code
    Trigger trigger = {led_yellow,sw0,speakerControl};

    //OLED
    /*auto scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    auto sda = hwlib::target::pin_oc( hwlib::target::pins::sda );

    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );

    auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

    auto w = hwlib::part(oled,hwlib::xy( 0, 0 ),hwlib::xy( 128, 64));
    auto f1 = hwlib::font_default_8x8();
    auto d1 = hwlib::terminal_from( w, f1 );*/
    //END_OLED

    //START CODE
    rtos::run();
}