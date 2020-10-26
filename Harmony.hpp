/** @file Harmony.hpp
 *
 * Harmony implements Notes, Scales, Modes, Chords
 * the intent is to keep this library as clean
 * as possible to allow implementation on hardware
 * platforms such as ARM MBED OS.
 * TODO: Chords, Inversions, Progressions
 *
 * @author Jan-Willem Smaal <usenet@gispen.org>
 * @date 13/12/2020
 * @copyright APACHE-2.0
 */
#ifndef Harmony_hpp
#define Harmony_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <array>
//#include <deque>


class Note {
public:
	Note() noexcept {
		number = 60;
	};
	Note(uint8_t midinote) noexcept {
		number = midinote;
	};
    uint8_t number;	// MIDI note 7 bits 0 --> 127
    bool flats;
	/**
	 * Various ways of displaying the note.
	 */
	const std::string Text() {
		return Note::ToText(number, flats, false);
	};
    const std::string Name() {
		return Note::ToText(number, flats, true);
    };
	const std::string FlatsName() {
		return Note::ToText(number, true, true);
	};
	const std::string SharpsName() {
		return Note::ToText(number, false, true);
	};
	/** Simply convert MIDI notenumber to a note (either using flats or sharps
	 */
	static const std::string ToText(uint8_t midinote,
									bool flats,
									bool showoctave);
	/** Convert string to MIDI note number
	 */
	static const uint8_t ToNote(std::string str);
private:
};


class Mode {
public:
	Mode(class Scale *scaleParent,
		 unsigned int modeNumArg,
		 unsigned long numOfNotesArg,
		 std::string modeNameArg) noexcept;
    unsigned int modeNum;
	unsigned long numOfNotes;
    const std::string Name(){
        return privName;
    }
	const std::string Text() {
		return "Mode::" + privName;
	}
	
    std::vector<Note> notes;
    unsigned long NumOfNotes() {
        return notes.size();
    };
private:
    std::string privName;
private:
	/*
	 * Scales are listed below as arrays of
	 * uint8_t indicating the either a 1/2 step as 1
	 * or a whole step as 2.
	 * minor 3'rd as 3
	 */
	// -----------------------------------------------------------------------
	enum interVal {
		H = 1,
		W = 2,
		WH = 3,
		WW = 4
	};

	/*
	 * CHROMATIC Scale 12 note
	 */
	const uint8_t  chromatic[12]  = {
		H,H,H,H,H,H,H,H,H,H,H,H,
	};
	
	/*
	 * OCTATONIC 8 notes (of course)
	 */
	const uint8_t  octatonic[2][8]  = {
		{H,W,H,W,H,W,H,W},
		{W,H,W,H,W,H,W,H}
	};
	
	/*
	 * Dominant Diminished (Dom13, b9,#9, b5)   8 note scale
	 * same as "first mode of OCTATONIC" see above
	 */
	const uint8_t  dominant_diminished[8]  = {
		H,W,H,W,H,W,H,W
	};
	
	/*
	 * Diminished (Dim7, Maj/b9)  8 note scale
	 * same as "second mode of OCTATONIC" see above
	 */
	const uint8_t  diminished[8]  = {
		W,H,W,H,W,H,W,H
	};
	
	
	/*
	 * HEPTATONIC scales used by the
	 * majority of western music
	 */
	
	/*
	 * MAJOR Scale modes  7 notes
	 */
	/*
	 * For ATMEL AVR implementations.
	 * Be aware when putting these things in PROGMEM
	 * we need a MACRO to access them e.g. like below
	 * uint8_t (*scale)[7] = pgm_read_ptr(&major[0]);
	 */
	const uint8_t major_s[7][7]  = {
		{W,W,H,W,W,W,H}, // IONIAN   	: Happy
		{W,H,W,W,W,H,W}, // DORIAN		: Jazzy,
		{H,W,W,W,H,W,W}, // PHRYGIAN	: Exotic, latin
		{W,W,W,H,W,W,H}, // LYDIAN		: Hopefull
		{W,W,H,W,W,H,W}, // MIXOLYDIAN	: Positive
		{W,H,W,W,H,W,W}, // AEOLIAN		: Sad
		{H,W,W,H,W,W,W}  // LOCRIAN		:
	};
	
