#ifndef IR_RECEIVE_HPP
#define IR_RECEIVE_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

#include "struct.hpp"
#include "run_game.hpp"
#include "start_game.hpp"

//def data types
const int TIJD_AND_COUNTDOWN2 = 2;
const int HIT_DAMAGE2 = 3;

class message_translation : public rtos::task<>{
private:
    /// @brief enum state_t for the different states of the class
    /*enum state_t{
        idle,
        for_me,
        data_type,
        report_tijd_countdown,
        report_hit
    };

    /// @brief state of the class
    state_t state = idle;*/

    //members:
    /// @brief run_game
    //run_game& runGame;
    /// @brief startgame
    //start_game& startGame;

    /// @brief vector<uint16_t> of the input
    //std::vector<uint16_t> input_all = {};
    //uint16_t input_all[6];

    //rtos members:
    /// @brief rtos::channel with the received bytes
    //rtos::channel<uint16_t , 32> receive_bytes_channel = {this, "receive_bytes_channel"};

    rtos::timer timer_ggg = {this, "timer_ggg"};

    /// @brief main funtion for rtos
    /// @details state switch for the different states
    void main(){
        while (true){
            timer_ggg.set(rtos::ms * 100);
            wait(timer_ggg);
            hwlib::cout << "KUT RTOS\n";
            /*switch (state) {
                case idle: {
                    //entry
                    //transition
                    wait(receive_bytes_channel);
                    extract_half_word_to_vector();
                    state = for_me;
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
                    if(type_t == TIJD_AND_COUNTDOWN2){
                        for(int i = 0; i < 4; i++){
                            extract_half_word_to_vector();
                        }
                        state = report_tijd_countdown;
                    }else if(type_t == HIT_DAMAGE2){
                        for(int i = 0; i < 2; i++){
                            extract_half_word_to_vector();
                        }
                        state = report_hit;
                    }
                    //transition
                    break;
                }case report_tijd_countdown: {
                    //entry
                    input_tijd_countdown();
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
            }*/
        }
    }

public:
    /*// channel =    1-11111-00010-11101 > data = 2 dus 6 bytes
    //              1-11111-00010-11101
    //              1-11111-10110-xor
    //              1-11111-10000-xor
    //              1-11111-00010-xor
    //              1-11111-11010-xor
    /// @brief Constructor of the message_translation class.
    /// @details This constructor sets the run_game and start_game classes as members.
    /// @param runGame_ This is a run_game class.
    /// @param startGame_ This is a start_game class.
    message_translation(run_game& runGame_, start_game& startGame_):
            runGame(runGame_),
            startGame(startGame_)
    {}

    /// @brief Extracts the half word (16bits)
    /// @details This funtion gets the first 16 bits of the channel and puts them in the vector input_all.
    /// @see input_all.
    void extract_half_word_to_vector(){
        uint16_t x = receive_bytes_channel.read();
        //check complete;
        //input_all.push_back(x);
        for(int i = 0; i < 6; i ++){
            if(input_all[i] == 0){
                input_all[i] = x;
                return;
            }
        }
    }

    /// @brief Checks if the 16 bits are complete
    /// @details Gets the values in the function. The function make the xor of the number and data part and compares it to the xor of the message.
    bool check_complete(uint16_t input){
        return ((extract_for(input) ^ extract_data(input)) == extract_xor(input));
    }

    /// @brief Extracts the data part of the 16 bits
    /// @details Gets bit 6 - 10 of the input.
    uint8_t extract_data(uint16_t input){
        //get data
        uint16_t mask = 0x3E0; // 00000011 11100000
        input &= mask;
        input = input >> 5;
        return input;
    }

    /// @brief Extracts the for part of the 16 bits
    /// @details Gets bit 1 - 5 of the input.
    uint8_t extract_for(uint16_t input){
        //get data
        uint16_t mask = 0x7C00; // 01111100 00000000
        input &= mask;
        input = input >> 10;
        return input;
    }

    /// @brief Extracts the xor part of the 16 bits
    /// @details Gets bit 1 - 15 of the input.
    uint8_t extract_xor(uint16_t input){
        //get xor
        uint16_t mask = 0x1F; // 00000000 00011111
        input &= mask;
        return input;
    }

    /// @brief Checks if the is for this player
    /// @details Checks if the for part of the input if for this player or for everyone (31).
    bool check_for_me(uint16_t input){
        input = extract_for(input);
        if(input == 31 || input == runGame.get_player_number()){
            return true;
        }return false;
    }

    //int get_data_type(uint16_t input){
    //    //get data first or 2nd half word for type
    //    uint16_t mask = 0x3E0; // 00000011 11100000
    //    input &= mask;
    //    input = input >> 5;
    //    return input;
    //}

    /// @brief Checks if 2 16bits are double
    bool check_double(const uint16_t& input1, const uint16_t& input2){
        return input1 == input2;
    }

    /// @brief x
    /// @details v
    void get_hit(){ // data type == 3
        uint8_t f = extract_data(input_all[2]);
        uint8_t d = extract_data(input_all[3]);
        clean();
        runGame.add_hit(hit{f,d});                            //Naar channel!!!! <-----------------------------------------------
        runGame.enable_run_game_hit_flag();
    }

    /// @brief x
    /// @details v
    void input_tijd_countdown(){
        uint16_t t, c;
        t = extract_data(input_all[2]);
        t = t << 5;
        t |= extract_data(input_all[3]);
        c = extract_data(input_all[4]);
        c = c << 5;
        c |= extract_data(input_all[5]);
        clean();
        startGame.add(tijd_countdown{t,c});
    }

    /// @brief Cleans the input_all vector
    /// @details v
    void clean() {
        for(int i = 0; i < 6; i ++){
            input_all[i] = 0;
        }
    }

    /// @brief Adds to channel
    /// @details Adds a uint16_t to the receive_bytes_channel channel.
    void add(uint16_t x){
        receive_bytes_channel.write(x);
    }*/
};

class ir_receiver : public rtos::task<>{
private:

    enum state_t{
        idle,
        receive_bits
    };

    state_t state = idle;

    hwlib::target::pin_in& signal;

    rtos::timer receive_timer = {this, "receive_timer"};
    long long int delay = 1200 * rtos::us;
    rtos::timer receive_timer_1us = {this, "receive_timer_1us"};
    long long int start_delay_1us = 1 * rtos::us;

    message_translation& messageTranslation;

    uint16_t message = 0x00;

    void main(){
        while(true){
            switch(state){
                case idle:{
                    //entry
                    signal.refresh();
                    //transition
                    if (signal.read() == 1){
                        state = receive_bits;
                    }
                    receive_timer.set(delay);
                    wait(receive_timer);
                    break;
                }
                case receive_bits:{
                    //entry
                    ir_receive(message);
                    //transition
                    state = idle;
                    break;
                }
            }
        }
    }

    void ir_receive(uint16_t& message){
        signal.refresh();
        if(signal.read() == 1) {
            for (int i = 0; i < 16; ++i) {
                receive_timer.set(delay);
                wait(receive_timer);
                signal.refresh();
                if (signal.read() == 1) {
                    message = message << 1;
                    message |= 0x01;
                } else{
                    message = message << 1;
                }
                receive_timer.set(delay);
                wait(receive_timer);
            }
            //messageTranslation.add(message);
            message = 0x00;
        }
    }

public:
    ir_receiver(hwlib::target::pin_in& signal_, message_translation& messageTranslation_):
            signal(signal_),
            messageTranslation(messageTranslation_)
    {}
};

#endif //IR_RECEIVE_HPP
