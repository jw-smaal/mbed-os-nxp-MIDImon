/** @file Harmony.cpp
 *
 * Harmony implements Notes, Scales, Modes
 * the intent is to keep this library as clean
 * as possible to allow implementation on hardware
 * platforms such as ARM MBED OS.
 * No exceptions are used as the platform does not
 * support it.
 *
 * TODO: Chords, Inversions, Progressions
 *
 * @author Jan-Willem Smaal <usenet@gispen.org>
 * @date 13/12/2020
 * @copyright APACHE-2.0
 */
#include "Harmony.hpp"


/** Convert string to midi note.
 * e.g. F#, G# Gb A
 * C3 C-2 Ab5 etc...
 */
const uint8_t Note::ToNote(std::string str) {
	uint8_t basenote = 60;  // If no basenote is given use 60 (middle C3)
	
	// TODO: find the last numeral in the string add 2
	// TODO: and then multiply by 12
	// TODO: implement .e.g C#3 ('3')
	// TODO: i.e. (3 + 2)  12 = 60
	
	
	// Uppercase:
	if(str == "C") 	basenote += 0;
	if(str == "C#") basenote += 1;
	if(str == "Db") basenote += 1;
	if(str == "D") 	basenote += 2;
	if(str == "D#") basenote += 3;
	if(str == "Eb") basenote += 3;
	if(str == "E") 	basenote += 4;
	if(str == "F")	basenote += 5;
	if(str == "F#") basenote += 6;
	if(str == "Gb") basenote += 6;
	if(str == "G") 	basenote += 7;
	if(str == "G#") basenote += 8;
	if(str == "Ab") basenote += 8;
	if(str == "A") 	basenote += 9;
	if(str == "A#")	basenote += 10;
	if(str == "Bb")	basenote += 10;
	if(str == "B")	basenote += 11;
	
	// Same with lowercase:
	if(str == "c") 	basenote += 0;
	if(str == "c#") basenote += 1;
	if(str == "db") basenote += 1;
	if(str == "d") 	basenote += 2;
	if(str == "d#") basenote += 3;
	if(str == "eb") basenote += 3;
	if(str == "e") 	basenote += 4;
	if(str == "f")	basenote += 5;
	if(str == "f#") basenote += 6;
	if(str == "gb") basenote += 6;
	if(str == "g") 	basenote += 7;
	if(str == "g#") basenote += 8;
	if(str == "ab") basenote += 8;
	if(str == "a") 	basenote += 9;
	if(str == "a#")	basenote += 10;
	if(str == "bb")	basenote += 10;
	if(str == "b")	basenote += 11;
	
	return basenote;
}

/** Simply convert MIDI notename to a note either using flats or sharps
 */
const std::string Note::ToText(uint8_t midinote,
						 bool flats,
						 bool showoctave)
{
	int octave;
	uint8_t note;
	std::string strng;
	
	// For MIDI note 0 is written down as "C-2" hence the -2 below
	octave = (midinote / 12) - 2;
	note = midinote - ((octave + 2) * 12);
	
	if(flats) {
		switch(note) {
			case 0:
				strng = "C";
				break;
			case 1:
				strng = "Db";
				break;
			case 2:
				strng = "D";
				break;
			case 3:
				strng = "Eb";
				break;
			case 4:
				strng = "E";
				break;
			case 5:
				strng = "F";
				break;
			case 6:
				strng = "Gb";
				break;
			case 7:
				strng = "G";
				break;
			case 8:
				strng = "Ab";
				break;
			case 9:
				strng = "A";
				break;
			case 10:
				strng = "Bb";
				break;
			case 11:
				strng = "B";
				break;
			default:
				strng = "!!";
				break;
		}
	}
	else {
		switch(note) {
			case 0:
				strng = "C";
				break;
			case 1:
				strng = "C#";
				break;
			case 2:
				strng = "D";
				break;
			case 3:
				strng = "D#";
				break;
			case 4:
				strng = "E";
				break;
			case 5:
				strng = "F";
				break;
			case 6:
				strng = "F#";
				break;
			case 7:
				strng = "G";
				break;
			case 8:
				strng = "G#";
				break;
			case 9:
				strng = "A";
				break;
			case 10:
				strng = "A#";
				break;
			case 11:
				strng = "B";
				break;
			default:
				strng = "!!";
				break;
		}
	}
	if (showoctave) {
		strng = strng + std::to_string(octave);
	}
	return strng;
}



