#include "hwlib.hpp"
#include "rtos.hpp"
#include <bitset>

//def data types
uint8_t tijd_countdown = 2;

struct gun_data{
    uint8_t health = 100; // default = 100;
    uint8_t power = 10; // default = 10;
    uint8_t player;
};

class Speaker{
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
};

class SpeakerControl{
private:
    Speaker& speaker;
public:
    SpeakerControl(Speaker& speaker_):
            speaker(speaker_)
    {}

    void peew(){
        speaker.beep(20'000, 200'000, 990 );
    }
};

class ir_send : public rtos::task<>{
private:
    hwlib::pin_direct_from_out_t led;

    rtos::mutex & mutex;

    rtos::timer send_timer_long = {this, "send_timer_long"};
    long long int delay_long = 1600 * rtos::us;
    rtos::timer send_timer_short = {this, "send_timer_short"};
    long long int delay_short = 800 * rtos::us;
    rtos::timer send_timer_3ms = {this, "send_timer_3ms"};
    long long int delay_3ms = 3 * rtos::ms;

    rtos::channel<uint16_t , 1024> buffer;

    void send_bit(bool x){
        if(x){
            mutex.wait();
            led.write(true);
            send_timer_long.set(delay_long);
            wait(send_timer_long);
            mutex.signal();
            led.write(false);
            send_timer_short.set(delay_short);
            wait(send_timer_short);
            return;
        }else{
            mutex.wait();
            led.write(true);
            send_timer_short.set(delay_short);
            wait(send_timer_short);
            mutex.signal();
            led.write(false);
            send_timer_long.set(delay_long);
            wait(send_timer_long);
            return;
        }
    }

    void main(){
        while(true){
            auto x = buffer.read();
            uint16_t mask = 0x8000;
            for(int i = 15; i >= 0; i--){
                send_bit((x & mask) >> i);
                mask = mask >> 1;
            }
            send_timer_3ms.set(delay_3ms);
            wait(send_timer_3ms);
        }
    }

public:
    ir_send(hwlib::pin_out& led_, rtos::mutex& mutex_):
        rtos::task<>(1),
        led(led_),
        mutex(mutex_),
        buffer(this, "uint16_t buffer")
    {}

    void add(uint16_t x){
        buffer.write(x);
    }
};

class message_interpreter{
private:
    ir_send& ir_Send;

public:
    message_interpreter(ir_send& ir_Send_):
        ir_Send(ir_Send_)
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
        ir_Send.add(d);
        if(repeat){
            hwlib::wait_ms(30);
            ir_Send.add(d);
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
            hwlib::wait_ms(30);
            send_data(n,data[i],false);
        }
    }
};

class runner: public rtos::task<> {
private:
    message_interpreter& m;
    rtos::clock runner_clock;

    void main(){
        while(true){
            wait( runner_clock );
            m.send_tijd_countdown(7,720,60);
        }
    }

public:
    runner(message_interpreter& m_, long long int delay):
        m(m_),
        runner_clock( this, delay, "runner_clock" )
    {}
};

class Trigger: public rtos::task<>{
private:
    hwlib::pin_direct_from_out_t led;
    hwlib::pin_direct_from_in_t sw;

    SpeakerControl& speakerControl;

    long long int delay;
    rtos::timer trigger_timer;

    rtos::flag trigger_pressed_flag;

    void main(){
        bool switch_last = false;
        while(true){
            trigger_timer.set(delay);
            if(!sw.read() && !switch_last){
                switch_last = true;
                trigger_pressed_flag.set();
                led.write(true);
                speakerControl.peew();
            }else if(sw.read() && switch_last){
                switch_last = false;
                led.write(false);
            }
            wait( trigger_timer );
        }
    }

public:
    Trigger(hwlib::pin_out& led_, hwlib::pin_in& sw_, SpeakerControl& speakerControl_):
        led(led_),
        sw(sw_),
        speakerControl(speakerControl_),
        delay(30 * rtos::ms),
        trigger_timer( this, "trigger_timer"),
        trigger_pressed_flag(this, "trigger_pressed_flag")
    {}
};

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
    message_interpreter m = {s};
    runner r = {m, 500 * rtos::ms};

    //speaker gpio
    auto speaker_pin = hwlib::target::pin_out( hwlib::target::pins::d7 );

    //speaker control code
    Speaker speaker = {speaker_pin};
    SpeakerControl speakerControl = {speaker};

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
