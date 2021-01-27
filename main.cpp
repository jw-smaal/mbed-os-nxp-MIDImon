/** 
  * MIDI parsing Test with the NXP FRDM-K64F board
  * Jan-Willem Smaal <usenet@gispen.org>  
 */
#include "MK64F12.h" 
//#include "MK66F18.h"
#include "Semaphore.h"
#include "ThisThread.h"
#include "mbed.h"
#include "SerialBase.h"
#include <cstdint>
#include <cstdio>
#include <iostream>

// MIDI transforms 
#include "TransformMIDI.h"

// Musical scale implementation by Jan-Willem Smaal <usenet@gispen.org> 
//#include "midi-scales.h"
#include "Harmony.hpp"

// Serial USART MIDI implementation by Jan-Willem Smaal <usenet@gispen.org> 
#include "serial-midi.h"
/*  
 * TODO: Need to rewrite this below so that it becomes a singleton. 
 * on second thought maybe not because there can be multiple MIDI inputs.
 * therefore multiple MIDI parsers. 
 * in any case needs some work to port from C to C++ properly.   
 */  
SerialMidi serialMidiGlob(
		&midi_note_on_handler,
		&realtime_handler,
		&midi_note_off_handler,
		&midi_control_change_handler,
		&midi_pitchwheel_handler
); 

Chord::Type chordTypeGlob = Chord::Type::MAJOR;  

// Driver for the Magneto and Gyro 
#include "FXOS8700CQ.h"

/** Minimal LCD lib by Jan-Willem Smaal <usenet@gispen.org>
 * need to do something about it being accessable from 
 * all threads. 
 */ 
#include "i2c-lcd.h"

/** Musical Harmony lib by Jan-Willem Smaal <usenet@gispen.org> 
 *
 */
 #include "Harmony.hpp" 

/////////////////////////////////////////////////////////////////
//  MIDI callback functions  
//  TODO: need to find a more C++ way of doing this with 
//        delegates. 
/////////////////////////////////////////////////////////////////
void midi_note_on_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_on_handler(%d, %d)\n", note, velocity);

	Scale scl(Scale::TypeOfScale::HARMONIC_MINOR, note); 
	uint8_t i, j; 
	uint8_t midi_note = note; 

	// Velocity decided on the chord quality. 
	if (velocity > 127 ) return; 
 	if (velocity >= 0 && velocity < 16 ) 	chordTypeGlob = Chord::Type::MAJOR;	
	if (velocity >= 16 && velocity < 32)	chordTypeGlob = Chord::Type::MINOR;
	if (velocity >= 32 && velocity < 48) 	chordTypeGlob = Chord::Type::AUGMENTED;
	if (velocity >= 48 && velocity < 64) 	chordTypeGlob = Chord::Type::DIMINISHED_7;
	if (velocity>= 64 && velocity < 80) 	chordTypeGlob = Chord::Type::MINOR_7_FLAT5;
	if (velocity >= 80 && velocity < 96) 	chordTypeGlob = Chord::Type::DOMINANT_7_ADD9_SHARP11;
	if (velocity >= 96 && velocity < 112) 	chordTypeGlob = Chord::Type::DOMINANT_7_ADD9_FLAT5;
	if (velocity >= 112 && velocity < 128) 	chordTypeGlob = Chord::Type::SUS4;
	

#if 0	// Go through all the modes and notes of this scale 
	for(auto mode: scl.modes) {
		std::cout << mode.Name() << "\t"; 
		for (auto note: mode.notes) {
			std::cout << "\t"<< note.Name();
			serialMidiGlob.NoteON(SerialMidi::CH1, note.number, velocity);
			ThisThread::sleep_for(80ms);
			serialMidiGlob.NoteOFF(SerialMidi::CH1, note.number, 100);
			ThisThread::sleep_for(80ms);
		}
		std::cout << std::endl;
	}
#endif 


#if 1	// Play a Chord based on the root note given.  
	//Chord chrd(Chord::Type::DIMINISHED_7, note, note);
	// Chord played is based on the Modulation wheel. 
	Chord chrd(chordTypeGlob, note, note);
	for(auto note: chrd.voicing) {
		serialMidiGlob.NoteON(SerialMidi::CH1, note.number, velocity);
	}
	ThisThread::sleep_for(400ms);
	// Make sure we also send a note off.... 
	for(auto note: chrd.voicing) {
		serialMidiGlob.NoteOFF(SerialMidi::CH1, note.number, 100);
	}
#endif 


	return; 
}

/*
 * Timer is declared global. 
 * as it needs be accessed from the MIDI thread. 
 */
using namespace std::chrono;
Timer t;

/** 
 * Called with realtime messages
 * do not use blocking calls!  
 */ 
