#include "stdafx.h"

#include "Sheet.h"
#include "structs.h"
#include "Text.h"
#include <string>
#include "Windowsx.h"
#include "ScreenDialog.h"
#include "color.h"
#include "MST01.h"
#include "MIDI.h"



// sound a tugboat toot using a midi voice from the winmm.lib
// in case of Dev C++ link with libwinmm.a via
// Project>>Project Options>>Parameters>>Add Lib>>libwinmm.a
// BCX generated C code painstakingly modified for Dev C++ 
//
// Sound(Frequency,Duration[,Volume,Voice,Tempo])
// Volume = 0 to 127 (off to loudest)
// Voice = 0 to 127 (Shanai = 111, Tuba = 58, Accordion = 21)
// more midi voices: 
// Rock Organ = 18, Marimba = 12, Steel String Guitar = 25
// Choir Aahs = 52, Alto Sax = 65, Bird Tweet = 123, Sitar = 104
// FX 8 (sci-fi) = 103, FX 3 (crystal) = 98, Glockenspiel = 9 
//
// a Dev-C++ tested Console Application by  vegaseat  21nov2004
#include <cmath>
#include <windows.h>
#include <mmsystem.h>
using namespace std;


#define SNDQUE 10000
typedef struct _soundtype
{
	double  Freq;
	int     Dura;
	int     Vol;
	int     Voice;
	double  Tempo;
	int     sndTid;
} soundtype, * LPSOUNDTYPE;

static soundtype  SndPmtr[SNDQUE + 1];
static int        gTenter;
static int        gTwait;
static int        gTexit;
static int        gTarray;
static BOOL       gTsig;
static HANDLE     gSThread = NULL;

double  Round(double, int);
double  Abs(double);
int     Sound(float, int = 0, int = 127, int = 0, float = 1);
// changed this from int PlaySnd(void) to:
DWORD WINAPI PlaySnd(LPVOID);

HMIDIOUT hMidi;


// ***************** MIDI Instruments **********************

