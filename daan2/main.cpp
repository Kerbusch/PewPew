#include "hwlib.hpp"
#include "rtos.hpp"
#include <vector>
#include <string>

//moet in eigen hpp
struct tijd_countdown{
    int tijd;
    int countdown = 60;
};

struct gun_data{
    int number;
    int power;
    int health = 100;
};

struct hit{
    int from;
    int damage;
};

struct game_parameters_stuct{
    //tijd_countdown
    int tijd;
    int countdown = 60;
    //gun_data
    int number;
    int power;
    int health = 100;
};

//def data types
const int TIJD_AND_COUNTDOWN = 2;
const int HIT_DAGAME = 3;

class DisplayControl{
private:
    //oled
    hwlib::glcd_oled& oled;
    //window
    hwlib::window& w1;
    hwlib::window& w2;
    hwlib::window& w3;
    //font
    hwlib::font_default_8x8 f1;
    //terminal
    hwlib::terminal_from d1; //tijd en countdown
    hwlib::terminal_from d2; //stats en invoer
    hwlib::terminal_from d3; //stats en invoer

    //rtos::timer display_timer = {this, "display_timer"};

public:
    DisplayControl(hwlib::glcd_oled& oled_, hwlib::window& w1_, hwlib::window& w2_, hwlib::window& w3_):
            oled(oled_),
            w1(w1_),
            w2(w2_),
            w3(w3_),
            d1(w1,f1),
            d2(w2,f1),
            d3(w3,f1)
    {
        d1 << "\f" << "";
        d2 << "\f" << "";
        d3 << "\f" << "";
        oled.flush();
    }

    void SetDisplayHit(const hit& input){ //d2
        d2  << "\f" << "HIT! - " << input.damage << "health";
        oled.flush();
        //hwlib::wait_ms(1000);
        /*if(receive_hit == true){

        }else if (gamesettings_receive == true){

        }else{
            d2 << "\f"
               << hwlib::flush;
        }*/
    }

    void SetDisplayStats(const game_parameters_stuct& input) { //d3
        //d2 << "\f" << "Setting received!";
        //oled.flush();
        //health:
        //power:
    }

    void SetDisplayInput(const char* input, const int& n, const char* input2){ //d3
        d3  << "\f" << input << n << input2;
        oled.flush();
    }

