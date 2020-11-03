#ifndef IR_HPP
#define IR_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "struct.hpp"

//def data types
const int TIJD_AND_COUNTDOWN = 2;
const int HIT_DAMAGE = 3;

class ir_send : public rtos::task<>{
private:
    enum state_t{
        idle,
        running
    };

    state_t state = idle;

    hwlib::pin_direct_from_out_t led;

    //rtos::mutex & mutex;

    rtos::timer send_timer_long = {this, "send_timer_long"};
    long long int delay_long = 1600 * rtos::us;
    rtos::timer send_timer_short = {this, "send_timer_short"};
    long long int delay_short = 800 * rtos::us;
    rtos::timer send_timer_3ms = {this, "send_timer_3ms"};
    long long int delay_3ms = 3 * rtos::ms;

    rtos::channel<uint16_t , 1024> send_unit16_t_channel;

    void send_bit(bool x){
        if(x){
            //mutex.wait();
            led.write(true);
            send_timer_long.set(delay_long);
            wait(send_timer_long);
            //mutex.signal();
            led.write(false);
            send_timer_short.set(delay_short);
            wait(send_timer_short);
            return;
        }else{
            //mutex.wait();
            led.write(true);
            send_timer_short.set(delay_short);
            wait(send_timer_short);
            //mutex.signal();
            led.write(false);
            send_timer_long.set(delay_long);
            wait(send_timer_long);
            return;
        }
    }

    void main(){
        while(true){
            switch (state) {
                case idle:
                    //entry
                    //rest
                    wait(send_unit16_t_channel);
                    state = running;
                    break;
                case running:
                    //entry
                    auto x = send_unit16_t_channel.read();
                    uint16_t mask = 0x8000;
                    for(int i = 15; i >= 0; i--){
                        send_bit((x & mask) >> i);
                        mask = mask >> 1;
                    }
                    //done
                    send_timer_3ms.set(delay_3ms);
                    wait(send_timer_3ms);
                    state = idle;
                    break;
            }
        }
        /*while(true){ //deze code is veel beter en sneller, maar het MOET met states!
            auto x = send_unit16_t_channel.read();
            uint16_t mask = 0x8000;
            for(int i = 15; i >= 0; i--){
                send_bit((x & mask) >> i);
                mask = mask >> 1;
            }
            send_timer_3ms.set(delay_3ms);
            wait(send_timer_3ms);
        }*/
    }

public:
    ir_send(hwlib::pin_out& led_/*, rtos::mutex& mutex_*/):
            rtos::task<>(100),
            led(led_),
            //mutex(mutex_),
            send_unit16_t_channel(this, "send_unit16_t_channel")
    {}

    void add(uint16_t x){
        send_unit16_t_channel.write(x);
    }
};

class message_writing : public rtos::task<>{
private:
    enum state_t{
        idle,
        sendStartgame,
        sendShot
    };
    state_t state = idle;
    ir_send& ir_Send;

    rtos::channel<tijd_countdown , 1024> send_tijd_countdown_channel;
    rtos::flag send_tijd_countdown_flag = {this, "send_tijd_countdown_flag"};
    rtos::channel<gun_data , 1024> send_shoot_channel;
    rtos::flag send_shoot_flag = {this, "send_shoot_flag"};

    void main(){
        while (true){
            switch (state) {
                case idle: {
                    hwlib::cout << "idle-message_writing\n";
                    //entry
                    //transition
                    auto evt = wait(send_tijd_countdown_flag + send_shoot_flag);
                    if (evt == send_tijd_countdown_flag) {
                        state = sendStartgame;
                    } else if(evt == send_shoot_flag){
                        state = sendShot;
                    }
                    break;
                }case sendStartgame: {
                    hwlib::cout << "sendStartgame-message_writing\n";
                    //entry
                    auto channel_read = send_tijd_countdown_channel.read();
                    send_tijd_countdown(31, channel_read);
                    //transition
                    state = idle;
                    break;
                }case sendShot: {
                    hwlib::cout << "sendShot-message_writing\n";
                    //entry
                    auto channel_read = send_shoot_channel.read();
                    send_shoot(channel_read.number, channel_read.power);
                    //transition
                    state = idle;
                    break;
                }
            }
        }
    }

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

    void send_tijd_countdown(const uint8_t& n, const tijd_countdown& data){ // 2 == tijd_countdown
        //tijd = 1011010000, countdown = 111100
        uint8_t data_uitvoer[4];
        uint8_t mask0 = 31; //00011111
        uint16_t mask1 = 0x3E0; //11 1110 0000
        data_uitvoer[0] = (data.tijd & mask1) >> 5;
        data_uitvoer[1] = (data.tijd & mask0);
        data_uitvoer[2] = (data.countdown & mask1) >> 5;
        data_uitvoer[3] = (data.countdown & mask0);
        send_data(n,TIJD_AND_COUNTDOWN,true);
        for(int i = 0; i < 4; i++){
            hwlib::wait_ms(30);
            send_data(n,data_uitvoer[i],false);
        }
    }

    void send_shoot(const int& from_player, const int& power){ // 3 == shoot
        //altijd naar 31
        send_data(31,HIT_DAMAGE,false);
        send_data(31,from_player,false);
        send_data(31,from_player,false);
    }

public:
    /// @brief Constructor of the message_writing class.
    /// @details This constructor sets the pins and speakerControl as members.
    /// @param ir_Send_ This is a hwlib::pin_out& for the led of trigger.
    message_writing(ir_send& ir_Send_):
            ir_Send(ir_Send_),
            send_tijd_countdown_channel(this, "send_tijd_countdown_channel"),
            send_shoot_channel(this, "send_shoot_channel")
    {}

    /// @brief add input to send_tijd_countdown_channel channel.
    void add_tijd_countdown(tijd_countdown x){
        send_tijd_countdown_channel.write(x);
    }

    /// @brief add input to send_shoot_channel channel.
    void add_shoot(gun_data x){
        send_shoot_channel.write(x);
    }

    void enable_send_tijd_countdown_flag(){
        send_tijd_countdown_flag.set();
    }

    void enable_send_shoot_flag(){
        send_shoot_flag.set();
    }
};

#endif //IR_HPP
