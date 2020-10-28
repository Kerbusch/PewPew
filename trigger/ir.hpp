#ifndef IR_HPP
#define IR_HPP

#include "struct.hpp"
#include <vector>

//def data types
const int TIJD_AND_COUNTDOWN = 2;
const int HIT_DEMAGE = 3;

//temp class
class run_game{
public:
gun_data gun_data1 = {1,10,100};
};

class ir_send : public rtos::task<>{
private:
    enum state_t{
        idle,
        running
    };

    state_t state = idle;

    hwlib::pin_direct_from_out_t led;

    rtos::mutex & mutex;

    rtos::timer send_timer_long = {this, "send_timer_long"};
    long long int delay_long = 1600 * rtos::us;
    rtos::timer send_timer_short = {this, "send_timer_short"};
    long long int delay_short = 800 * rtos::us;
    rtos::timer send_timer_3ms = {this, "send_timer_3ms"};
    long long int delay_3ms = 3 * rtos::ms;

    rtos::channel<uint16_t , 1024> send_unit16_t_channel;

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
    ir_send(hwlib::pin_out& led_, rtos::mutex& mutex_):
            rtos::task<>(1),
            led(led_),
            mutex(mutex_),
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
    rtos::channel<gun_data , 1024> send_shoot_channel;

    void main(){
        while (true){
            switch (state) {
                case idle: {
                    //entry
                    //transition
                    auto evt = wait(send_tijd_countdown_channel + send_shoot_channel);
                    if (evt == send_tijd_countdown_channel) {
                        state = sendStartgame;
                    } else if (evt == send_shoot_channel) {
                        state = sendShot;
                    }
                    break;
                }case sendStartgame: {
                    //entry
                    auto channel_read = send_tijd_countdown_channel.read();
                    send_tijd_countdown(31, channel_read);
                    //transition
                    state = idle;
                    break;
                }case sendShot: {
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
        send_data(31,HIT_DEMAGE,false);
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
};

class message_translation : public rtos::task<>{
private:
    enum state_t{
        idle,
        for_me,
        data_type,
        report_tijd_countdown,
        report_hit

    };
    state_t state = idle;

    //members:
    run_game& runGame;
    //start game

    int number = -1;

    std::vector<uint16_t> input_all = {};

    //rtos members:
    rtos::channel<uint16_t , 1024> receive_bytes_channel;

    void main(){
        while (true){
            switch (state) {
                case idle: {
                    //entry
                    //transition
                    auto evt = wait(receive_bytes_channel);
                    if (evt == receive_bytes_channel) {
                        extract_half_word_to_vector();
                        state = for_me;
                    }
                    break;
                }case for_me: {
                    //entry
                    bool check_t = check_for_me(input_all[0]);
                    //transition
                    if(check_t){
                        state = data_type;
                    }else{
                        clean();
                        state = idle;
                    }
                    break;
                }case data_type: {
                    //entry
                    extract_half_word_to_vector();
                    int type_t = extract_data(input_all[0]);
                    if(type_t == TIJD_AND_COUNTDOWN){
                        for(int i = 0; i < 4; i++){
                            extract_half_word_to_vector();
                        }
                        state = report_tijd_countdown;
                    }else if(type_t == HIT_DEMAGE){
                        for(int i = 0; i < 2; i++){
                            extract_half_word_to_vector();
                        }
                        state = report_hit;
                    }
                    //transition
                    break;
                }case report_tijd_countdown: {
                    //entry
                    get_tijd_countdown();
                    //transition
                    state = idle;
                    break;
                }case report_hit: {
                    //entry
                    get_hit();
                    //transition
                    state = idle;
                    break;
                }
            }
        }
    }

public:
    // channel =    1-11111-00010-11101 > data = 2 dus 6 bytes
    //              1-11111-00010-11101
    //              1-11111-10110-xor
    //              1-11111-10000-xor
    //              1-11111-00010-xor
    //              1-11111-11010-xor
    message_translation(run_game& runGame_):
        runGame(runGame_),
        receive_bytes_channel(this, "receive_bytes_channel")
    {}

    void extract_half_word_to_vector(){
        uint16_t x = receive_bytes_channel.read();
        //check complete;
        input_all.push_back(x);
    }

    bool check_complete(uint16_t input){
        //1 start bit
        //5 number (to) bits
        //5 data bits
        //55 xor bits
        return input;
    }

    uint8_t extract_data(uint16_t input){
        //get data
        uint16_t mask = 0x3E0; // 00000011 11100000
        input &= mask;
        input = input >> 5;
        return input;
    }

    bool check_for_me(uint16_t input){
        //if number = gun_data.number of 31
        uint16_t mask = 0x7C00; // 01111100 00000000
        input &= mask;
        input = input >> 10;
        if(input == 31 || input == runGame.gun_data1.number){
            return true;
        }return false;
    }

    int get_data_type(uint16_t input){
        //get data first or 2nd half word for type
        uint16_t mask = 0x3E0; // 00000011 11100000
        input &= mask;
        input = input >> 5;
        return input;
    }

    bool check_double(const uint16_t& input1, const uint16_t& input2){
        return input1 == input2;
    }

    hit get_hit(){ // data type == 3
        uint8_t from = extract_data(input_all[2]);
        uint8_t demage = extract_data(input_all[3]);
        clean();
        return hit{from,demage}; //Naar channel!!!! <-----------------------------------------------
    }

    tijd_countdown get_tijd_countdown(){
        uint16_t t, c;
        t = extract_data(input_all[2]);
        t = t << 5;
        t |= extract_data(input_all[3]);
        c = extract_data(input_all[4]);
        c = c << 5;
        c |= extract_data(input_all[5]);
        clean();
        return tijd_countdown{t,c}; //Naar channel!!!! <-----------------------------------------------
    }

    void clean() {
        input_all = {};
    }
};

/*class runner: public rtos::task<> {
private:
    message_writing& m;
    rtos::clock runner_clock;

    void main(){
        while(true){
            wait( runner_clock );
            m.send_tijd_countdown(7,720,60);
        }
    }

public:
    runner(message_writing& m_, long long int delay):
            m(m_),
            runner_clock( this, delay, "runner_clock" )
    {}
};*/

#endif //IR_HPP