    void SetDisplayMessageD1(const char* input){ //d1
        d1  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageD2(const char* input){ //d2
        d2  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageD3(const char* input){ //d3
        d3  << "\f" << input;
        oled.flush();
    }

    void SetDisplayMessageFadeD2(const char* input/*, long long const int& fade_delay*/){ //d2
        d2  << "\f" << input;
        oled.flush();
        //display_timer.set(fade_delay);
        //wait(display_timer);
        hwlib::wait_ms(1000);
        d2  << "\f" << "";
        oled.flush();
    }

    void SetDisplayTime(const int& input){ //d1
        d1 << "Time left:\n" << input << "\n";
        oled.flush();
    }

    void SetDisplayCountdown(const int& input){ //d1
        d1 << "Start game in:\n" << input << "\n";
        oled.flush();
    }

    /*SetDisplayMessageD2() //d2
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

class game_parameters{
private:
    tijd_countdown tijdCountdown = {0 ,0};
    gun_data gunData;

public:
    void set_gun_data(const gun_data& input){
        hwlib::cout << "game_parameters_number-" << input.number << "\n";
        hwlib::cout << "game_parameters_power-" << input.power << "\n";
        hwlib::cout << "game_parameters_health-" << input.health << "\n";
        gunData = input;
    }

    void set_tijd_countdown(const tijd_countdown& input){
        hwlib::cout << "game_parameters_tijd-" << input.tijd << "\n";
        hwlib::cout << "game_parameters_countdown-" << input.countdown << "\n";
        tijdCountdown = input;
    }

    game_parameters_stuct get_game_parameters(){
        return game_parameters_stuct{tijdCountdown.tijd,tijdCountdown.countdown,gunData.number,gunData.power,gunData.health};
    }

    bool check_time_input(){
        return tijdCountdown.tijd != 0;
    }
};

class run_game : public rtos::task<>{
private:
    //flag
    rtos::flag clock_done = {this, "clock_done"};
    rtos::channel<game_parameters_stuct , 1024> run_game_parameters_channel = {this, "run_game_parameters_channel"};

    void main(){
        while(true){
            wait(clock_done);
        }
    }

public:
    gun_data gun_data1 = {1,10,100};

    void enable_clock_done(){
        clock_done.set();
    }

    void add(game_parameters_stuct x){
        run_game_parameters_channel.write(x);
    }
};

class start_game : public rtos::task<>{
private:
    enum state_t {
        idle,
        check,          //check complete
        get_time,       //get time from ir translator
        get_data        //get struct from entity
    };
    state_t state = idle;

    //reference members
    game_parameters& gameParameters;
    run_game& runGame;
    DisplayControl& displayControl;

    //rtos members
    rtos::flag start_game_flag = {this, "start_game_flag"};
    rtos::channel<tijd_countdown , 1024> start_game_channel = {this, "start_game_channel"};

    void main(){
        while(true) {
            switch (state) {
                case idle: {
                    hwlib::cout << "start_game idle\n";
                    //entry
                    //transtion
                    auto evt = wait(start_game_flag);
                    if (evt == start_game_flag) {
                        state = check;
                    }
                    break;
                }
                case check: {
                    hwlib::cout << "start_game check\n";
                    //entry
                    //transtion
                    if (gameParameters.check_time_input()) {
                        state = get_data;
                    } else {
                        state = get_time;
                    }
                    break;
                }
                case get_time: {
                    hwlib::cout << "start_game get_time\n";
                    //entry
                    //transtion
                    wait(start_game_channel);
                    gameParameters.set_tijd_countdown(start_game_channel.read());
                    state = get_data;
                    break;
                }
                case get_data: {
                    hwlib::cout << "start_game get_data\n";
                    //entry
                    displayControl.SetDisplayMessageFadeD2("Settings received");
                    runGame.add(gameParameters.get_game_parameters());
                    //transtion
                    state = idle;
                    break;
                }
            }
        }
    }

public:
    start_game(game_parameters& gameParameters_, run_game& runGame_, DisplayControl& displayControl_):
            gameParameters(gameParameters_),
            runGame(runGame_),
            displayControl(displayControl_)
    {}

    void enable_start_game_flag(){
        start_game_flag.set();
    }

    void add(tijd_countdown x){
        start_game_channel.write(x);
    }
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
        send_data(31,HIT_DAGAME,false);
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
    /// @brief enum state_t for the different states of the class
    enum state_t{
        idle,
        for_me,
        data_type,
        report_tijd_countdown,
        report_hit

    };

    /// @brief state of the class
    state_t state = idle;

    //members:
    /// @brief run_game
    run_game& runGame;
    /// @brief startgaem
    start_game& startGame;

    /// @brief vector<uint16_t> of the input
    std::vector<uint16_t> input_all = {};

    //rtos members:
    /// @brief rtos::channel with the received bytes
    rtos::channel<uint16_t , 1024> receive_bytes_channel;

    /// @brief main funtion for rtos
    /// @details state switch for the different states
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
                    }else if(type_t == HIT_DAGAME){
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
    /// @brief Constructor of the message_translation class.
    /// @details This constructor sets the run_game and start_game classes as members.
    /// @param runGame_ This is a run_game class.
    /// @param startGame_ This is a start_game class.
    message_translation(run_game& runGame_, start_game& startGame_):
            runGame(runGame_),
            startGame(startGame_),
            receive_bytes_channel(this, "receive_bytes_channel")
    {}

    /// @brief Extracts the half word (16bits)
    /// @details This funtion gets the first 16 bits of the channel and puts them in the vector input_all.
    /// @see input_all.
    void extract_half_word_to_vector(){
        uint16_t x = receive_bytes_channel.read();
        //check complete;
        input_all.push_back(x);
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
        if(input == 31 || input == runGame.gun_data1.number){
            return true;
        }return false;
    }

    /*int get_data_type(uint16_t input){
        //get data first or 2nd half word for type
        uint16_t mask = 0x3E0; // 00000011 11100000
        input &= mask;
        input = input >> 5;
        return input;
    }*/

    /// @brief Checks if 2 16bits are double
    bool check_double(const uint16_t& input1, const uint16_t& input2){
        return input1 == input2;
    }

    /// @brief x
    /// @details v
    hit get_hit(){ // data type == 3
        uint8_t f = extract_data(input_all[2]);
        uint8_t d = extract_data(input_all[3]);
        clean();
        return hit{f,d};                            //Naar channel!!!! <-----------------------------------------------
    }

    /// @brief x
    /// @details v
    tijd_countdown input_tijd_countdown(){
        uint16_t t, c;
        t = extract_data(input_all[2]);
        t = t << 5;
        t |= extract_data(input_all[3]);
        c = extract_data(input_all[4]);
        c = c << 5;
        c |= extract_data(input_all[5]);
        clean();
        return tijd_countdown{t,c};                 //Naar channel!!!! <-----------------------------------------------
    }

    /// @brief Cleans the input_all vector
    /// @details v
    void clean() {
        input_all = {};
    }

    /// @brief Adds to channel
    /// @details Adds a uint16_t to the receive_bytes_channel channel.
    void add(uint16_t x){
        receive_bytes_channel.write(x);
    }
};

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

    int get_time(){
        return timer;
    }

    void enable_start_clock_flag(){
        start_clock_flag.set();
    }
};

class SpeakerControl: public rtos::task<> {
private:
    enum state_t{
        Idle,
        Play_hit,
        Play_shoot
    };

