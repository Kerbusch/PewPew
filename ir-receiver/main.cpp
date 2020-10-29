/*#include "hwlib.hpp"
#include <bitset>

int main( void ) {
	auto tsop_signal = hwlib::target::pin_in(hwlib::target::pins::d8);
	hwlib::wait_us(1);
	uint16_t message = 0x00;

	while(true){
		tsop_signal.refresh();
		if(!tsop_signal.read() == 1) {
			for (int i = 0; i < 16; ++i) {
				hwlib::wait_us(1200);
				tsop_signal.refresh();
				if (!tsop_signal.read() == 1) {
					message = message << 1;
					message |= 0x01;
				} else{
					message = message << 1;
				}
				hwlib::wait_us(1200);
			}
			hwlib::cout << "message: "<<message<<'\n';
			break;
		}
	}
}*/

#include "hwlib.hpp"
#include "rtos.hpp"

class ir_receive : public rtos::task<>{
private:
	enum state_t{
		Idle,
		Receive_bits
	};
	hwlib::target::pin_in signal;
	rtos::timer send_timer_long = {this, "send_timer_long"};
	long long int delay_long = 1600 * rtos::us;
	rtos::timer send_timer_short = {this, "send_timer_short"};
	long long int delay_short = 800 * rtos::us;
	rtos::timer send_timer_1us = {this, "send_timer_1us"};
	long long int start_delay_1us = 1 * rtos::us;

	messagetranslation& messagetranslation;

	void receive_bits(){
		while(true){
			tsop_signal.refresh();
			if(!tsop_signal.read() == 1) {
				for (int i = 0; i < 16; ++i) {
					hwlib::wait_us(1200);
					tsop_signal.refresh();
					if (!tsop_signal.read() == 1) {
						message = message << 1;
						message |= 0x01;
					} else{
						message = message << 1;
					}
					hwlib::wait_us(1200);
				}
				//hwlib::cout << "message: "<<message<<'\n';
				return message;
				break;
			}
		}
	}

	void main(){
		receive_bits();
	}

public:
	ir_receive(hwlib::target::pin_in& signal_):
	signal(signal_)
	{}

	void add(uint16_t & message){
		received_bits_channel.write(message);
	}
};