string MidiVoiceTable[128] =
{
	//Piano: 1 to 8
	"0-Acoustic Grand Piano",
	"1-Bright Acoustic Piano",
	"2-Electric Grand Piano",
	"3-Honky-tonk Piano",
	"4-Electric Piano 1",
	"5-Electric Piano 2",
	"6-Harpsichord",
	"7-Clavinet",

	//Chromatic Percussion: 9 to 16
	"8-Celesta",
	"9-Glockenspiel",
	"10-Music Box",
	"11-Vibraphone",
	"12-Marimba",
	"13-Xylophone",
	"14-Tubular Bells",
	"15-Dulcimer",

	//Organ: 17 to 24
	"16-Drawbar Organ",
	"17-Percussive Organ",
	"18-Rock Organ",
	"19-Church Organ",
	"20-Reed Organ",
	"21-Accordion",
	"22-Harmonica",
	"23-Tango Accordion",

	// Guitar: 25 to 32
	"24-Acoustic Guitar (nylon)",
	"25-Acoustic Guitar (steel)",
	"26-Electric Guitar (jazz)",
	"27-Electric Guitar (clean)",
	"28-Electric Guitar (muted)",
	"29-Overdriven Guitar",
	"30-Distortion Guitar",
	"31-Guitar harmonics",

	// Bass: 33 to 40
	"32-Acoustic Bass",
	"33-Electric Bass (finger)",
	"34-Electric Bass (pick)",
	"35-Fretless Bass",
	"36-Slap Bass 1",
	"37-Slap Bass 2",
	"38-Synth Bass 1",
	"39-Synth Bass 2",

	// Strings: 41 to 48
	"40-Violin",
	"41-Viola",
	"42-Cello",
	"43-Contrabass",
	"44-Tremolo Strings",
	"45-Pizzicato Strings",
	"46-Orchestral Harp",
	"47-Timpani",

	// Strings (continued): 49 to 56
	"48-String Ensemble 1",
	"49-String Ensemble 2",
	"50-Synth Strings 1",
	"51-Synth Strings 2",
	"52-Choir Aahs",
	"53-Voice Oohs",
	"54-Synth Voice",
	"55-Orchestra Hit",

	// Brass: 57 to 64
	"56-Trumpet",
	"57-Trombone",
	"58-Tuba",
	"59-Muted Trumpet",
	"60-French Horn",
	"61-Brass Section",
	"62-Synth Brass 1",
	"63-Synth Brass 2",


	// Reed: 65 to 72
	"64-Soprano Sax",
	"65-Alto Sax",
	"66-Tenor Sax",
	"67-Baritone Sax",
	"68-Oboe",
	"69-English Horn",
	"70-Bassoon",
	"71-Clarinet",

	// Pipe: 73 80
	"72-Piccolo",
	"73-Flute",
	"74-Recorder",
	"75-Pan Flute",
	"76-Blown Bottle",
	"77-Shakuhachi",
	"78-Whistle",
	"79-Ocarina",

	// Synth Lead: 81 to 88
	"80-Lead 1 (square)",
	"81-Lead 2 (sawtooth)",
	"82-Lead 3 (calliope)",
	"83-Lead 4 (chiff)",
	"84-Lead 5 (charang)",
	"85-Lead 6 (voice)",
	"86-Lead 7 (fifths)",
	"87-Lead 8 (bass + lead)",

	// Synth Pad: 89 to 96
	"88-Pad 1 (new age)",
	"89-Pad 2 (warm)",
	"90-Pad 3 (polysynth)",
	"91-Pad 4 (choir)",
	"92-Pad 5 (bowed)",
	"93-Pad 6 (metallic)",
	"94-Pad 7 (halo)",
	"95-Pad 8 (sweep)",

	// Synth Effects: 97 to 104
	"96-FX 1 (rain)",
	"97-FX 2 (soundtrack)",
	"98-FX 3 (crystal)",
	"99-FX 4 (atmosphere)",
	"100-FX 5 (brightness)",
	"101-FX 6 (goblins)",
	"102-FX 7 (echoes)",
	"103-FX 8 (sci-fi)",

	// Ethnic: 105 to 112
	"104-Sitar",
	"105-Banjo",
	"106-Shamisen",
	"107-Koto",
	"108-Kalimba",
	"109-Bag pipe",
	"110-Fiddle",
	"111-Shanai",

	// Percussive: 113 to 119
	"112-Tinkle Bell",
	"113-Agogo",
	"114-Steel Drums",
	"115-Woodblock",
	"116-Taiko Drum",
	"117-Melodic Tom",
	"118-Synth Drum",

	// Sound effects: 120 to 128
	"119-Reverse Cymbal",
	"120-Guitar Fret Noise",
	"121-Breath Noise",
	"122-Seashore",
	"123-Bird Tweet",
	"124-Telephone Ring",
	"125-Helicopter",
	"126-Applause",
	"127-Gunshot"
};

// *************************** MIDI Play *********************************************
#if 0
int MidiPlay_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool trigger = false;
	if (input[5].value[0]) {
		if (memory[0] == 0)
			trigger = true;
		memory[0] = 1;
	}
	else
		memory[0] = 0;
	
	if (trigger) {

		// wait till que is empty
		while (Sound(0) != 0)
			Sleep(10);

		for (int k = 0; k < loops; k++) {
			float note = floor(getLoopingValueD(0, k, 30));
			int duration = floor(getLoopingValueD(1, k, 1000));
			int volume = floor(getLoopingValueD(2, k, 127));
			int voice = floor(getLoopingValueD(3, k, 0));
			float tempo = getLoopingValueD(4, k, 1);

			Sound(note, duration, volume, voice, tempo);
		}
	}
	
	output[0].stringType = true;
	int voices = input[3].value.size();
	output[0].stringValue.resize(voices);
	for (int k = 0; k < voices; k++) {
		int voice = getLoopingValueD(3, k, 111);
		if (voice < 0)
			voice = 0;
		if (voice > 126)
			voice = 126;
		output[0].stringValue[k] = MidiVoiceTable[voice];
	}
	output[0].dimensions[0] = voices;
	
	return 1;
}
#endif


