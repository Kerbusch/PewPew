// simple IR signal detector

#include "hwlib.hpp"

int main( void ){
   
   namespace target = hwlib::target;
   
   auto tsop_signal = target::pin_in( target::pins::d8 );
   auto tsop_gnd    = target::pin_out( target::pins::d9 );
   auto tsop_vdd    = target::pin_out( target::pins::d10 );
   tsop_gnd.write( 0 );
   tsop_vdd.write( 1 );
   tsop_gnd.flush();
   tsop_vdd.flush();
   
   auto led         = target::pin_out( target::pins::d11 );
   
   auto const active = 100'000;
   auto last_signal = hwlib::now_us() - active;

   auto ir = hwlib::target::d2_36kHz();
   auto red = hwlib::target::pin_out( hwlib::target::pins::d22 );
   auto sw = hwlib::target::pin_in( hwlib::target::pins::d23 );



   
   for(;;){
      tsop_signal.refresh();
      if( tsop_signal.read() == 0 ){
         last_signal = hwlib::now_us();
      }
      led.write( ( last_signal + active) > hwlib::now_us() );
      led.flush();

      hwlib::wait_ms( 1 ); 
      
      sw.refresh();
      ir.write( ! sw.read() );
      red.write( ! sw.read() );
      ir.flush();
      red.flush();
      
      hwlib::wait_ms( 1 );
      ir.write( 0 );
      ir.flush();
   }
}