	/*
	 * MINOR Scale modes  7 notes
	 */
	const uint8_t  minor_s[7][7]  = {
		{W,H,W,W,H,W,W}, // AEOLIAN		: Sad
		{H,W,W,H,W,W,W}, // LOCRIAN
		{W,W,H,W,W,W,H}, // IONIAN		: Happy
		{W,H,W,W,W,H,W}, // DORIAN		: Jazzy,
		{H,W,W,W,H,W,W}, // PHRYGIAN
		{W,W,W,H,W,W,H}, // LYDIAN
		{W,W,H,W,W,H,W}  // MIXOLYDIAN
	};
	
	/*
	 * MELODIC MINOR Scale modes  7 notes
	 */
	const uint8_t  melodic_minor[7][7]  = {
		{W,H,W,W,W,W,H}, // Melodic minor     (minor major7)
		{H,W,W,W,W,H,W}, // DORIAN bW         (minor7 sus4 b9)
		{W,W,W,W,H,W,H}, // LYDIAN augmented  (major7 #4 #5)
		{W,W,W,H,W,H,W}, // MIXOLYDIAN #HH    (dominant7 b5)
		{W,W,H,W,H,W,W}, // MIXOLYDIAN b6     (dominant7 b6)
		{W,H,W,H,W,W,W}, // LOCRIAN natural 9 (minor9 b6)
		{H,W,H,W,W,W,W}  // Altered Dominant  (dominant7, #9, b5, #5)
	};
	
	/*
	 * HARMONIC MINOR Scale modes  7 notes
	 */
	const uint8_t  harmonic_minor[7][7]  = {
		{W,H,W,W,H,WH,H}, // Harmonic minor    (minor major7)
		{H,W,W,H,WH,H,W}, // LOCRIAN Nat.6     (minor7 b5)
		{W,W,H,WH,H,W,H}, // IONIAN Augmented  (major7 sus4, #5)
		{W,H,WH,H,W,H,W}, // DORIAN #HH        (minor7 #HH)
		{H,WH,H,W,H,W,W}, // PHRYGIAN major    (dominant7 sus4, b9, #5
		{WH,H,W,H,W,W,H}, // LYDIAN #9         (major7 #9,#HH)
		{H,W,H,W,W,H,WH} // ALTERED DOM bb7   (dim7)
	};
	
	/*
	 * Gypsy scale
	 */
	const uint8_t  gypsy[7]   = 	{
		W,H,WH,H,H,WH,H
	};
	
	
	/*
	 * Symetrical scale
	 */
	const uint8_t  symetrical[7]  = {
		H,W,W,WH,H,H,W
	};
	
	/*
	 * Enigmatic scale
	 */
	const uint8_t  enigmatic[7]  = {
		H,WH,W,W,W,H,H
	};
	
	/*
	 * Arabian scale
	 */
	const uint8_t  arabian[7]  = {
		W,W,H,H,W,W,W
	};
	
	/*
	 * Hungarian scale
	 */
	const uint8_t  hungarian[7]  = {
		WH,H,W,H,W,H,W
	};
	
	/*
	 * Whole tone (Dom7 #5, b6)   6 note scale
	 */
	const uint8_t  whole_tone[6]  = {
		W,W,W,W,W,W
	};
	//  uint8_t *hexatonic  = whole_tone;
	
	
	/*
	 * Augmented (Aug)   6 note scale
	 * (two modes? how does one call this second one then)
	 */
	const uint8_t  augmented[2][6]  = {
		{WH,H,WH,H,WH,H},
		{H,WH,H,WH,H,WH}	//	 Augmented inverse ?
	};
	
	/*
	 * Blues major  6 note scale
	 */
	const uint8_t  blues_major[6]  = {
		W,H,H,WH,W,WH
	};
	
	/*
	 * Blues minor  6 note scale
	 * not sure if these are called "modes"
	 */
	const uint8_t  blues_minor[6][6]  = {
		{WH,W,H,H,WH,W},
		{W,H,H,WH,W,WH},      // Same as blues major scale
		{H,H,WH,W,WH,W},
		{H,WH,W,WH,W,H},
		{WH,W,WH,W,H,H},
		{W,WH,W,H,H,WH}
	};
	
	/*
	 * Major Pentatonic  5 note scale
	 */
	const uint8_t  pentatonic[5]  = {
		W,W,WH,W,WH
	};
	
