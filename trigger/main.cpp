#include "hwlib.hpp"
#include "rtos.hpp"

#include "speaker.hpp"
#include "run_game.hpp"
#include "ir.hpp"
#include "trigger.hpp"
#include "struct.hpp"

class clock : public rtos::task<>{
private:
    enum state_t {
        idle,
        counting
    };
    state_t state = idle;

    //reference
    run_game& runGame;

    int timer = 0;

    //rtos members
    rtos::flag start_clock_flag = {this, "start_clock_flag"};
    long long int delay = 1000 * rtos::ms;
    rtos::timer clock_timer = {this, "clock_timer"};

    void main(){
        switch (state) {
            case idle: {
                //entry
                //transistion
                wait(start_clock_flag);
                state = counting;
                break;
            }case counting: {
                //entry
                count();
                runGame.enable_clock_done();
                //transistion
                state = idle;
                break;
            }
        }
    }

    void count(){
        while (timer > 0){
            clock_timer.set(delay);
            wait(clock_timer);
            timer--;
        }
    }

public:
    clock(run_game& runGame_):
        runGame(runGame_)
    {}

    void set_timer(const int& x){
        timer = x;
    }

    void enable_start_clock_flag(){
        start_clock_flag.set();
    }
};

class DisplayControl{
private:
    //oled
    hwlib::glcd_oled& oled;
    //window
    hwlib::window& w1;
    hwlib::window& w2;
    //font
    hwlib::font_default_8x8 f1;
    //terminal
    hwlib::terminal_from d1;
    hwlib::terminal_from d2;

public:
    DisplayControl(hwlib::glcd_oled& oled_, hwlib::window& w1_, hwlib::window& w2_):
            oled(oled_),
            w1(w1_),
            w2(w2_),
            d1(w1,f1),
            d2(w2,f1)
    {}

    void SetDisplayMessage(){
        if(receive_hit == true){
            d2  << "\f" << "HIT! - " /*damage_score()*/ << "health";
                << hwlib::flush;
                hwlib::wait_ms(1000);

        }else if (gamesettings_receive == true){
            d2  << "\f" << "Setting received!";
                << hwlib::flush;
                hwlib::wait_ms(1000);
        }else{
            d2 << "\f"
                << hwlib::flush;
        }
    }
    /*SetDisplayMessage() //d2
    SetDisplayGameStatus(health, time) // d1
    SetShowInvoer() //d2

    //----------------------------

    stats
    //----------------------------
    invoer / message
    hit()

    //----------------------------
    */
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
    //runner r = {m, 500 * rtos::ms};

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
    auto scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    auto sda = hwlib::target::pin_oc( hwlib::target::pins::sda );

    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );

    auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

    auto w1 = hwlib::part(oled,hwlib::xy( 0, 0 ),hwlib::xy( 128, 32));
    auto w2 = hwlib::part(oled,hwlib::xy( 0, 33 ),hwlib::xy( 128, 64));
    //END_OLED

    //START CODE
    rtos::run();
}