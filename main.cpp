/** 
  * MIDI parsing Test with the NXP FRDM-K64F board
  * Jan-Willem Smaal <usenet@gispen.org>  
 */
#include "SerialBase.h"
#include "mbed.h"
#include "serial-usart-midi.h"




/**
 * Delegates required for SerialMIDI callbacks 
 */ 

void midi_note_on_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_on_handler(%2X, %2X)\n", note, velocity);
	return; 
}


void realtime_handler(uint8_t msg) {
	return; 
}


void midi_note_off_handler(uint8_t note, uint8_t velocity) {
	return; 
}


void midi_control_change_handler(uint8_t controller, uint8_t value) {
	return; 
}


int main()
{
	// Application buffer to receive the data
    char buf[32] = {0};

    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
	DigitalOut stat1(PTC3);
	DigitalOut stat2(PTC2);
	
	// Analog input 
	AnalogIn a0in(A0, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	AnalogIn a1in(A1, MBED_CONF_TARGET_DEFAULT_ADC_VREF);

	// Serial Midi outputs 
	SerialMidi serialMidi(
		&midi_note_on_handler,
		&realtime_handler,
		&midi_note_off_handler,
		&midi_control_change_handler
	);

    while (1) {

/*
		if(a0in.read_u16() == 0xffff) 
			ThisThread::sleep_for(2000ms);
		else if(a0in.read_u16() > 0x3000)
			ThisThread::sleep_for(80ms);
		else if(a0in.read_u16() > 0x8000)
			ThisThread::sleep_for(50ms);
		else 
			ThisThread::sleep_for(2ms);
*/	

			led = !led;
			stat2 = !led;
			serialMidi.ReceiveParser();
			//ThisThread::sleep_for(80ms);

#if 0
		// This call is blocking.    
     	 if (uint32_t num = serial_port.read(buf, sizeof(buf))) {
			// Toggle the LED.
			led = !led;
			stat2 = !led;

            // Echo the input back to the terminal.
            serial_port.write(buf, num);
			for(uint32_t i = 0; i < num; i++){
				printf("%2X ", buf[i]);
			}
			printf("\n");
		}
#endif 

	}  // End of while(1) loop 
	
	return 0;
} // End of main(1) loop 
