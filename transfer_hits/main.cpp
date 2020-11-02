#include "hwlib.hpp"
#include <vector>

class transfer_hits{
private:
	int player_number;
	std::vector<int> hit_vector;

public:
	transfer_hits(int player_number_, std::vector<int> hit_vector_):
	player_number(player_number_),
	hit_vector(hit_vector_)
	{}

	void set_player_number(int & playerNumber){
		hwlib::wait_ms(1);
		hwlib::cout << "Player: "<< player_number;
	}
	void add_hit(std::vector<int>& hit_vector){
		hwlib::wait_ms(1);
		for ( unsigned int i = 0; i < hit_vector.size(); ++i) {
			int hit = hit_vector[i];
			hwlib::cout << " has " <<hit << " hits\n";
		}
	}
};

int main(){
	int playerNumber = 1;
	std::vector<int>hitVector = {14,10,8,11};
	transfer_hits hits = {playerNumber, hitVector};
	hits.set_player_number(playerNumber);
	hits.add_hit(hitVector);
};