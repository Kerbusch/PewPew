#include "hwlib.hpp"
#include "rtos.hpp"
#include <vector>

//includes
#include "struct.hpp"
#include "display.hpp"
#include "gameparameters.hpp"
#include "ir_send.hpp"
#include "ir_receive.hpp"
#include "start_game.hpp"
#include "speaker.hpp"
#include "keypad.hpp"
#include "run_game.hpp"
#include "trigger.hpp"
#include "init_game.hpp"
#include "manage_time.hpp"
#include "transfer.hpp"

//run_game
int main( void ) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    //wait for terminal
    hwlib::wait_ms(1000);


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

    //speaker gpio
    auto speaker_pin = hwlib::target::pin_out( hwlib::target::pins::d7 );

    //speaker control code
    SpeakerControl speakerControl = {speaker_pin};

    // IR output LED
    auto ir = hwlib::target::d2_36kHz();

    //ir code
    ir_send s = {ir};
    message_writing m = {s};

    //Gameparameters
    game_parameters gameParameters;

    //manage_time
    manage_time manageTime = {};

    //transfer_hits
    transfer_hits transferHits = {};

    //run_game
    run_game runGame = {displayControl, speakerControl, m, manageTime,transferHits};

    //start_game
    start_game startGame = {gameParameters, runGame, displayControl};

    //ir receiver:
    auto tsop_signal = hwlib::target::pin_in( hwlib::target::pins::d8 );

    //message_translation
    message_translation messageTranslation = {runGame, startGame};

    //ir_receiver
    ir_receiver irReceiver = {tsop_signal, messageTranslation};

    //trigger gpio
    auto led_yellow = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto sw0 = hwlib::target::pin_in( hwlib::target::pins::d3 );

    //trigger control code
    Trigger trigger1 = {led_yellow,sw0,speakerControl,runGame};
    
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

    //keypad_listener
    keypad_listener keypad_listener = {keypad};

    //init_game
    init_game initGame = {keypad_listener,displayControl,gameParameters,startGame};

    //START CODE
    rtos::run();
}