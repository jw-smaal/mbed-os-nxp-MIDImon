/** This MIDI class implements various transforms for MIDI messages 
 * @date: 24 Oct 2020 
 * @author: Jan-Willem Smaal <usenet@gispen.org>  
 * @license: APACHE-2.0 
 */
 #include "mbed.h"
 #include "serial-midi.h"

/** TransformMIDI class  
 * protoype 
 */
class TransformMIDI {
public: 
	TransformMIDI(SerialMidi *ptr) {
		ptrSerialMidi = ptr; 
	};

protected: 

private: 
	SerialMidi *ptrSerialMidi; 
};




class SuperARP: public TransformMIDI {

};


