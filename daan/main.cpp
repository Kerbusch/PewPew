#include "hwlib.hpp"
#include "rtos.hpp"
#include <bitset>

uint8_t tijd_countdown = 2;


struct gun_data{
    uint8_t health = 100; // default = 100;
    uint8_t power = 10; // default = 10;
    uint8_t player;
};

class ir_send {
private:
    hwlib::pin_direct_from_out_t led;

    void send_bit(bool x){
        if(x){
            led.write(true);
            hwlib::wait_us(1600);
            led.write(false);
            hwlib::wait_us(800);
            return;
        }else{
            led.write(true);
            hwlib::wait_us(800);
            led.write(false);
            hwlib::wait_us(1600);
            return;
        }
    }

    void send_2byte(uint16_t data){
        //Deze funtie keer stop de bit in de send_bit() funtie van links naar rechts.
        uint16_t mask = 0x8000;
        for(int i = 15; i >= 0; i--){
            send_bit((data & mask) >> i);
            mask = mask >> 1;
        }
    }

public:
    ir_send(hwlib::pin_out& led_):
        led(led_)
    {}

    void send_data(uint8_t n, uint8_t data, bool repeat){
        uint16_t d = 1;
        d = d << 5;
        d |= n;
        d = d << 5;
        d |= data;
        for(int i = 4; i >= 0; i--){
            d = d << 1;
            d |= (((n & (1 << i)) >> i) ^ ((data & (1 << i)) >> i)); //xor
        }
        send_2byte(d);
        if(repeat){
            hwlib::wait_ms(3);
            send_2byte(d);
        }
    }

    void send_tijd_countdown(const uint8_t& n, uint16_t tijd, uint8_t countdown){
        //tijd = 1011010000, countdown = 111100
        uint8_t data[4];
        uint8_t mask0 = 31; //00011111
        uint16_t mask1 = 0x3E0; //11 1110 0000
        data[0] = (tijd & mask1) >> 5;
        data[1] = (tijd & mask0);
        data[2] = (countdown & mask1) >> 5;
        data[3] = (countdown & mask0);
        send_data(n,tijd_countdown,true);
        for(int i = 0; i < 4; i++){
            hwlib::wait_ms(3);
            send_data(n,data[i],false);
        }
    }
};

class trigger{
private:
    hwlib::pin_direct_from_out_t led;
    hwlib::pin_direct_from_in_t sw;

    //rtos::flag trigger_pressed_flag;

public:
    trigger(hwlib::pin_out& led_, hwlib::pin_in& sw_):
        led(led_),
        sw(sw_)
    {}

    void update(){
        while(true){
            hwlib::cout << sw.read() << "\n";
            led.write(!sw.read());
            hwlib::wait_ms(30);
        }
    }
};

//run_game

int main( void ){	
   
    // IR output LED
    auto ir = hwlib::target::d2_36kHz();
    auto led_yellow = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto sw0 = hwlib::target::pin_in( hwlib::target::pins::d3 );

    ir_send s = {ir};
    uint16_t tijd = 720; // 12min in seconde
    s.send_tijd_countdown(1,tijd,60);

    /*while(true){
        s.send_tijd_countdown(1,tijd,60);
        hwlib::wait_ms(100);
    }*/

    trigger t = {led_yellow,sw0};
    t.update();
    hwlib::cout << "end\n";
}