#ifndef MANAGE_TIME_HPP
#define MANAGE_TIME_HPP

#include "rtos.hpp"

/// @brief Manage_time class
/// @details Class that's a timer
class manage_time : public rtos::task<>{
private:
    /// @brief Enum of the different states
    enum state_t {
        idle,
        counting
    };
    /// @brief Selects the starting state
    state_t state = idle;

    //rtos members
    /// @brief start_manage_time_flag flag for starting the manage_time class.
    rtos::flag start_manage_time_flag = {this, "start_manage_time_flag"};
    /// @brief delay member of 1000 ms (1 second).
    long long int delay = 1000 * rtos::ms;
    /// @brief manage_time_timer timer for counting
    rtos::timer manage_time_timer = {this, "manage_time_timer"};

    ///@brief Main function for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        switch (state) {
            case idle: {
                //entry
                //transistion
                wait(start_manage_time_flag);
                state = counting;
                break;
            }case counting: {
                //entry
                count();
                //transistion
                state = idle;
                break;
            }
        }
    }

    ///@brief Function that subtracts a second from the remaining time till the time is 0.
    void count(){
        while (timer > 0){
            manage_time_timer.set(delay);
            wait(manage_time_timer);
            timer--;
        }
    }

public:
    /// @brief timer member for counting
    int timer = 0;

    /// @brief Set_timer function
    /// @details This function sets the timer member variable as the input.
    void set_timer(const int& input){
        timer = input;
    }

    /// @brief Get_timer function
    /// @details This function gets the timer member variable and returns it.
    int get_time(){
        return timer;
    }

    /// @brief Enable_start_manage_time_flag function
    /// @details This function sets the start_manage_time_flag flag.
    void enable_start_manage_time_flag(){
        start_manage_time_flag.set();
    }

    /// @brief Enable_start_manage_time_flag function
    /// @details This function checks if the timer <= 0;
    bool check_done(){
        return timer <= 0;
    }
};
#endif //MANAGE_TIME_HPP