    state_t state = Idle;

    hwlib::pin_direct_from_out_t pin;

    rtos::timer beep_timer = {this, "beep_timer"};
    rtos::flag play_shoot_flag = {this, "play_shoot_flag"};
    rtos::flag play_hit_flag = {this, "play_hit_flag"};

    void main(){
        while (true) {
            switch (state) {
                case Idle: {
                    //entry
                    //normal
                    auto evt = wait(play_shoot_flag + play_hit_flag);
                    if (evt == play_shoot_flag) {
                        state = Play_shoot;
                    } else {
                        state = Play_hit;
                    }
                    break;
                }case Play_shoot: {
                    //entry
                    beep3();
                    //normal
                    state = Idle;
                    break;
                }case Play_hit: {
                    //entry
                    peew(); //nog geen hit geluid gemaakt
                    //normal
                    state = Idle;
                    break;
                }
            }
        }
    }

    void beep( int f, int d, int fd = 1000 ){
        auto end = hwlib::now_us() + d;
        while( end > hwlib::now_us() ){
            auto p = 500'000 / f;
            f = f * fd / 1000;
            pin.write( 1 );
            pin.flush();
            beep_timer.set(p * rtos::us);
            wait(beep_timer);
            pin.write( 0 );
            pin.flush();
            beep_timer.set(p * rtos::us);
            wait(beep_timer);
        }
    }

    void peew(){
        beep(20'000, 200'000, 990 );
    }

    void beep3(){
        for( int i = 0; i < 3; i++ ){
            beep( 1'000, 50'000 );
            beep_timer.set(20'000 * rtos::us);
            wait(beep_timer);
        }
    }

public:
    SpeakerControl(hwlib::pin_out& pin_):
            pin(pin_)
    {}

    void enable_play_shoot_flag(){
        play_shoot_flag.set();
    }

    void enable_play_hit_flag(){
        play_hit_flag.set();
    }
};

/// @brief Trigger Class
/// @details ...
class Trigger: public rtos::task<>{
private:
    /// @brief gpio pin for the led
    hwlib::pin_direct_from_out_t led;
    /// @brief gpio pin for the switch/trigger
    hwlib::pin_direct_from_in_t sw;

    /// @brief SpeakerControl member for enabling the play sound flags
    SpeakerControl& speakerControl;