	/*
	 * Minor Pentatonic  5 note scale
	 */
	const uint8_t  minor_pentatonic[5]  = {
		WH,W,W,WH,W
	};
	// -----------------------------------------------------------------------
private:
	/*
	 * Scales definition in intervals.
	 * TODO: work in progress
	 * TODO: getting rid of the c style arrays.
	 */
	// -----------------------------------------------------------------------
	enum class Iv: unsigned int { // Interval
		H  = 1,     // Half step
		W  = 2,     // Whole step
		WH = 3,     // Whole + Half
		WW = 4      // Whole + Whole
	};
	/*
	 * MAJOR Scale modes  7 notes
	 */
	const std::vector<std::array<enum Iv, 7>> majorScaleVect = { {
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H}, // IONIAN
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W}, // DORIAN
		{Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, // PHRYGIAN
		{Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H}, // LYDIAN
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W}, // MIXOLYDIAN
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, // AEOLIAN
		{Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W}  // LOCRIAN
	} };
	
	/*
	 * MINOR Scale modes  7 notes
	 */
	const std::vector<std::array<enum Iv, 7>> minorScaleVect  = { {
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, 	// AEOLIAN
		{Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W}, 	// LOCRIAN
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H}, 	// IONIAN
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W}, 	// DORIAN
		{Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, 	// PHRYGIAN
		{Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H},	// LYDIAN
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W}	// MIXOLYDIAN
	} };
	
	
};


class Scale {
public:
	enum class KindOfScale {	// How many notes.
		CHROMATIC,		// 12
		PENTATONIC,		// 5
		HEXATONIC,		// 6
		HEPTATONIC,		// 7
		OCTATONIC,		// 8
	};
	enum KindOfScale kindOfScale;
	
	enum class TypeOfScale {
		CHROMATIC,
		OCTATONIC,
		DOMINANT_DIMINISHED,
		DIMINISHED,
		MAJOR,
		MINOR,
		MELODIC_MINOR,
		HARMONIC_MINOR,
		GYPSY,
		SYMETRICAL,
		ENIGMATIC,
		ARABIAN,
		HUNGARIAN,
		WHOLE_TONE,
		AUGMENTED,
		BLUES_MAJOR,
		BLUES_MINOR,
		PENTATONIC,
		MINOR_PENTATONIC
	};
	enum TypeOfScale typeOfScale;
	
	// This is just to facilitate some iterations
	const std::vector<TypeOfScale> allScaleKinds = {
		TypeOfScale::CHROMATIC,
		TypeOfScale::OCTATONIC,
		TypeOfScale::DOMINANT_DIMINISHED,
		TypeOfScale::DIMINISHED,
		TypeOfScale::MAJOR,
		TypeOfScale::MINOR,
		TypeOfScale::MELODIC_MINOR,
		TypeOfScale::HARMONIC_MINOR,
		TypeOfScale::GYPSY,
		TypeOfScale::SYMETRICAL,
		TypeOfScale::ENIGMATIC,
		TypeOfScale::ARABIAN,
		TypeOfScale::HUNGARIAN,
		TypeOfScale::WHOLE_TONE,
		TypeOfScale::AUGMENTED,
		TypeOfScale::BLUES_MAJOR,
		TypeOfScale::BLUES_MINOR,
		TypeOfScale::PENTATONIC,
		TypeOfScale::MINOR_PENTATONIC
	};

public:
	// Constructors
	Scale() noexcept;
	Scale(TypeOfScale typeOfScaleArg,
		  uint8_t rootNoteArg) noexcept;

    unsigned int rootNote;
    unsigned int numOfNotes;
	std::vector<Mode> modes;
	unsigned int numOfModes;
	
    const std::string Text() {
		return "Scale::TypeOfScale::" + scaleText;
    }
    enum class Iv { // Interval
        H  = 1,     // Half step
        W  = 2,     // Whole step
        WH = 3,     // Whole + Half
        WW = 4      // Whole + Whole
    };
private:
	std::string scaleText;
};


#if 0 
/*
 * TODO: work in progress
 * TODO: see if we can rid of the c style arrays in the
 * TODO: scale definitions.
 */