/*
 * Constructor of 'Mode'
 * we need to know the Scale (hence the pointer)
 * otherwise we cannot create the
 * mode intervals.
 */
Mode::Mode(Scale *scaleParent,
		   unsigned int modeNumArg,
		   unsigned long numOfNotesArg,
		   std::string modeNameArg) noexcept {
	modeNum = modeNumArg;
	//privName = modeNumArg;
	numOfNotes = numOfNotesArg;
	privName = modeNameArg;
	uint8_t rootNote = scaleParent->rootNote;
	
	// Create the notes for this mode
	for(uint8_t j = 0, k = rootNote; j < numOfNotes; j++) {
		Note note;
		// If the numOfNotes is bigger than the
		// const array we have a out of bounds issue.
		// TODO: change the array to a std:vector
		// TODO: or std:array

		if (j == 0) {	// Root note remains root
			note.number = rootNote;
		}
		else {
			note.number = k; // take the previous note number.
			// Based on the type of scale we lookup the table
			// We need to substract j - 1 otherwise
			// we will be missing the first offset ( j == 0 is covered above)
			switch(scaleParent->typeOfScale) {
				case Scale::TypeOfScale::CHROMATIC:
					note.number = 	note.number + chromatic[j-1];
					//note.number += 	scaleParent->chromatic[j-1];
					break;
				case Scale::TypeOfScale::OCTATONIC:
					note.number = 	note.number +
									octatonic[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::DOMINANT_DIMINISHED:
					note.number = 	note.number +
									dominant_diminished[j-1];
					break;
				case Scale::TypeOfScale::DIMINISHED:
					note.number = 	note.number +
									diminished[j-1];
					break;
				case Scale::TypeOfScale::MAJOR:
					note.number = 	note.number +
									major_s[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::MINOR:
					note.number = 	note.number +
									minor_s[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::MELODIC_MINOR:
					note.number = 	note.number +
									melodic_minor[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::HARMONIC_MINOR:
					note.number = 	note.number +
									harmonic_minor[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::GYPSY:
					note.number = 	note.number +
									gypsy[j-1];
					break;
				case Scale::TypeOfScale::SYMETRICAL:
					note.number =	note.number +
									symetrical[j-1];
					break;
				case Scale::TypeOfScale::ENIGMATIC:
					note.number =	note.number +
									enigmatic[j-1];
					break;
				case Scale::TypeOfScale::ARABIAN:
					note.number =	note.number +
									arabian[j-1];
					break;
				case Scale::TypeOfScale::HUNGARIAN:
					note.number =	note.number +
									hungarian[j-1];
					break;
				case Scale::TypeOfScale::WHOLE_TONE:
					note.number =	note.number +
									whole_tone[j-1];
					break;
				case Scale::TypeOfScale::AUGMENTED:
					note.number =	note.number +
									augmented[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::BLUES_MAJOR:
					note.number =	note.number +
									blues_major[j-1];
					break;
				case Scale::TypeOfScale::BLUES_MINOR:
					note.number =	note.number +
									blues_minor[this->modeNum][j-1];
					break;
				case Scale::TypeOfScale::PENTATONIC:
					note.number =	note.number +
									pentatonic[j-1];
					break;
				case Scale::TypeOfScale::MINOR_PENTATONIC:
					note.number =	note.number +
									minor_pentatonic[j-1];
					break;
					// TODO: IMPLEMENT
				default:
					// Don't to anything.
					break;
			}
			k = note.number; 	// Save just calculated note number in 'k'
		}
		// 'notes' is a vector containing our 'note' objects itself
		this->notes.push_back(note);
	}
	
};


/*
 * Default constructor for Scale
 * create a major scale starting at
 * middle C (rootNote=60)
 */
Scale::Scale() noexcept{
	kindOfScale = Scale::KindOfScale::HEPTATONIC;
	typeOfScale = Scale::TypeOfScale::MAJOR;
	numOfModes = 7; // 7 modes in this scale
	numOfNotes = 7; // Heptatonic = 7 notes.
	rootNote = 60;
	
	//	 Create 7 (i) modes object in the vector
	//	 We give a pointer to ourselves as the mode
	//	 must know what kind of scale it is a mode of.
	for(unsigned int i = 0; i < numOfModes; i++) {
		Mode mode(this, i, numOfNotes, "major scale mode");
		modes.push_back(mode);
	}
};


/*
 * Scale::Scale constructs a scale with modes
 * and notes based on the typeOfScaleArg argument given
 * starting at the rootnote (midi note numbers are used).
 */
Scale::Scale(TypeOfScale typeOfScaleArg,
			 uint8_t rootNoteArg) noexcept {
	// MIDI notes cannot be higher than 128
	// instead of throwing an exception we just truncate it to the
	// higest value.
	if(rootNoteArg > 128 ) {
		rootNoteArg = 127;
	}
	rootNote = rootNoteArg;
	typeOfScale = typeOfScaleArg;
	
	switch(typeOfScaleArg) {
		case Scale::TypeOfScale::CHROMATIC:
			scaleText = "CHROMATIC";
			kindOfScale = Scale::KindOfScale::CHROMATIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 12;
			break;
			// ---==== 8 NOTE scale's ====----
		case Scale::TypeOfScale::OCTATONIC:
			scaleText ="OCTATONIC";
			kindOfScale = Scale::KindOfScale::OCTATONIC;
			numOfModes = 2; // only 2 modes in this scale
			numOfNotes = 8;
			break;
		case Scale::TypeOfScale::DOMINANT_DIMINISHED:
			scaleText ="DOMINANT_DIMINISHED";
			kindOfScale = Scale::KindOfScale::OCTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 8;
			break;
		case Scale::TypeOfScale::DIMINISHED:
			scaleText ="DIMINISHED";
			kindOfScale = Scale::KindOfScale::OCTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 8;
			break;
			// ---==== 7 NOTE scale's ====----
		case Scale::TypeOfScale::MAJOR:
			scaleText ="MAJOR";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 7; // 7 modes in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::MINOR:
			scaleText ="MINOR";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 7; // 7 modes in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::MELODIC_MINOR:
			scaleText ="MELODIC_MINOR";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 7; // 7 modes in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::HARMONIC_MINOR:
			scaleText ="HARMONIC_MINOR";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 7; // 7 modes in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::GYPSY:
			scaleText ="GYPSY";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::SYMETRICAL:
			scaleText ="SYMETRICAL";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::ENIGMATIC:
			scaleText ="ENIGMATIC";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::ARABIAN:
			scaleText ="ARABIAN";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
		case Scale::TypeOfScale::HUNGARIAN:
			scaleText ="HUNGARIAN";
			kindOfScale = Scale::KindOfScale::HEPTATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 7; // Heptatonic = 7 notes.
			break;
			// ---==== 6 NOTE scale's ====----
		case Scale::TypeOfScale::WHOLE_TONE:
			scaleText ="WHOLE_TONE";
			kindOfScale = Scale::KindOfScale::HEXATONIC;
			numOfModes = 1; // only 1 mode in this scale
			numOfNotes = 6; // Hexatonic 6 notes.
			break;
		case Scale::TypeOfScale::AUGMENTED:
			scaleText ="AUGMENTED";
			kindOfScale = Scale::KindOfScale::HEXATONIC;
			numOfModes = 2;
			numOfNotes = 6; // Hexatonic 6 notes.
			break;
		case Scale::TypeOfScale::BLUES_MAJOR:
			scaleText ="BLUES MAJOR";
			kindOfScale = Scale::KindOfScale::HEXATONIC;
			numOfModes = 1;
			numOfNotes = 6; // Hexatonic 6 notes.
			break;
		case Scale::TypeOfScale::BLUES_MINOR:
			scaleText ="BLUES_MINOR";
			kindOfScale = Scale::KindOfScale::HEXATONIC;
			numOfModes = 6;
			numOfNotes = 6; // Hexatonic 6 notes.
			break;
			// ---==== 5 NOTE scale's ====----
		case Scale::TypeOfScale::PENTATONIC:
			scaleText ="PENTATONIC";
			kindOfScale = Scale::KindOfScale::PENTATONIC;
			numOfModes = 1;
			numOfNotes = 5; // Hexatonic 6 notes.
			break;
		case Scale::TypeOfScale::MINOR_PENTATONIC:
			scaleText ="MINOR_PENTATONIC";
			kindOfScale = Scale::KindOfScale::PENTATONIC;
			numOfModes = 1;
			numOfNotes = 5; // Hexatonic 6 notes.
			break;
		// TODO: implement other type of scales.
		default:
			numOfModes = 0;
			numOfNotes = 0;
			break;
	}
	
	//	 Create (i) modes object in the vector
	//	 We give a pointer to ourselves 'this' as the mode
	//	 must know what kind of scale it is a mode of.
	for(unsigned int i = 0; i < numOfModes; i++) {
		Mode mode(this, i, numOfNotes, "mode" + std::to_string(i + 1));
		modes.push_back(mode);
	}
	
};



/** Chord constructor
 * for now it's not scale aware yet (work in progress)
 */
Chord::Chord(Chord::Type chordTypeArg,
	  uint8_t bassnoteArg,
	  uint8_t rootnoteArg ) noexcept {

	chordType = chordTypeArg;
	bassnote = bassnoteArg;
	rootnote = rootnoteArg;

	// Find out the type of chord we are asked to become.
	switch(chordTypeArg) {
		case Type::MAJOR:
			privText = "Major";
			shortPrivText = "";
			chordVect = majorVect;
			break;
		case Type::MAJOR_6:
			privText = "Major 6'th";
			shortPrivText = "6";
			chordVect = major_6_Vect;
			break;
		case Type::MAJOR_7:
			privText = "Major 7'th";
			shortPrivText = "maj7";
			chordVect = major_7_Vect;
			break;
		case Type::MAJOR_9:
			privText = "Major 9'th";
			shortPrivText = "maj9";
			chordVect = major_9_Vect;
			break;
		case Type::MAJOR_7_SHARP11:
			privText = "Major 7'th #11";
			shortPrivText = "maj7#11";
			chordVect = major_7_sharp11_Vect;
			break;
		case Type::MINOR:
			privText = "Minor";
			shortPrivText = "m";
			chordVect = minorVect;
			break;
		case Type::MINOR_6:
			privText = "Minor 6'th";
			shortPrivText = "m6";
			chordVect = minor_6_Vect;
			break;
		case Type::MINOR_FLAT6:
			privText = "Minor b6";
			shortPrivText = "mb6";
			chordVect = minor_flat6_Vect;
			break;
		case Type::MINOR_7:
			privText = "Minor 7'th";
			shortPrivText = "m7";
			chordVect = minor_7_Vect;
			break;
		case Type::MINOR_9:
			privText = "Minor 9'th";
			shortPrivText = "m9";
			chordVect = minor_9_Vect;
			break;
		case Type::MINOR_MAJOR_7:
			privText = "Minor major 7'th";
			shortPrivText = "minMaj7";
			chordVect = minor_major_7_Vect;
			break;
		case Type::DOMINANT_7:
			privText = "Dominant 7'th";
			shortPrivText = "7";
			chordVect = dominant_7_Vect;
			break;
		case Type::DOMINANT_7_FLAT5:
			privText = "Dominant 7'th b5";
			shortPrivText = "7b5";
			chordVect = dominant_flat5_Vect;
			break;
		case Type::DOMINANT_7_SHARP5:
			privText = "Dominant 7'th #5";
			shortPrivText = "7#5";
			chordVect = dominant_sharp5_Vect;
			break;
		case Type::DOMINANT_7_SUS4:
			privText = "Dominant 7'th with suspended 4'th";
			shortPrivText = "7sus4";
			chordVect = dominant_7_sus4;
			break;
		case Type::DOMINANT_7_FLAT9:
			privText = "Dominant 7'th b9";
			shortPrivText = "7b9";
			chordVect = dominant_7_flat9;
			break;
		case Type::DOMINANT_7_SHARP9:
			privText = "Dominant 7'th #9";
			shortPrivText = "7#9";
			chordVect = dominant_7_sharp9;
			break;
		case Type::DOMINANT_7_SHARP5_FLAT9:
			privText = "Dominant 7'th #5 b9";
			shortPrivText = "7#5b9";
			chordVect = dominant_7_sharp5_flat9;
			break;
		case Type::DOMINANT_7_SHARP11:
			privText = "Dominant 7'th #11";
			shortPrivText = "7#11";
			chordVect = dominant_7_sharp11;
			break;
		case Type::DOMINANT_7_ADD9_FLAT5:
			privText = "Dominant 7'th add9 b5";
			shortPrivText = "7add9b5";
			chordVect = dominant_7_add9_flat5;
			break;
		case Type::DOMINANT_7_ADD9_SHARP11:
			privText = "Dominant 7'th add9 #11";
			shortPrivText = "7add9#11";
			chordVect = dominant_7_add9_sharp11;
			break;
		case Type::DOMINANT_7_ADD13:
			privText = "Dominant 7'th add13";
			shortPrivText = "7add13";
			chordVect = dominant_7_add13;
			break;
		case Type::DOMINANT_7_SHARP9_FLAT13:
			privText = "Dominant 7'th #9 b13";
			shortPrivText = "7#9b13";
			chordVect = dominant_7_sharp9_flat13;
			break;
		case Type::DOMINANT_7_SHARP11_ADD13:
			privText = "Dominant 7'th #11 add13";
			shortPrivText = "7#11add13";
			chordVect = dominant_7_sharp11_add13;
			break;
		case Type::DIMINISHED:
			privText = "Diminished";
			shortPrivText = "dim";
			chordVect = diminished;
			break;
		case Type::DIMINISHED_7:
			privText = "Diminished 7";
			shortPrivText = "dim7";
			chordVect = diminished_7;
			break;
		case Type::MINOR_7_FLAT5:
			privText = "Minor 7 b5";
			//shortPrivText = "m7b5";
			shortPrivText = "0";
			chordVect = minor_7_flat5;
			break;
		case Type::AUGMENTED:
			privText = "Augmented";
			shortPrivText = "+";
			chordVect = augmented;
			break;
		case Type::SUS4:
			privText = "Suspended 4'th";
			shortPrivText = "sus4";
			chordVect = sus4;
			break;
		case Type::SUS2:
			privText = "Suspended 2'nd";
			shortPrivText = "sus2";
			chordVect = sus2;
			break;
		//default:	//	Does not exist when using enum Classes!
			// break;
	}
	
	// Build the chord now that we know the recipe.
	Note note(rootnote);
	notes.push_back(note);
	uint8_t previousnote = rootnote;
	for (auto vect: chordVect) {
		// c style cast required on the Enum
		previousnote = previousnote + (uint8_t)vect;
		Note noteU(previousnote);
		notes.push_back(noteU);
	}
	// Default voicing is as in the constructor.
	voicing = notes;
};


/**
 * Chord voicings
 */
void Chord::setVoicing(VoicingType voicingTypeArg,
					   bool dropRootDown){
	
	// Drop the root note an octave when asked for.
	if (notes.front().number >= 12 && dropRootDown ) {
		voicing.front().number = notes.front().number - 12;
	}
	
	// Based on the requested voicing shuffle around the notes.
	switch (voicingTypeArg) {
		case VoicingType::DROP1:
			// Take the last note and drop an octave.
			if (voicing.back().number >= 12 ) {
				voicing.back().number = voicing.back().number - 12;
			}
			break;
		case VoicingType::DROP2:
			// Take the last note and drop an octave.
			if (voicing.back().number >= 12 ) {
				voicing.back().number = voicing.back().number - 12;
			}
			// Take the second to last note and drop an octave.
			Note tmpNote = voicing.back();
			voicing.pop_back(); // Remove the last one (put it back later).
			if (voicing.back().number >= 12 ) {
				voicing.back().number = voicing.back().number - 12;
			}
			// Not exactly in the right order anymore but that's ok.
			voicing.push_back(tmpNote);
			break;
	}
};
	
/* EOF */
