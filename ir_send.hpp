#ifndef IR_HPP
#define IR_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "struct.hpp"

//def data types
const int TIJD_AND_COUNTDOWN = 2;
const int HIT_DAMAGE = 3;


/// @brief ir_send class
/// @details This class gets a uint16_t in its channel and chops this into bits and sends those bits.
class ir_send : public rtos::task<>{
private:
    /// @brief Enum of the different states
    enum state_t{
        idle,
        running
    };
    /// @brief Selects the starting state
    state_t state = idle;

    /// @brief Led member for the ir led
    hwlib::pin_direct_from_out_t led;

    /// @brief send_timer_long timer for the long part of the signal.
    rtos::timer send_timer_long = {this, "send_timer_long"};
    /// @brief delay_long, a long long int for the time of the send_timer_long timer.
    long long int delay_long = 1600 * rtos::us;
    /// @brief send_timer_long timer for the long part of the signal.
    rtos::timer send_timer_short = {this, "send_timer_short"};
    /// @brief delay_short, a long long int for the time of the send_timer_short timer.
    long long int delay_short = 800 * rtos::us;
    /// @brief send_timer_long timer for the long part of the signal.
    rtos::timer send_timer_3ms = {this, "send_timer_3ms"};
    /// @brief delay_3ms, a long long int for the time of the send_timer_3ms timer.
    long long int delay_3ms = 3 * rtos::ms;
    /// @brief send_unit16_t_channel for receiving the uint16_t for the ir sender.
    rtos::channel<uint16_t , 1024> send_unit16_t_channel;


    /// @brief Send_bit function
    /// @details This function translates the input but to a on or off signal for the led
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

    /// @brief Main function for the rtos::task<>.
    /// @details State switcher for the different states
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
    /// @brief constructor for the ir_send class
    /// @details This constructor initializes the reference members.
    /// @param led_ , a hwlib::pin_out
    ir_send(hwlib::pin_out& led_):
            task(100, "ir_send"),
            led(led_),
            send_unit16_t_channel(this, "send_unit16_t_channel")
    {}

    /// @brief Add function for the send_unit16_t_channel channel
    void add(uint16_t x){
        send_unit16_t_channel.write(x);
    }
};

/// @brief message_writing class
/// @details Class for making a uint16_t from the given data and sending it to ir_send.
class message_writing : public rtos::task<>{
private:
    /// @brief Enum of the different states
    enum state_t{
        idle,
        sendStartgame,
        sendShot
    };
    /// @brief Selects the starting state
    state_t state = idle;

    /// @brief ir_send member as reference
    ir_send& ir_Send;

    /// @brief Player member
    int player = 31;

    /// @brief tijd_countdown channel for receiving the time and countdown from run_game.
    rtos::channel<tijd_countdown , 1024> send_tijd_countdown_channel;
    /// @brief send_tijd_countdown_flag flag knowing when there's something in the channel.
    rtos::flag send_tijd_countdown_flag = {this, "send_tijd_countdown_flag"};
    /// @brief send_shoot_channel channel for receiving the time and countdown from run_game.
    rtos::channel<gun_data , 1024> send_shoot_channel;
    /// @brief send_shoot_flag flag knowing when there's something in the channel.
    rtos::flag send_shoot_flag = {this, "send_shoot_flag"};

    /// @brief Main function for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        while (true){
            switch (state) {
                case idle: {
                    //hwlib::cout << "idle-message_writing\n";
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
                    //hwlib::cout << "sendStartgame-message_writing\n";
                    //entry
                    tijd_countdown channel_read = send_tijd_countdown_channel.read();
                    send_data(player,channel_read.tijd,true);
                    //transition
                    state = idle;
                    break;
                }case sendShot: {
                    //hwlib::cout << "sendShot-message_writing\n";
                    //entry
                    auto channel_read = send_shoot_channel.read();
                    send_data(player,channel_read.power,true);
                    //transition
                    state = idle;
                    break;
                }
            }
        }
    }

    /*void send_tijd_countdown(const uint8_t& n, const tijd_countdown& data){ // 2 == tijd_countdown
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
    }*/

public:
    /// @brief Constructor of the message_writing class.
    /// @details This constructor sets the pins and speakerControl as members.
    /// @param ir_Send_ This is a ir_send class for sending the bytes.
    message_writing(ir_send& ir_Send_):
            ir_Send(ir_Send_),
            send_tijd_countdown_channel(this, "send_tijd_countdown_channel"),
            send_shoot_channel(this, "send_shoot_channel")
    {}

    /// @brief Send_data function
    /// @details This function gets makes a proper uint16_t for the inputs and sends them to the ir led.
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

    /// @brief Add input to send_tijd_countdown_channel channel.
    void add_tijd_countdown(tijd_countdown x){
        send_tijd_countdown_channel.write(x);
    }

    /// @brief Add input to send_shoot_channel channel.
    void add_shoot(gun_data x){
        send_shoot_channel.write(x);
    }

    /// @brief Enable function for the send_tijd_countdown_flag flag
    void enable_send_tijd_countdown_flag(){
        send_tijd_countdown_flag.set();
    }

    /// @brief Enable function for the send_shoot_flag flag
    void enable_send_shoot_flag(){
        send_shoot_flag.set();
    }

    /// @brief Function for setting the sending player so the class know where the message is coming from.
    void set_player(const int& i){
        player = i;
        hwlib::cout << "player: " << player << "\n";
    }
};

#endif //IR_HPP