class NewScale {
public:
	NewScale() noexcept;
	//~MajorScale() noexcept;
	//  std::vector<Mode> modes;
	//  unsigned int numOfModes;
	//  unsigned int numOfNotes;
private:
	/*
	 * MAJOR Scale modes  7 notes
	 */
	enum interVal {	// Private member scope
		H = 1,
		W = 2,
		WH = 3,
		WW = 4
	};
	const uint8_t major_s[7][7]  = {
		{W,W,H,W,W,W,H}, // IONIAN
		{W,H,W,W,W,H,W}, // DORIAN
		{H,W,W,W,H,W,W}, // PHRYGIAN
		{W,W,W,H,W,W,H}, // LYDIAN
		{W,W,H,W,W,H,W}, // MIXOLYDIAN
		{W,H,W,W,H,W,W}, // AEOLIAN
		{H,W,W,H,W,W,W}  // LOCRIAN
	};
	enum class Iv: unsigned int { // Interval
		H  = 1,     // Half step
		W  = 2,     // Whole step
		WH = 3,     // Whole + Half
		WW = 4      // Whole + Whole
	};
	const std::vector<std::array<enum Iv, 7>> majorScaleVect = { {
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H}, // IONIAN
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W}, // DORIAN
		{Iv::H, Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, // PHRYGIAN
		{Iv::W, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H}, // LYDIAN
		{Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W}, // MIXOLYDIAN
		{Iv::W, Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W}, // AEOLIAN
		{Iv::H, Iv::W, Iv::W, Iv::H, Iv::W, Iv::W, Iv::W}  // LOCRIAN
	} };
	
	
	// Just playing around below with ADT's.
	// Best way to describe a list of bits?
	std::deque<bool> myBitstr = {
		1,0,1,1,0,1,1,1,
		1,0,0,0,0,1,1,0,
		1,0,0,0,0,1
	};
	std::deque<bool> myBitstr2 = {
		true, false, true, true, false, true, true, true
	};
	std::array<std::string, 4> arr = {
		{"the", "quick", "brown", "fox"}
	};
	std::array<std::array<int, 3>, 3> twodim = {
		{
			{5, 8, 2},
			{8, 3, 1},
			{5, 3, 9}
		}
	};
	std::array<std::array<int, 3>, 2> twodim2 = {
		{ // 2
			{5, 8, 2},   // 3
			{8, 3, 1}
		}
	};
	
};
#endif 

/** Implementation of Chords
 *
 * TODO: The difficulty is that some chords do not make sense in a certain
 * TODO: scale and that the notes varies as wel...
 * TODO: chords act in various degrees of the scale too.
 *
 * TODO: Music is not a hard science that will always follow a certain
 * TODO: algorithm.  However we can at least try to caputure some of the
 * TODO: basics that will be able to implemented in multiple scales.
 */
class Chord {
public:
	uint8_t rootnote;
	uint8_t bassnote;
	enum class Type {
		MAJOR,
		MAJOR_6,
		MAJOR_7,
		MAJOR_9,
		MAJOR_7_SHARP11,
		MINOR,
		MINOR_6,
		MINOR_FLAT6,
		MINOR_7,
		MINOR_9,
		MINOR_MAJOR_7,
		DOMINANT_7,
		DOMINANT_7_FLAT5,
		DOMINANT_7_SHARP5,
		DOMINANT_7_SUS4,
		DOMINANT_7_FLAT9,
		DOMINANT_7_SHARP9,
		DOMINANT_7_SHARP5_FLAT9,
		DOMINANT_7_SHARP11,
		DOMINANT_7_ADD9_FLAT5,
		DOMINANT_7_ADD9_SHARP11,
		DOMINANT_7_ADD13,
		DOMINANT_7_SHARP9_FLAT13,
		DOMINANT_7_SHARP11_ADD13,
		DIMINISHED,			// Double check!
		DIMINISHED_7,		// Double check!
		MINOR_7_FLAT5,
		// TODO: implement !!!
		AUGMENTED,
		SUS4,
		SUS2
	};
	Chord::Type chordType;
	
