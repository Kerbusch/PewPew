#ifndef IR_HPP
#define IR_HPP

#include "struct.hpp"
#include "run_game.hpp"
#include <vector>



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