void realtime_handler(uint8_t msg)
{
	static uint8_t midi_f8_counter; 
	//static uint8_t midi_beat; 
	uint16_t ppm24; 
	long long bpm; 
	
	if (msg == 0xf8) { 
		if(midi_f8_counter == 23) {
			//stat1 = true; 
			t.stop(); 
	 		bpm = 60000000 /  duration_cast<milliseconds>(t.elapsed_time()).count();
			//sem_led.acquire(); 
			 printf("%llu %llu\n", 
			 		duration_cast<milliseconds>(t.elapsed_time()).count(),
				//	duration_cast<microseconds>(t.elapsed_time()).count(),
					bpm
				 );
			//sem_led.release(); 
			midi_f8_counter = 0;
			t.reset(); 
			t.start();
		}
		else if(midi_f8_counter == 11 ) {
			//stat1 = false; 
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
			printf("RT msg:%x\n", msg);
			midi_f8_counter =0; 
	}
	return;
}




void midi_note_off_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_off_handler(%d, %d)\n", note, velocity);
	return; 
}


void midi_control_change_handler(uint8_t controller, uint8_t value) {
	printf("midi_control_change_handler(%2X, %2X)\n", controller, value);

	if (value > 127 ) return; 
 	if (value >= 0 && value < 16 ) 	chordTypeGlob = Chord::Type::MAJOR;	
	if (value >= 16 && value < 32)	chordTypeGlob = Chord::Type::MINOR;
	if (value >= 32 && value < 48) 	chordTypeGlob = Chord::Type::AUGMENTED;
	if (value >= 48 && value < 64) 	chordTypeGlob = Chord::Type::DIMINISHED_7;
	if (value >= 64 && value < 80) 	chordTypeGlob = Chord::Type::MINOR_7_FLAT5;
	if (value >= 80 && value < 96) 	chordTypeGlob = Chord::Type::DOMINANT_7_ADD9_SHARP11;
	if (value >= 96 && value < 112) chordTypeGlob = Chord::Type::DOMINANT_7_ADD9_FLAT5;
	if (value >= 112 && value < 128) chordTypeGlob = Chord::Type::SUS4;
	
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
/////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////
// Threads 
/////////////////////////////////////////////////////////////////
Thread thread_midi_tx;


void led1_thread()
{
    while (true) {
       	ThisThread::sleep_for(1ms);
    }
}

void led2_thread()
{
	DigitalOut led2(LED2);
	led2 =1; 
	
    while (true) {
		//sem_led.acquire();
        printf("2");
		//pc.write("2",1);
		//sem_led.release();
        ThisThread::sleep_for(10ms);
    }
}




void midi_tx_thread() 
{
	uint16_t b2in_value; 
	uint8_t	prev_b2in_value;
	
	uint16_t b3in_value;
	uint8_t  prev_b3in_value; 

	uint16_t ribbon_value;
	uint8_t prev_ribbon_value;
	
	uint16_t prev_tmp; 
	uint16_t tmp; 
	int16_t tmpsig; 
	uint8_t tmp8_t;
	

	// Analog inputs on K66 board  
	//AnalogIn b2in(PTB7, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	//AnalogIn b3in(PTB6, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	//AnalogIn ribbon(PTB5, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	
	// K64 board 
	AnalogIn b2in(PTB2, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	AnalogIn b3in(PTB3, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	AnalogIn ribbon(PTB10, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	

	// I prefer the USB console port of the mbed to be 115200
	// This way printf's don't slow down the execution of the thread. 
	// too much.  
	BufferedSerial pc(USBTX, USBRX);
	pc.set_baud(115200);

	// Built in magneto and gyro chip of the NXP FRDM board 
	//FXOS8700CQ magneto(PTE25, PTE24, FXOS8700CQ_SLAVE_ADDR1);
	//magneto.enable();


	//Scale scl(Scale::ScaleKinds::HARMONIC_MINOR, 0); 
	Scale  scl(Scale::TypeOfScale::HARMONIC_MINOR, 0); 

	uint8_t i, j; 
	uint8_t midi_note = 60; 
	
	// Trying out the new type of scale. 
	//Scale scl2(Scale2::TypeOfScale::GYPSY, 60);
	Scale scl2(Scale::TypeOfScale::GYPSY, 60);

	for(auto mode: scl2.modes) {
		for (auto note: mode.notes) {
			//std::cout << "\t"<< note.Name();
			serialMidiGlob.NoteON(SerialMidi::CH1, note.number, 100);
			ThisThread::sleep_for(200ms);
			serialMidiGlob.NoteOFF(SerialMidi::CH1, note.number, 100);
			ThisThread::sleep_for(100ms);
		}
		//std::cout << std::endl;
	}

#if 0 
	// Play the root note 
	serialMidiGlob.NoteON(SerialMidi::CH1, midi_note, 100);
	ThisThread::sleep_for(200ms);
	serialMidiGlob.NoteOFF(SerialMidi::CH1, midi_note, 100);
	ThisThread::sleep_for(100ms);
	// Iterate through the rest of the scale 

	for(i = 0; i < scl.notes; i++) {
		printf("midi_note: %d | ", midi_note);
		midi_note = midi_note + scl.ptrToScale[i]; 
		serialMidiGlob.NoteON(SerialMidi::CH1, midi_note, 100);
		ThisThread::sleep_for(200ms);
		serialMidiGlob.NoteOFF(SerialMidi::CH1, midi_note, 100);
		ThisThread::sleep_for(100ms);
	}
#endif 


	/** Tx thread should never end. 
	 */
	while(true ) {
		/*
		* MIDI TX processing 
		* run in a seperate thread (transmission) as MIDI is full duplex.  
		*/

		// A0 potmeter on MIDI shield 
		tmp = b2in.read_u16(); 
		// We shift right 9 because MIDI only uses 7 bits (16-9 = 7)
		tmp = (uint8_t)(MIDI_DATA & (tmp >>9)); 
		if (prev_b2in_value != tmp) {
			serialMidiGlob.ControlChange(SerialMidi::CH3, 
									SerialMidi::CTL_MSB_BREATH, 
									(uint8_t)tmp);
			prev_b2in_value = tmp; 
		}


		// A1 potmeter on MIDI shield 
		tmp = b3in.read_u16(); 
		// We shift right 9 because MIDI only uses 7 bits (16-9 = 7)
		tmp = (uint8_t)(MIDI_DATA & (tmp >>9)); 
		if (prev_b3in_value != tmp) {
			serialMidiGlob.ControlChange(SerialMidi::CH3, 
									SerialMidi::CTL_MSB_EXPRESSION, 
									(uint8_t)tmp);
			prev_b3in_value = tmp; 
		}

		// Ribbon 
		tmp = ribbon.read_u16(); 
		// We shift right 9 because MIDI only uses 7 bits (16-9 = 7)
		tmp = (uint8_t)(MIDI_DATA & (tmp >>9)); 
		if (prev_ribbon_value != tmp) {
			serialMidiGlob.ControlChange(SerialMidi::CH3, 
									SerialMidi::CTL_MSB_MODWHEEL, 
									(uint8_t)tmp);
			prev_ribbon_value = tmp; 
		}


		// Magneto sensor   
#if MAGNETO_SENSOR 
		magneto.get_data();
		// 16 bit signed
		//printf("32768 -32768 "); 
#if 0 
		printf("%04d %04d %04d ", 
			magneto.getMagnetX(),  
			magneto.getMagnetY(), 
			magneto.getMagnetZ());
		// 14 bit signed
#endif 
#if 0 	
		printf("%04d %04d %04d \r\n", 
			magneto.getAccelX(),
			magneto.getAccelY(),
			magneto.getAccelZ());
#endif 
		tmpsig = magneto.getMagnetZ(); 
		tmp = MIDI_DATA & ((tmpsig + 8192) >>4); 

		// Only send out if there is a change in value 
		if (prev_tmp != tmp ) {
			serialMidiGlob.ControlChange(SerialMidi::CH2, 
									SerialMidi::CTL_MSB_MODWHEEL, 
									tmp);
			prev_tmp = tmp; 
		} 
#endif // MAGNETO_SENSOR 

		// Limit the amount of MIDI messages to something 
		// a human will not notice the intervals. 
		ThisThread::sleep_for(30ms); 
	}	
}



/**
 * Main run loop never ends.   
 * this is also a special thread in the RTOS...  
 */
int main()
{
	int i, j; 
	// Application buffer to receive the data
    char buf[32] = {0};
	uint16_t b2in_value; 
	uint16_t b3in_value; 
	uint16_t tmp; 
 	I2cLcd i2clcd;

	i2clcd.move_cursor_line1();
	i2clcd.write(RETURN_HOME);
	i2clcd.putchar('J');
	i2clcd.putchar('-');
	i2clcd.putchar('W');
	//i2clcd.printf("Hello\n"); 


	// I prefer the USB console port of the mbed to be 115200
	// This way printf's don't slow down the execution of the thread. 
	// too much.  
	BufferedSerial pc(USBTX, USBRX);
	pc.set_baud(115200);
	std::cout << "MIDImon K64 by Jan-Willem Smaal <usenet@gispen.org>";
	std::cout << std::endl;

    // Initialise the digital pin STAT2 as an output
    DigitalOut stat2(PTC2);

	// All tests complete start the threads. 
//	thread_led1.start(led1_thread);
	//thread_midi_tx.start(midi_tx_thread);

    while (true) {
		// Toggle green stat2 LED.  
		stat2 = !stat2; 
			
		/* 
		 * MIDI RX processing 
		 */
		serialMidiGlob.ReceiveParser();

	}  // End of while(1) loop 
	
	return 0;
} // End of main(1) loop 

/* EOF */ 