	const std::vector<Chord::Type> allChordTypes =  {
		Type::MAJOR,
		Type::MAJOR_6,
		Type::MAJOR_7,
		Type::MAJOR_9,
		Type::MAJOR_7_SHARP11,
		Type::MINOR,
		Type::MINOR_6,
		Type::MINOR_FLAT6,
		Type::MINOR_7,
		Type::MINOR_9,
		Type::MINOR_MAJOR_7,
		Type::DOMINANT_7,
		Type::DOMINANT_7_FLAT5,
		Type::DOMINANT_7_SHARP5,
		Type::DOMINANT_7_SUS4,
		Type::DOMINANT_7_FLAT9,
		Type::DOMINANT_7_SHARP9,
		Type::DOMINANT_7_SHARP5_FLAT9,
		Type::DOMINANT_7_SHARP11,
		Type::DOMINANT_7_ADD9_FLAT5,
		Type::DOMINANT_7_ADD9_SHARP11,
		Type::DOMINANT_7_ADD13,
		Type::DOMINANT_7_SHARP9_FLAT13,
		Type::DOMINANT_7_SHARP11_ADD13,
		Type::DIMINISHED,
		Type::DIMINISHED_7,
		Type::MINOR_7_FLAT5,
		// TODO: implement!!!
		Type::AUGMENTED,
		Type::SUS4,
		Type::SUS2
	};
	
	// Constructor
	Chord(Chord::Type chordTypeArg,
		  uint8_t bassnoteArg,
		  uint8_t rootnoteArg )
	noexcept;
	
public:
	// Vector of notes of the chord.
	// when constructed it's in the root position.
	std::vector<Note> notes;
	
public:
	/*
	 * Intervals in ENGLISH with the DUTCH equivalent
	 */
	enum class Iv: unsigned int {
		// Whole and halfs
		H  = 1,     // Half step    (minor second)
		W  = 2,     // Whole step	(major second)
		WH = 3,     // Whole + Half	(minor third)
		WW = 4,     // Whole + Whole	(major third)
		WWH = 5,	// Whole + Whole + Half	(perfect fourth)
		WWW = 6,	// Whole + Whole + Whole	(tritonus)
		WWWH = 7, 	// Whole  + Whole + Whole + Half (perfect fifth)
		
		// English names
		PERFECT_UNISON  = 0,
		AUGMENTED_UNISON = 1,
		MINOR_SECOND = 1,
		MAJOR_SECOND = 2,
		AUGMENTED_SECOND = 3,
		MINOR_THIRD = 3,
		MAJOR_THIRD = 4,
		PERFECT_FOURTH = 5,
		TRITONE = 6,
		DIMINISHED_FIFTH = 6,
		PERFECT_FIFTH = 7,
		AUGMENTED_FIFTH = 8,
		MINOR_SIXTH = 8,
		MAJOR_SIXTH = 9,
		AUGMENTED_SIXTH = 10,
		MINOR_SEVENTH = 10,
		MAJOR_SEVENTH = 11,
		PERFECT_OCTAVE = 12,
		
		// Dutch names
		PRIME             =  0,
		KLEINE_SECUNDE    =  1,
		GROTE_SECUNDE     =  2,
		KLEINE_TERTS      =  3,
		GROTE_TERTS       =  4,
		REINE_KWART       =  5,
		OVERMATIGE_KWART   = 6,
		VERMINDERDE_KWINT  = 6,
		TRITONUS          =  6,
		REINE_KWINT       =  7,
		KLEINE_SEXT       =  8,
		GROTE_SEXT        =  9,
		KLEINE_SEPTIEM    = 10,
		GROTE_SEPTIEM     = 11,
		OCTAAF           =  12
	};
	
