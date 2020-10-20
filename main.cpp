/** 
  * MIDI parsing Test with the NXP FRDM-K64F board
  * Jan-Willem Smaal <usenet@gispen.org>  
 */
#include "SerialBase.h"
#include "mbed.h"
#include "serial-usart-midi.h"
#include <cstdint>


/*
 * Timer is global. 
 */
using namespace std::chrono;
Timer t;


/**
 * Delegates required for SerialMIDI callbacks 
 */ 

void midi_note_on_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_on_handler(%2X, %2X)\n", note, velocity);
	return; 
}


/** 
 * Called with realtime messages 
 *
 * To calculate the BPM (with a prescaler of 64) 
 * bpm = FOSC/64 * (60/(cycles*24))
 * bpm = 625e3/cycles 
 */ 
void realtime_handler(uint8_t msg)
{
	static uint8_t midi_f8_counter; 
	static uint8_t midi_beat; 
	uint16_t ppm24; 
	long long bpm; 

	if (msg == 0xf8) {
		if(midi_f8_counter == 23) {
			t.stop(); 
	 		bpm = 60000000 /  duration_cast<milliseconds>(t.elapsed_time()).count();
			 printf("%llu milliseconds, BPM %llu\n", 
			 		duration_cast<milliseconds>(t.elapsed_time()).count(),
					bpm
				 );
			midi_f8_counter = 0;
			t.reset(); 
			t.start();
		}
		else {
			midi_f8_counter++;
		}
	}
	else if(msg == 0xfe){
		// we ignore active-sensee
		return; 
	}
	else {
			printf("RT msg:%x", msg);
			midi_f8_counter =0; 
			midi_beat = 0; 
	}
	return;
}




void midi_note_off_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_off_handler(%2X, %2X)\n", note, velocity);
	return; 
}


void midi_control_change_handler(uint8_t controller, uint8_t value) {
	printf("midi_control_change_handler(%2X, %2X)\n", controller, value);
	return; 
}


/**
 * Not all MIDI controllers use the LSB on the pitch bend. 
 * mid position == 0x2000 because this is a 14 bit value.  
 * we use a signed int16_t and simply substract to be 
 * able to present a human readable value  
 */
void midi_pitchwheel_handler(uint8_t valueLSB, uint8_t valueMSB)  {
	int16_t pitch = (valueLSB + (valueMSB<<7)) - 0x2000; 
	printf("midi_pitch_wheel_handler%2X %2X (%d)\n", valueLSB, valueMSB, pitch);
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
		&midi_control_change_handler,
		&midi_pitchwheel_handler
	);

    while (1) {
			led = !led;
			stat2 = !led;
			serialMidi.ReceiveParser();
	}  // End of while(1) loop 
	
	return 0;
} // End of main(1) loop 
