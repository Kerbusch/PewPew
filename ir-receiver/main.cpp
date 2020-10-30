#include "hwlib.hpp"
#include "rtos.hpp"

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
			messageTranslation.write(message);
			message = 0x00;
		}
	}

public:
	ir_receiver(hwlib::target::pin_in& signal_, message_translation& messageTranslation_):
	    signal(signal_),
        messageTranslation(messageTranslation_)
	{}
};