	// Constrants
	const std::vector<enum Iv> majorVect =
		//{Iv::WW, Iv::WH};
		{Iv::MAJOR_THIRD, Iv::MINOR_THIRD};
	const std::vector<enum Iv> major_6_Vect =
		{Iv::WW, Iv::WH, Iv::W};
	const std::vector<enum Iv> major_7_Vect =
		//{Iv::WW, Iv::WH, Iv::WW};
		{Iv::MAJOR_THIRD, Iv::MINOR_THIRD, Iv::MAJOR_THIRD};
	const std::vector<enum Iv> major_9_Vect =
		{Iv::WW, Iv::WH, Iv::WW ,Iv::WH};
	const std::vector<enum Iv> major_7_sharp11_Vect =
		{Iv::WW, Iv::WH, Iv::WW ,Iv::WH, Iv::WW};
	const std::vector<enum Iv> minorVect =
		//{Iv::WH, Iv::WW};
		{Iv::MINOR_THIRD, Iv::MAJOR_THIRD};
	const std::vector<enum Iv> minor_6_Vect =
		{Iv::WH, Iv::WW, Iv::W};
	const std::vector<enum Iv> minor_flat6_Vect =
		{Iv::WH, Iv::WW, Iv::H};
	const std::vector<enum Iv> minor_7_Vect =
		//{Iv::WH, Iv::WW, Iv::WH};
		{Iv::MINOR_THIRD, Iv::MAJOR_THIRD, Iv::MINOR_THIRD};
	const std::vector<enum Iv> minor_9_Vect =
		{Iv::WH, Iv::WW, Iv::WH, Iv::WW};
	const std::vector<enum Iv> minor_major_7_Vect =
		{Iv::WH, Iv::WW, Iv::WW};
	const std::vector<enum Iv> dominant_7_Vect =
		//{Iv::WW, Iv::WH, Iv::WH};
		{Iv::MAJOR_THIRD, Iv::MINOR_THIRD, Iv::MINOR_THIRD};
	const std::vector<enum Iv> dominant_flat5_Vect =
		{Iv::WW, Iv::W, Iv::WW};
	const std::vector<enum Iv> dominant_sharp5_Vect =
		{Iv::WW, Iv::WW, Iv::W};
	const std::vector<enum Iv> dominant_7_sus4 =
		{Iv::WWH, Iv::W, Iv::WH};
	const std::vector<enum Iv> dominant_7_flat9 =
		{Iv::WW, Iv::WH, Iv::WH, Iv::WH};
	const std::vector<enum Iv> dominant_7_sharp9 = \
		{Iv::WW, Iv::WH, Iv::WH, Iv::WWH};
	// As the intervals are getting larger using the Dutch intervals for
	// the definitions instead of wholes and halfs.
	const std::vector<enum Iv> dominant_7_sharp5_flat9 =
	{Iv::GROTE_TERTS, Iv::GROTE_TERTS, Iv::GROTE_SECUNDE, Iv::KLEINE_TERTS};
	const std::vector<enum Iv> dominant_7_sharp11 =
	{Iv::GROTE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_SEXT};
	const std::vector<enum Iv> dominant_7_add9_flat5 =
	{Iv::GROTE_TERTS, Iv::GROTE_SECUNDE, Iv::GROTE_TERTS, Iv::GROTE_TERTS};
	const std::vector<enum Iv> dominant_7_add9_sharp11 =
	{Iv::GROTE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS,
		Iv::GROTE_TERTS, Iv::GROTE_TERTS};
	const std::vector<enum Iv> dominant_7_add13 =
	{Iv::GROTE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS,
		Iv::GROTE_SEPTIEM};
	const std::vector<enum Iv> dominant_7_sharp9_flat13 =
	{Iv::GROTE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS,
		Iv::REINE_KWART, Iv::REINE_KWART};
	const std::vector<enum Iv> dominant_7_sharp11_add13 =
	{Iv::GROTE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS,
		Iv::KLEINE_SEXT, Iv::KLEINE_TERTS};
	const std::vector<enum Iv> diminished =
		{Iv::KLEINE_TERTS, Iv::KLEINE_TERTS};
	const std::vector<enum Iv> diminished_7 =
		{Iv::KLEINE_TERTS, Iv::KLEINE_TERTS, Iv::KLEINE_TERTS};
	const std::vector<enum Iv> minor_7_flat5 =
		{Iv::KLEINE_TERTS, Iv::KLEINE_TERTS, Iv::GROTE_TERTS};
	const std::vector<enum Iv> augmented =
		{Iv::GROTE_TERTS, Iv::GROTE_TERTS};
	const std::vector<enum Iv> sus4 =
		{Iv::REINE_KWART, Iv::GROTE_SECUNDE};
	const std::vector<enum Iv> sus2 =
		{Iv::GROTE_SECUNDE, Iv::REINE_KWART};
	// TODO: Finish the rest of the chords.

	// Lists of alternative Voicing.
public: enum class VoicingType {
		DROP1,
		DROP2,
		// TODO: work in progress
	};
public:
	void setVoicing(Chord::VoicingType voicingTypeArg,
						bool dropRootDown);
public: std::vector<Note> voicing;
	
	// Text representation of the Chords.
private: std::string privText;
public: std::string Text() {
	return Note::ToText(rootnote, false, false) + " " + privText;
};
	
private: std::string shortPrivText;
public: std::string ShortText() {
	return Note::ToText(rootnote, false, false) + shortPrivText;
};
	// Vector describing the Chord Recipe.
	// assigned in constructor.
private: std::vector<enum Iv> chordVect;
};

#endif /* Harmony_hpp */
/* EOF */
