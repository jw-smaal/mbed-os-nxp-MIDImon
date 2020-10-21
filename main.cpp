/** 
  * MIDI parsing Test with the NXP FRDM-K64F board
  * Jan-Willem Smaal <usenet@gispen.org>  
 */
#include "MK64F12.h"
#include "ThisThread.h"
#include "mbed.h"
#include "SerialBase.h"
#include <cstdint>

#include "serial-usart-midi.h"


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
	//static uint8_t midi_beat; 
	uint16_t ppm24; 
	long long bpm; 
	
	//DigitalOut led2(LED2);
	DigitalOut stat1(PTC3);


	if (msg == 0xf8) { 
		if(midi_f8_counter == 23) {
			stat1 = true; 
			t.stop(); 
	 		bpm = 60000000 /  duration_cast<milliseconds>(t.elapsed_time()).count();
			 printf("%llu ms %llu us, BPM %llu\n", 
			 		duration_cast<milliseconds>(t.elapsed_time()).count(),
					duration_cast<microseconds>(t.elapsed_time()).count(),
					bpm
				 );
			midi_f8_counter = 0;
			t.reset(); 
			t.start();
		}
		else if(midi_f8_counter == 11 ) {
			stat1 = false; 
			midi_f8_counter++; 
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



/**
 * Main run loop never ends.   
 */
int main()
{
	int i, j; 
	// Application buffer to receive the data
    char buf[32] = {0};
	uint16_t b2in_value; 
	uint16_t b3in_value; 
	uint16_t tmp; 

	// I prefer the USB console port of the mbed to be 115200 
	BufferedSerial pc(USBTX, USBRX);
	pc.set_baud(115200);

    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED3);
	DigitalOut stat1(PTC3);
	DigitalOut stat2(PTC2);
	
	// Analog input 
	AnalogIn b2in(PTB2, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	AnalogIn b3in(PTB3, MBED_CONF_TARGET_DEFAULT_ADC_VREF);

	// Serial Midi outputs 
	SerialMidi serialMidi(
		&midi_note_on_handler,
		&realtime_handler,
		&midi_note_off_handler,
		&midi_control_change_handler,
		&midi_pitchwheel_handler
	); 

	
	// Test all the notes 
	for(i = 0; i < 128; i++) { 
		serialMidi.NoteON(SerialMidi::CH1, i, 100); 
		ThisThread::sleep_for(40);
		serialMidi.NoteOFF(SerialMidi::CH1, i, 10); 
	}

	// Test lower half the modulation wheel steps  
	for(i = 128; i < 500; i++) { 
		// explicit case required as there are two implementations 
		// of this one 
		serialMidi.ModWheel(SerialMidi::CH1, (uint16_t)i); 
	//	serialMidi.PitchWheel(CH1, (uint16_t)i);
	}
	
	// Test Pitch wheel 
	for(i = 0x1F00; i < 0x2100; i++) { 
		// explicit case required as there are two implementations 
		// of this one 
	//	serialMidi.ModWheel(CH1, (uint16_t)i); 
		serialMidi.PitchWheel(SerialMidi::CH1, (uint16_t)i);
		ThisThread::sleep_for(10);
	}
	
	// Test all the modulation wheel steps (MSB only)  
	for(i = 0; i < 128; i++) { 
		// explicit case required as there are two implementations 
		// of this one 
		serialMidi.ModWheel(SerialMidi::CH1, (uint8_t)i); 
	}
	serialMidi.ModWheel(SerialMidi::CH1, (uint8_t)0); 


    while (1) {
			led = !led;
			stat2 = !led;
			
			printf("SerialMidi state: %s\n", serialMidi.Text());
			serialMidi.ReceiveParser();

			
			tmp = b2in.read_u16(); 
			if (tmp == b2in_value) { 
				// Do nothing 
			}
			else { 
				b2in_value = tmp; 
				tmp = MIDI_DATA & (tmp >> 8); 
				serialMidi.ControlChange(SerialMidi::CH1, 
										 SerialMidi::CTL_MSB_MODWHEEL, 
										 tmp);
			}

			tmp = b3in.read_u16(); 
			if (tmp == b3in_value) {
				// Do nothing 
			}
			else {
				b3in_value = tmp; 
				tmp = MIDI_DATA & (tmp >> 8); 
				serialMidi.ControlChange(SerialMidi::CH1, 
										 SerialMidi::CTL_MSB_EXPRESSION, 
										 tmp);
			}

	}  // End of while(1) loop 
	
	return 0;
} // End of main(1) loop 