int MidiPlay_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	const int NOTE = 0;
	const int VOLUME = 1;
	const int VOICE = 2;
	const int CHANNEL = 3;
	const int TRIGGER = 4;
	
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool triggerPositiveEdge = false;
	bool triggerNegativeEdge = false;
	if (input[TRIGGER].value[0]) {
		if (memory[0] == 0)
			triggerPositiveEdge = true;
		memory[0] = 1;
	}
	else {
		if (memory[0] == 1)
			triggerNegativeEdge = true;
		memory[0] = 0;
	}

	MMRESULT result;

	if (triggerPositiveEdge || triggerNegativeEdge) {
		
		for (int k = 0; k < loops; k++) {
			int note = floor(getLoopingValueD(NOTE, k, 30));
			if (note < 0)
				note = 0;
			if (note > 127)
				note = 127;
			int volume = floor(getLoopingValueD(VOLUME, k, 127));
			if (volume < 0)
				volume = 0;
			if (volume > 127)
				volume = 127;
			int voice = floor(getLoopingValueD(VOICE, k, 0));
			if (voice < 0)
				voice = 0;
			if (voice > 126)
				voice = 126;
			int channel = floor(getLoopingValueD(CHANNEL, k, 0));
			if (channel < 0)
				channel = 0;
			if (channel > 8)
				channel = 8;

			if (triggerPositiveEdge) {
				if (initialMemoryX.size() != loops) {
					element.resize(loops);			// volume
					initialMemoryX.resize(loops);	// voice
					initialMemoryY.resize(loops);	// channel
					for (int k = 0; k < loops; k++)
						initialMemoryX[k] = -1;
				}

				if( hMidi == NULL )
					result = midiOutOpen(&hMidi, (UINT)-1, 0, 0, CALLBACK_NULL);

				if (initialMemoryX[k] != -1) {
					int v = element[k];			// volume
					int n = initialMemoryX[k];	// note
					int c = initialMemoryY[k];	// channel
					int Phrase = (v * 256 + n) * 256 + 128 + c;
					result = midiOutShortMsg(hMidi, Phrase);
				}

				result = midiOutShortMsg(hMidi, (256 * voice) + 192 + channel);
				int Phrase = (volume * 256 + note) * 256 + 144 + channel;
				result = midiOutShortMsg(hMidi, Phrase);
				element[k] = volume;			// volume
				initialMemoryX[k] = note;		// note
				initialMemoryY[k] = channel;	// channel
			}

			if (triggerNegativeEdge) {
				int Phrase = (volume * 256 + note) * 256 + 128 + channel;
				result = midiOutShortMsg(hMidi, Phrase);
				if( k < initialMemoryX.size())
					initialMemoryX[k] = -1;
				//result = midiOutClose(hMidi);
			}
		}
	}

	output[0].stringType = true;
	int voices = input[VOICE].value.size();
	output[0].stringValue.resize(voices);
	for (int k = 0; k < voices; k++) {
		int voice = getLoopingValueD(VOICE, k, 111);
		if (voice < 0)
			voice = 0;
		if (voice > 126)
			voice = 126;
		output[0].stringValue[k] = MidiVoiceTable[voice];
	}
	output[0].dimensions[0] = voices;

	return 1;
}

MidiPlay_Component::MidiPlay_Component()
	: Component(L"MidiPlay", L"MIDI Play", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "Note", "Volume", "Voice", "Channel", "Trigger");
	setRightLabels(3, "Voice");
	width = 45;
	setMemory(0, 0); // edge trigger
}


// ************************** MIDI Percussion **************************************




string MidiDrumTable[47] =
{
	"35 Bass Drum 2",	// 0
	"36 Bass Drum 1",
	"37 Side Stick",
	"38 Snare Drum 1",
	"39 Hand Clap",
	"40 Snare Drum 2",
	"41 Low Tom 2",
	"42 Closed Hi-hat",
	"43 Low Tom 1",
	"44 Pedal Hi-hat",
	"45 Mid Tom 2",		// 10
	"46 Open Hi-hat",
	"47 Mid Tom 1",
	"48 High Tom 2",
	"49 Crash Cymbal 1",
	"50 High Tom 1",
	"51 Ride Cymbal 1",
	"52 Chinese Cymbal",
	"53 Ride Bell",
	"54 Tambourine",
	"55 Splash Cymbal",		// 20
	"56 Cowbell",
	"57 Crash Cymbal 2",
	"58 Vibra Slap",
	"59 Ride Cymbal 2",
	"60 High Bongo",
	"61 Low Bongo",
	"62 Mute High Conga",
	"63 Open High Conga",
	"64 Low Conga",
	"65 High Timbale",		// 30
	"66 Low Timbale",
	"67 High Agogo",
	"68 Low Agogo",
	"69 Cabasa",
	"70 Maracas",
	"71 Short Whistle",
	"72 Long Whistle",
	"73 Short Guiro",
	"74 Long Guiro",
	"75 Claves",			// 40
	"76 High Wood Block",
	"77 Low Wood Block",
	"78 Mute Cuica",
	"79 Open Cuica",
	"80 Mute Triangle",
	"81 Open Triangle"		//46
};


