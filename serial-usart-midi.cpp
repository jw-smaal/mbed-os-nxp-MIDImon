/**
 * serial-usart-midi.c
 * avrMIDIout
 *
 *  Created by Jan-Willem Smaal on 21/12/14.
 *  Copyright (c) 2014 Jan-Willem Smaal. All rights reserved.
 */
#include "serial-usart-midi.h"


/**
 * Constructor 
 * Inits the serial USART with MIDI clock speed and 
 * registers delegates for the callbacks of the parser. 
 */
SerialMidi::SerialMidi ( 
	void (*note_on_handler_ptr)(uint8_t note, uint8_t velocity),
	void (*realtime_handler_ptr)(uint8_t msg),
	void (*note_off_handler_ptr)(uint8_t note, uint8_t velocity),
	void (*control_change_handler_ptr)(uint8_t controller, uint8_t value), 
	void (*midi_pitchwheel_ptr)(uint8_t valueLSB, uint8_t valueMSB)
) 
: serial_port(PTC17, PTC16, MIDI_BAUD_RATE)   
{
    // Assign delegate's
    midi_note_on_delegate = note_on_handler_ptr;
    realtime_handler_delegate = realtime_handler_ptr;
    midi_note_off_delegate = note_off_handler_ptr;
    midi_control_change_delegate = control_change_handler_ptr;
	midi_pitchwheel_delegate = midi_pitchwheel_ptr; 
    
    // init the serial-usart system done via Constructor just to be on the safe
	// side we set it explicitly below 
    serial_port.set_baud(MIDI_BAUD_RATE);
    serial_port.set_format(
        /* bits */ 		8,
        /* parity */ 	BufferedSerial::None,
        /* stop bit */ 	1
    );


	global_running_status = 0; 
	global_3rd_byte_flag = 0;
	global_midi_c2 = 0;
	global_midi_c3 = 0;
	global_midi_state = RESET;
}


void SerialMidi::NoteON(uint8_t channel, uint8_t key, uint8_t velocity)
{
//    SerialTransmit(C_NOTE_ON | channel);
//    SerialTransmit(key);
//    SerialTransmit(velocity);
}


void SerialMidi::NoteOFF(uint8_t channel, uint8_t key, uint8_t velocity)
{
//    SerialTransmit(C_NOTE_OFF | channel);
//    SerialTransmit(key);
//    SerialTransmit(velocity);
}


void SerialMidi::ControlChange(uint8_t channel, uint8_t controller, uint8_t val)
{
//    SerialTransmit(C_CONTROL_CHANGE | channel);
//    SerialTransmit(controller);
//    SerialTransmit(val);
}


void SerialMidi::ChannelAfterTouch(uint8_t channel, uint8_t val)
{
//    SerialTransmit(C_CHANNEL_AFTERTOUCH | channel);
 //   SerialTransmit(val);
}


/**
 * Modulation Wheel both LSB and MSB
 * range: 0 --> 16383
 */
void SerialMidi::ModWheel(uint8_t channel, uint16_t val)
{
    ControlChange(channel, CTL_MSB_MODWHEEL,  ~(CHANNEL_VOICE_MASK) & (val>>7));
    ControlChange(channel, CTL_LSB_MODWHEEL,  ~(CHANNEL_VOICE_MASK) & val);
}


/**
 * PitchWheel is always with 14 bit value.
 *       LOW   MIDDLE   HIGH
 * range: 0 --> 8192  --> 16383
 */
void SerialMidi::PitchWheel(uint8_t channel, uint16_t val)
{
//    SerialTransmit(C_PITCH_WHEEL | channel);
    // Value is 14 bits so need to shift 7
//   SerialTransmit(val & ~(CHANNEL_VOICE_MASK));        // LSB
//    SerialTransmit((val>>7) & ~(CHANNEL_VOICE_MASK));   // MSB
}



void SerialMidi::TimingClock(void)
{
//    SerialTransmit(RT_TIMING_CLOCK);
}


void SerialMidi::Start(void)
{
//    SerialTransmit(RT_START);
}


void SerialMidi::Continue(void)
{
//    SerialTransmit(RT_CONTINUE);
}


