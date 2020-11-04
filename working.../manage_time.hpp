#ifndef MANAGE_TIME_HPP
#define MANAGE_TIME_HPP

#include "rtos.hpp"

/// @brief Manage_time class
/// @details Class that's a timer
class manage_time : public rtos::task<>{
public:
    /// @brief timer member for counting
    int timer = 0;
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
    /// @brief stop_manage_time_flag flag for stopping the manage_time class.
    rtos::flag stop_manage_time_flag = {this, "stop_manage_time_flag"};
    /// @brief delay member of 1000 ms (1 second).
    long long int delay = 60000 * rtos::ms;
    /// @brief manage_time_timer timer for counting
    rtos::timer manage_time_timer = {this, "manage_time_timer"};

    ///@brief Main function for the rtos::task<>.
    /// @details State switcher for the different states
    void main(){
        while(true) {
            switch (state) {
                case idle: {
                    //hwlib::cout << "idle-manage_time\n";
                    //entry
                    //transistion
                    wait(start_manage_time_flag);
                    state = counting;
                    break;
                }
                case counting: {
                    //hwlib::cout << timer << "-manage_time\n";
                    //entry
                    manage_time_timer.set(delay);
                    auto evt = wait(manage_time_timer + stop_manage_time_flag);
                    if(evt == stop_manage_time_flag){
                        timer = 0;
                        state = idle;
                    }else{ //manage_time_timer
                        if (timer <= 0) {
                            hwlib::cout << "timer<=0-manage_time\n";
                            state = idle;
                        }else{
                            timer -= 1;
                        }
                    }

                    break;
                }
            }
        }
    }

public:
    manage_time():
            task(50,"manage_time")
    {}

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

    /// @brief Enable_stop_manage_time_flag function
    /// @details This function sets the stop_manage_time_flag flag.
    void enable_stop_manage_time_flag(){
        stop_manage_time_flag.set();
    }

    /// @brief Enable_start_manage_time_flag function
    /// @details This function checks if the timer <= 0;
    bool check_done(){
        return timer <= 0;
    }
};
#endif //MANAGE_TIME_HPP