int MidiPercussion_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	const int VOLUME = 0;
	const int VOICE = 1;
	const int TRIGGER = 2;

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool triggerPositiveEdge = false;
	bool triggerNegativeEdge = false;
	if (input[TRIGGER].value[0]) {
		if (memory[0] == 0)
			triggerPositiveEdge = true;
		memory[0] = 1;
	}
	else {
		if (memory[0] == 1)
			triggerNegativeEdge = true;
		memory[0] = 0;
	}

	if (triggerPositiveEdge || triggerNegativeEdge) {

		for (int k = 0; k < loops; k++) {

			int volume = floor(getLoopingValueD(VOLUME, k, 127));
			if (volume < 0)
				volume = 0;
			if (volume > 127)
				volume = 127;
			int voice = floor(getLoopingValueD(VOICE, k, 0));
			if (voice < 35)
				voice = 35;
			if (voice > 81)
				voice = 81;

			if (triggerPositiveEdge) {
				if (hMidi == NULL)
					midiOutOpen(&hMidi, (UINT)-1, 0, 0, CALLBACK_NULL);
				midiOutShortMsg(hMidi, (256 * voice) + 192 + 9);
				int Phrase = (volume * 256 + voice) * 256 + 144 + 9;
				midiOutShortMsg(hMidi, Phrase);
			}

			if (triggerNegativeEdge) {
				int Phrase = (volume * 256 + voice) * 256 + 128 + 9;
				midiOutShortMsg(hMidi, Phrase);
				//midiOutClose(hMidi);
			}
		}
	}

	output[0].stringType = true;
	int voices = input[VOICE].value.size();
	output[0].stringValue.resize(voices);
	for (int k = 0; k < voices; k++) {
		int voice = getLoopingValueD(VOICE, k, 35);
		if (voice < 35)
			voice = 35;
		if (voice > 81)
			voice = 81;
		output[0].stringValue[k] = MidiDrumTable[voice-35];
	}
	output[0].dimensions[0] = voices;

	return 1;
}

MidiPercussion_Component::MidiPercussion_Component()
	: Component(L"MidiDrum", L"MIDI Percussion", squareShape, 3, 1, 1, 1)
{
	setLeftLabels(3, "Volume", "Voice", "Trigger");
	setRightLabels(3, "Voice");
	width = 45;
	setMemory(0, 0); // edge trigger
}


// ************************** Sequencer ********************************

int Sequencer_Component::method2(Sheet* pg) {

	if (!checkValueInputs()) {
		memory[0] = 0;
		return clearOutputs();
	}

	const int INTERVALS = 0;
	const int STEPS = 1;
	const int DWELL = 2;
	const int ENABLE = 3;
	const int STEP = 0;
	const int PULSE = 1;

	if (input[ENABLE].value[0] == 0) {
		memory[0] = 0;
		return clearOutputs();
	}

	if( input[INTERVALS].drivenByComponent == -1) {
		memory[0] = 0;
		return clearOutputs();
	}

	int intervals = input[INTERVALS].value.size();
	if (intervals < 1) {
		memory[0] = 0;
		return clearOutputs();
	}

	double dwell = getLoopingValueD(DWELL, 0, 50);
	if (dwell < 1)
		dwell = 1;
	if (dwell > 99)
		dwell = 99;
	dwell /= 100;

	int step = output[STEP].value[0];
	int steps = getLoopingValueD(STEPS, 0, intervals);
	int index = step % intervals;
	double interval = input[INTERVALS].value[index];
	double elapsedTime = memory[0];

	if (elapsedTime >= interval) { // index to next interval
		step++;
		if (step < steps)  // advance to next step
			output[STEP].value[0] = step;
		else  // back to first step
			output[STEP].value[0] = 0;

		output[PULSE].value[0] = 1;
		memory[0] = 0;
	}
	else // check pulse width
	{
		double pulseFraction = elapsedTime / interval;
		if( pulseFraction < dwell)
			output[PULSE].value[0] = 1;
		else
			output[PULSE].value[0] = 0;
		memory[0] += pg->timeStep;
	}

	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;

	return 1;
}

