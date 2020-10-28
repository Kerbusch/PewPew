//
// Created by daan on 28-10-20.
//

#ifndef IR_HPP
#define IR_HPP

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

class message_writing{
private:
    ir_send& ir_Send;

public:
    message_writing(ir_send& ir_Send_):
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
        send_data(n,2,true); //2 moet TIJD_COUNTDOWN worden
        for(int i = 0; i < 4; i++){
            hwlib::wait_ms(30);
            send_data(n,data[i],false);
        }
    }
};

class runner: public rtos::task<> {
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
};

#endif //IR_HPP