    /// @brief delay for timer
    long long int delay;
    /// @brief rtos::timer for periodical checking of input
    rtos::timer trigger_timer;
    /// @brief NOT RIGHT NEEDS TO BE CHANGED
    rtos::flag trigger_pressed_flag;
    //channel

    /// @brief main funtion for the rtos::task<>. Wil run on rtos::run();
    void main(){
        bool switch_last = false;
        while(true){
            if(!sw.read() && !switch_last){
                switch_last = true;
                speakerControl.enable_play_shoot_flag(); //moet vervangen worden door "trigger_pressed_flag" als run game gemaakt is.
                led.write(true);
            }else if(sw.read() && switch_last){
                switch_last = false;
                led.write(false);
            }
            trigger_timer.set(delay);
            wait( trigger_timer );
        }
    }

public:
    /// @brief Constructor of the Trigger class.
    /// @details This constructor sets the pins and speakerControl as members.
    /// @param led_ This is a hwlib::pin_out& for the led of trigger.
    /// @param sw_ This is a hwlib::pin_in& for the switch of the trigger.
    /// @param speakerControl_ This is a SpeakerControl for setting flags.
    Trigger(hwlib::pin_out& led_, hwlib::pin_in& sw_, SpeakerControl& speakerControl_):
            rtos::task<>(5),
            led(led_),
            sw(sw_),
            speakerControl(speakerControl_),
            delay(30 * rtos::ms),
            trigger_timer( this, "trigger_timer"),
            trigger_pressed_flag(this, "trigger_pressed_flag")
    {}
};

class KeypadControl{
private:
    hwlib::keypad<16>& keypad1;

public:
    KeypadControl(hwlib::keypad<16>& keypad1_):
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



class init_game : public rtos::task<>{
private:
    enum state_t{
        idle,
        normale,
        getting_time,
        getting_gunpower,
        getting_player,
        startgame
    };
    state_t state = normale;

    KeypadControl& keypadControl;
    DisplayControl& displayControl;
    game_parameters& gameParameters;
    start_game& startGame;

    gun_data gun = {0,10}; //default
    tijd_countdown tijd_data = {600}; //default

    rtos::flag restart_init_game_flag = {this, "restart_init_game_flag"};
    long long int delay = {rtos::ms * 200};
    rtos::timer wrong_timer = {this, "wrong_timer"};