Sequencer_Component::Sequencer_Component()
	: Component(L"Sequ", L"Sequencer", squareShape, 4, 1, 2, 1)
{
	setLeftLabels(3, "Intervals", "Steps", "Dwell %", "Enable");
	setRightLabels(3, "Step", "Pulse");
	width = 45;
	setMemory(0, 0);
	timeBased = true;
}





#if 0
int tugboat()
{
	// Tugboat whistle sound 95 hertz, 2000ms, 127 = loud, 111 = Shanai
	// experiment with your own sounds, it's fun ...
	Sound(95, 2000, 127, 111);  // 2 second blast 
	Sound(1, 1000, 0, 111);  // 1 second of silence 
	Sound(95, 2000, 127, 111);  // 2 second blast 
	Sound(1, 1000, 0, 111);  // 1 second of silence 
	Sound(95, 2000, 127, 111);  // 2 second blast

	// wait till que is empty
	while (Sound(0) != 0)
	{
		Sleep(10);
	}

	return 0;
}
double Round(double n, int d)
{
	return (floor((n)* pow(10.0, (d)) + 0.5) / pow(10.0, (d)));
}
double Abs(double a)
{
	if (a < 0)  return -a;
	return  a;
}
int Sound(float Freq, int Dura, int Vol, int Voice, float Tempo)
{
	DWORD  dwThreadId;

	if (Freq == 0 && Dura < 1) return gTenter - gTexit;
	// silence
	if (Freq == 0) Vol = 0;
	if (Dura < 5) Dura = 5;
	gTenter++;
	gTsig = FALSE;
	if (gTenter >= SNDQUE)
	{
		gTarray = gTenter % SNDQUE + 1;
	}
	else
	{
		gTarray = gTenter;
	}
	SndPmtr[gTarray].Freq = Freq;
	SndPmtr[gTarray].Dura = Dura;
	SndPmtr[gTarray].Tempo = Tempo;
	SndPmtr[gTarray].Vol = Vol;
	SndPmtr[gTarray].Voice = Voice;
	SndPmtr[gTarray].sndTid = gTenter;
	if (gSThread == NULL && (Freq == Abs(Freq) || Freq == 0))
	{
		// "PlaySnd" needs casting (void *)
		gSThread = CreateThread(NULL, 0, PlaySnd, (void*)"PlaySnd", 0, &dwThreadId);
		Sleep(1);
		return 0;
	}
	if (Freq != Abs(Freq))
	{
		if (Freq == -1)
		{
			Freq = 0;
			SndPmtr[gTarray].Vol = 0;
		}
		SndPmtr[gTarray].Freq = Abs(Freq);
		gTsig = TRUE;
		while (gSThread != NULL)
		{
			Sleep(10);
		}
		gTexit = gTenter - 1;
		gTwait = gTenter - 1;
		gTsig = FALSE;
		return PlaySnd(0);  // needs some kind of argument
	}
	return 0;
}
DWORD WINAPI PlaySnd(LPVOID)
{
	soundtype  LocSndPar;
	int  lTarray;
	int channel = 0;

	while (gTenter > gTexit && gTsig == FALSE)
	{
		gTwait++;
		if (gTwait >= SNDQUE)
			lTarray = gTwait % SNDQUE + 1;
		else
			lTarray = gTwait;
		LocSndPar = SndPmtr[lTarray];
		int Note = 0;
		int Phrase = 0;
		HMIDIOUT hMidi;
		midiOutOpen(&hMidi, (UINT)-1, 0, 0, CALLBACK_NULL);
		midiOutShortMsg(hMidi, (256 * LocSndPar.Voice) + 192 + channel);
		// convert frequency to midi note
	//	Note = (int)Round((log(LocSndPar.Freq) - log(440.0)) / log(2.0) * 12 + 69, 0);
		Note = (int)LocSndPar.Freq;
		Phrase = (LocSndPar.Vol * 256 + Note) * 256 + 144 + channel;
		midiOutShortMsg(hMidi, Phrase);
		Sleep((int)(LocSndPar.Dura * (1 / LocSndPar.Tempo + 0.0001)));
		Phrase = (LocSndPar.Vol * 256 + Note) * 256 + 128 + channel;
		midiOutShortMsg(hMidi, Phrase);
		midiOutClose(hMidi);
		gTexit++;
	}
	CloseHandle(gSThread);
	gSThread = NULL;
	return 0;
}
#endif