void SerialMidi::Stop(void)
{
//    SerialTransmit(RT_STOP);
}


void SerialMidi::Active_Sensing(void)
{
//    SerialTransmit(RT_ACTIVE_SENSING);
}


void SerialMidi::Reset(void)
{
//    SerialTransmit(RT_RESET);
}


void SerialMidi::Text()
{

}


/**
 * MIDI Parser
 */
void SerialMidi::ReceiveParser(void)
{
    uint8_t c;

    // Read one byte from the circular FIFO input buffer
    // This buffer is filled by the ISR routine on receipt of
    // data on the port.
    // c = SerialReceive();

	if( serial_port.read(&c, 1) == 0) {
		return;
	}
	//printf("%2X ", c);
	// MIDI through (kind of with some processing delay)
	serial_port.write(&c,1);
	
    
    // Check if bit7 = 1
    if ( c & CHANNEL_VOICE_MASK ) {
	//	printf("Ch:%2X ", c);

        // if (! (c & SYSTEM_REALTIME_MASK)) {
		// is it a real-time message?  0xF8 up to 0xFF
        if (c >= 0xF8 ) {
			realtime_handler_delegate(c);
            return;
        }
        else {
            global_running_status = c;
            global_3rd_byte_flag = 0;
            // Is this a tune request
            if(c == SYSTEM_TUNE_REQUEST) {
                global_midi_c2 = c; // Store in FIFO.
                // TODO: Process something.
                return;
            }
            // Do nothing
            // Ignore for now.
            return;
        }
    }
    else {  // Bit 7 == 0   (data)
	//	printf("D:%2X ", c);

	    if(global_3rd_byte_flag == 1) {
            global_3rd_byte_flag = 0;
            global_midi_c3 = c;

			// We don't care about the input channel (OMNI) for now.
            global_running_status &= 0xF0;
            if(global_running_status == C_NOTE_ON){
	            midi_note_on_delegate(global_midi_c2, global_midi_c3);
                return;
            }
            else if(global_running_status == C_NOTE_OFF) {
                midi_note_off_delegate(global_midi_c2, global_midi_c3);
                return;
            }
			else if(global_running_status == C_PITCH_WHEEL) {
				return; 
			}
			else if(global_running_status == C_PROGRAM_CHANGE) {
				return; 
			}
			else if(global_running_status ==  C_POLYPHONIC_AFTERTOUCH) {
				return; 
			}
			else if(global_running_status ==  C_CHANNEL_AFTERTOUCH) {
				return; 
			}
            else if(global_running_status == C_CONTROL_CHANGE) {
                midi_control_change_delegate(global_midi_c2, global_midi_c3);
                return;
            }
			//else {
			//	return; 
			//	}
        }
        else {
            if(global_running_status == 0) {
                // Ignore data Byte if running status is  0
                return;
            }
            else {
                if (global_running_status < 0xC0) { // All 2 byte commands
                    global_3rd_byte_flag = 1;
                    global_midi_c2 = c;
                    // At this stage we have only 1 byte out of 2.
                    return;
                }
                else if (global_running_status < 0xE0) {    // All 1 byte commands
                    global_midi_c2 = c;
                    // TODO: !! Process callback/delegate for two bytes command.
                    return;
                }
                else if ( global_running_status < 0xF0){
                    global_3rd_byte_flag = 1;
                    global_midi_c2 = c;
                }
				//!!
                else if ( global_running_status >= 0xF0) {
                    if (global_running_status == 0xF2) {
                        global_running_status = 0;
                        global_3rd_byte_flag = 1;
                        global_midi_c2 = c;
                        return;
                    }
                    else if (global_running_status >= 0xF0 ){
                        if(global_running_status == 0xF3 ||
                           global_running_status == 0xF3 ) {
                            global_running_status = 0;
                            global_midi_c2 = c;
                            // TODO: !! Process callback for two bytes command.
                            return;
                        }
                        else {
                            // Ignore status
                            global_running_status = 0;
                            return;
                        }
                    }
                }
            }  
        }  // global_3rd_byte_flag
    } // end of data bit 7 == 0

} // End of SerialMidiReceiveParser


// EOF