    void main(){
        while(true) {
            switch (state) {
                case idle: {
                    //entry
                    //transition
                    wait(restart_init_game_flag);
                    state = normale;
                    break;
                }
                case normale: {
                    //entry
                    print_ui();
                    //transition
                    bool loop = true;
                    while (loop) {
                        char c = keypadControl.getc();
                        if (c == 'A') {
                            state = getting_player;
                            loop = false;
                        } else if (c == 'B') {
                            state = getting_gunpower;
                            loop = false;
                        } else if (c == 'C') {
                            state = getting_time;
                            loop = false;
                        } else if (c == '#') {
                            state = startgame;
                            loop = false;
                        }
                    }
                    break;
                }
                case getting_time: {
                    //entry
                    input_tijd();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case getting_gunpower: {
                    //entry
                    input_gunpower();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case getting_player: {
                    //entry
                    input_number();//de code komt hier uit als # ingedrukt word.
                    //transition
                    state = normale;
                    break;
                }
                case startgame: {
                    hwlib::cout << "startgame\n";
                    //entry
                    if(gun.number == 0){
                        gameParameters.set_tijd_countdown(tijd_data);
                    }
                    gameParameters.set_gun_data(gun);
                    startGame.enable_start_game_flag();
                    clear_ui();
                    state = idle;
                    //transition
                    break;
                }
            }
        }
    }

    void print_ui(){
        displayControl.SetDisplayMessageD1("Settings:");
        displayControl.SetDisplayMessageD2("A: player number\nB: gun power");
        displayControl.SetDisplayMessageD3("C: Tijd\n#: Done");
    }

    void clear_ui(){
        displayControl.SetDisplayMessageD1("");
        displayControl.SetDisplayMessageD2("");
        displayControl.SetDisplayMessageD3("");
    }

    int get_int(){
        displayControl.SetDisplayMessageD3("Input:\n");
        int n = 0;
        bool loop = true;
        while(loop){
            auto c = keypadControl.getc();
            if(c != 'A' && c != 'B' && c != 'C' && c != 'D'){
                if(c == '*'){
                    n = 0;
                    displayControl.SetDisplayMessageD3("Input:\n");
                }else if(c == '#'){
                    loop = false;
                    displayControl.SetDisplayMessageD3("");
                }else{
                    n *= 10;
                    n += c - '0';
                    displayControl.SetDisplayInput("Input: ",n,"\n");
                }
            }
        }
        return n;
    }

    void input_number(){
        while (true){
            displayControl.SetDisplayMessageD2("Gun number:\n(0-31)");
            gun.number = get_int();
            if(gun.number >= 0 && gun.number <= 31){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

    void input_tijd(){
        while (true){
            hwlib::cout << "\nGame duration(sec, 0-1023): \n";
            tijd_data.tijd = get_int();
            if(tijd_data.tijd >= 0 && tijd_data.tijd <= 1023){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

    void input_gunpower(){
        while (true){
            displayControl.SetDisplayMessageD2("Gun power:\n(5-15)");
            gun.power = get_int();
            if(gun.power >= 5 && gun.power <= 15){
                break;
            }
            displayControl.SetDisplayMessageD2("Wrong input");
            wrong_timer.set(delay);
            wait(wrong_timer);
        }
    }

public:
    init_game(KeypadControl& keypadControl_, DisplayControl& displayControl_, game_parameters& gameParameters_, start_game& startGame_):
            keypadControl(keypadControl_),
            displayControl(displayControl_),
            gameParameters(gameParameters_),
            startGame(startGame_)
    {}

    /*void setup(){
        get_gun_number();
        if(gun.number == 0){
            get_gamemode();
            input_tijd_countdown();
        }
        get_gun_power();
        get_health();

        hwlib::cout << "gun_number: " << gun.number << "\n";
        hwlib::cout << "tijd: " << tijd_data.tijd << "\n";
        hwlib::cout << "countdown: " << tijd_data.countdown << "\n";
        hwlib::cout << "gun_power: " << gun.power << "\n";
        hwlib::cout << "health: " << gun.health << "\n";
        hwlib::cout << "gamemode " << gamemode << "\n";
    }*/

    void enable_restart_init_game_flag(){
        restart_init_game_flag.set();
    }
};

//run_game
int main( void ){
    //make mutex
    auto m1 = rtos::mutex( "m1" );

    //OLED
    auto scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    auto sda = hwlib::target::pin_oc( hwlib::target::pins::sda );

    auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );

    auto oled = hwlib::glcd_oled( i2c_bus, 0x3c );

    auto w1 = hwlib::part(oled,hwlib::xy( 0, 0 ),hwlib::xy( 128, 21));
    auto w2 = hwlib::part(oled,hwlib::xy( 0, 21 ),hwlib::xy( 128, 43));
    auto w3 = hwlib::part(oled,hwlib::xy( 0, 43 ),hwlib::xy( 128, 64));
    //END_OLED

    //DisplayContol
    DisplayControl displayControl = {oled, w1, w2, w3};

    //Gameparameters
    game_parameters gameParameters;

    //run_game
    run_game runGame;

    //start_game
    start_game startGame = {gameParameters,runGame,displayControl};

    // IR output LED
    auto ir = hwlib::target::d2_36kHz();

    //ir code
    //ir_send s = {ir,m1};
    //message_writing m = {s};
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
    
    //KEYPAD
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
    auto keypad   = hwlib::keypad< 16 >( matrix, "123A456B789C*0#D" );
    //END_KEYPAD

    //KeypadControl
    KeypadControl keypadControl = {keypad};

    //init_game
    init_game initGame = {keypadControl,displayControl,gameParameters,startGame};

    //START CODE
    rtos::run();
}