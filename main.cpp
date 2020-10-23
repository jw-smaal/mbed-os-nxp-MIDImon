/** 
  * MIDI parsing Test with the NXP FRDM-K64F board
  * Jan-Willem Smaal <usenet@gispen.org>  
 */
#include "MK64F12.h"
#include "Semaphore.h"
#include "ThisThread.h"
#include "mbed.h"
#include "SerialBase.h"
#include <cstdint>
#include <cstdio>

// Musical scale implementation by Jan-Willem Smaal <usenet@gispen.org> 
#include "midi-scales.h"

// Serial USART MIDI implementation by Jan-Willem Smaal <usenet@gispen.org> 
#include "serial-midi.h"

// Driver for the Magneto and Gyro 
#include "FXOS8700CQ.h"




/////////////////////////////////////////////////////////////////
//  MIDI callback functions  
//  TODO: need to find a more C++ way of doing this with 
//        delegates. 
/////////////////////////////////////////////////////////////////
void midi_note_on_handler(uint8_t note, uint8_t velocity) {
	printf("midi_note_on_handler(%2X, %2X)\n", note, velocity);
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
			 printf("%llu ms %llu us, BPM %llu\n", 
			 		duration_cast<milliseconds>(t.elapsed_time()).count(),
					duration_cast<microseconds>(t.elapsed_time()).count(),
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
/////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////
// Threads 
/////////////////////////////////////////////////////////////////
Thread thread_led1;
Thread thread_led2;
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


/*  
 * Need to rewrite this so that it becomes a singleton class
 * Reason is that the serial port is used and can only be used in 
 * one class 
 */  
SerialMidi serialMidiGlob(
		&midi_note_on_handler,
		&realtime_handler,
		&midi_note_off_handler,
		&midi_control_change_handler,
		&midi_pitchwheel_handler
); 


void midi_tx_thread() 
{
	uint16_t b2in_value; 
	uint16_t b3in_value;
	uint16_t prev_tmp; 
	uint16_t tmp; 
	int16_t tmpsig; 
	uint8_t tmp8_t;


	// Analog inputs 
	AnalogIn b2in(PTB2, MBED_CONF_TARGET_DEFAULT_ADC_VREF);
	AnalogIn b3in(PTB3, MBED_CONF_TARGET_DEFAULT_ADC_VREF);


	// I prefer the USB console port of the mbed to be 115200
	// This way printf's don't slow down the execution of the thread. 
	// too much.  
	BufferedSerial pc(USBTX, USBRX);
	pc.set_baud(115200);

	// Built in magneto and gyro chip of the NXP FRDM board 
	FXOS8700CQ magneto(PTE25, PTE24, FXOS8700CQ_SLAVE_ADDR1);
	magneto.enable();
	printf("Sensor: %2X", magneto.get_whoami());
	magneto.get_accel_scale();
//	printf("Magneto %d %d %d\n", 
//		magneto.getMagnetX(), 
//		magneto.getMagnetY(), 
//		magneto.getMagnetZ() );

	while(true ) {
		/*
		* MIDI TX processing 
		* This should really run in a seperate thread (transmission)  
		* as MIDI is full duplex.  
		*/
		tmp = b2in.read_u16(); 
		//printf("Read b2in %u\n",tmp); 
		if (tmp == b2in_value) { 
			// Do nothing 
		}
		else { 
			b2in_value = tmp; 
			// We are shifting right 9 times as midi value can only 
			// be 7 bits -->  16-9 = 7)
			tmp = MIDI_DATA & (tmp >> 9);		 
			serialMidiGlob.ControlChange(SerialMidi::CH1, 
									SerialMidi::CTL_MSB_MODWHEEL, 
									tmp);
		}


		tmp = b3in.read_u16(); 
		//printf("Read b3in %u\n",tmp); 
		if (tmp == b3in_value) {
			// Do nothing 
		}
		else {
			b3in_value = tmp; 
			tmp = MIDI_DATA & (tmp >> 9); 
			serialMidiGlob.ControlChange(SerialMidi::CH1, 
									SerialMidi::CTL_MSB_EXPRESSION, 
									tmp);
		}
		

		magneto.get_data();
		// 16 bit signed
		//printf("32768 -32768 "); 
#if 1
		printf("%04d %04d %04d ", 
			magneto.getMagnetX(),  
			magneto.getMagnetY(), 
			magneto.getMagnetZ());
		// 14 bit signed
#endif 
#if 1 	
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
 

	// I prefer the USB console port of the mbed to be 115200
	// This way printf's don't slow down the execution of the thread. 
	// too much.  
	BufferedSerial pc(USBTX, USBRX);
	pc.set_baud(115200);

    // Initialise the digital pin STAT2 as an output
    DigitalOut stat2(PTC2);

#if 0
	// Test all the notes 
	for(i = 0; i < 128; i++) { 
		serialMidiGlob.NoteON(SerialMidi::CH1, i, 100); 
		ThisThread::sleep_for(40ms);
		serialMidiGlob.NoteOFF(SerialMidi::CH1, i, 10); 
	}

	// Test lower half the modulation wheel steps  
	for(i = 128; i < 500; i++) { 
		// explicit cast required as there are two implementations 
		// of this one based on the bitsize of the int  
		serialMidiGlob.ModWheel(SerialMidi::CH1, (uint16_t)i); 
	}
	
	// Test Pitch wheel 
	for(i = 0x1F00; i < 0x2100; i++) { 
		// explicit cast required as there are two implementations 
		// of this one 
		serialMidiGlob.PitchWheel(SerialMidi::CH1, (uint16_t)i);
		ThisThread::sleep_for(10ms);
	}
	
	// Test all the modulation wheel steps (MSB only)  
	for(i = 0; i < 128; i++) { 
		// explicit case required as there are two implementations 
		// of this one 
		serialMidiGlob.ModWheel(SerialMidi::CH1, (uint8_t)i); 
	}
	serialMidiGlob.ModWheel(SerialMidi::CH1, (uint8_t)0); 
#endif 
	// All tests complete start the threads. 

	thread_led1.start(led1_thread);
	thread_midi_tx.start(midi_tx_thread);

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