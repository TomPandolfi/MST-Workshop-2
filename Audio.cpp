#include "stdafx.h"
#include "Sheet.h"
#include <ctime>
#include <string>
#include "ScreenDialog.h"
#include "Audio.h"
#include "Text.h"

#include <mmsystem.h>  /* multimedia functions for Windows */

#pragma warning (disable: 4244)








/*
#include "W32_X86/INCLUDE/xvt.h"                    
#include <math.h>
#include "Const.h"

#include "Main.h"
#include "Bench2.h"
#include "file2.h"
#include "SetColor.h"
#include "Util.h"
#include "Param.h"
#include "Directory.h"
#include "ArrayUtility.h"
#include "Toolsbar.h"	// for Methods
#include "FindIt.h"		// for Methods
#include "Midi.h"

#include "MidiSend.h"
*/




const char *lastTrigger = "MIDISEND_TRIGGER";
const char *lastChannel = "MIDISEND_CHANNEL";
const char *lastProgram = "MIDISEND_PROGRAM";
const char *lastPort    = "MIDISEND_PORT";


#define TRIGGER_STROBE   0
#define TRIGGER_GATE	 1

#define SHORT_PORT		0
#define SHORT_CHANNEL	1

/*
static void NewMidiSend(WINDOW win, char *comment)
{
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part;
	int			i = winInfo->partCount;
	int			j;


	AllocatePartMemory2(&winInfo->part, &winInfo->partAllocation, i + 1);
	part = winInfo->part;
	ZeroPart(part, i);

	part[i].object.type = PART_TYPE_MIDISEND;
	part[i].object.version = PartTypeVersion[part[i].object.type];
	strcpy_s(part[i].object.comment, MAX_COMMENT, comment);

	part[i].object.x[0] = winInfo->horizontalPosition + 50;
	part[i].object.y[0] = winInfo->verticalPosition + 100;
	part[i].object.points = 1;

	part[i].object.choice = GetLongParam(lastTrigger, TRIGGER_STROBE);
	part[i].object.choice2 = GetLongParam(lastProgram, 0);
	part[i].object.shorts[SHORT_PORT] = (short)GetLongParam(lastPort, 0);
	part[i].object.shorts[SHORT_CHANNEL] = (short)GetLongParam(lastChannel, 0);

	part[i].object.terminals = 3;

	for (j = 0; j < part[i].object.terminals; j++)
	{
		part[i].object.terminalIsInput[j] = TRUE;
		part[i].object.terminalIsOutput[j] = FALSE;
	}

	PutPartInDragModeC(win);
	winInfo->partCount++;

	xvt_win_set_cursor(win, 128); 
}
*/






//typedef struct
//{
//	int		trigger;
//	int		program;
//	int		channel;
//	int		port;
//	int		result;
//
//} DIALOG_DATA;



/*
static long
dialog_handler(WINDOW win, EVENT *eventPtr)
{
	WINDOW		ctlwin;
	DIALOG_DATA *data;
	char		text[20];
	int			j;


	data = (DIALOG_DATA*)xvt_vobj_get_data(win);

	switch (eventPtr->type)
	{
	case E_CREATE:

		ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_TRIGGER);
		xvt_list_add(ctlwin, TRIGGER_STROBE, "Strobe");
		xvt_list_add(ctlwin, TRIGGER_GATE, "Gate");
		xvt_list_set_sel(ctlwin, data->trigger, TRUE);

		ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_PORT);
		sprintf_s(text, 20, "%d", data->port);
		xvt_vobj_set_title(ctlwin, text);

		ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_CHANNEL);
		sprintf_s(text, 20, "%d", data->channel);
		xvt_vobj_set_title(ctlwin, text);

		ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_PROGRAM);
		for (j = 0; j < 128; j++)
			xvt_list_add(ctlwin, j, programTable[j]);
		xvt_list_set_sel(ctlwin, data->program, TRUE);
		break;

	case E_UPDATE:
		xvt_dwin_clear(win, xvt_vobj_get_attr(NULL_WIN, ATTR_BACK_COLOR));
		break;

	case E_CHAR:
		switch (eventPtr->v.chr.ch)
		{
		case 13:	// return 
			data->result = NEW_MIDISEND_OK;
			xvt_vobj_destroy(win);
			break;

		case 27:	// escape 
			data->result = NEW_MIDISEND_CANCEL;
			xvt_vobj_destroy(win);
			break;
		}
		break;

	case E_CONTROL:

		switch (eventPtr->v.ctl.id)
		{
		case NEW_MIDISEND_OK:
		case NEW_MIDISEND_CANCEL:
		case NEW_MIDISEND_DELETE:
			data->result = eventPtr->v.ctl.id;
			xvt_vobj_destroy(win);
			break;

		case NEW_MIDISEND_TRIGGER:
			ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_TRIGGER);
			data->trigger = xvt_list_get_sel_index(ctlwin);
			break;

		case NEW_MIDISEND_PORT:
			ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_PORT);
			xvt_vobj_get_title(ctlwin, text, 20);
			data->port = atoi(text);
			break;

		case NEW_MIDISEND_CHANNEL:
			ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_CHANNEL);
			xvt_vobj_get_title(ctlwin, text, 20);
			data->channel = atoi(text);
			break;

		case NEW_MIDISEND_PROGRAM:
			ctlwin = xvt_win_get_ctl(win, NEW_MIDISEND_PROGRAM);
			data->program = xvt_list_get_sel_index(ctlwin);
			break;
		}
		break;

	case E_CLOSE:
		data->result = NEW_MIDISEND_CANCEL;
		xvt_vobj_destroy(win);
		return 0L;
	}

	return 0L;
}
*/


/*
void SetMidiSendTerminals(WINDOW win, int i)
{
	int			x, y;
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part = winInfo->part;
	int			k;


	x = part[i].object.x[0] - winInfo->horizontalPosition;
	y = part[i].object.y[0] - winInfo->verticalPosition;

	// setup the input terminals 
	y -= 3 * TERMINAL_GAP / 2;
	for (k = 0; k < part[i].object.terminals; k++)
	{
		part[i].state.terminal[k].top = y - TERMINAL_RADIUS;
		part[i].state.terminal[k].bottom = y + TERMINAL_RADIUS;
		part[i].state.terminal[k].left = x - MIDISEND_WIDTH - TERMINAL_WIDTH;
		part[i].state.terminal[k].right = x - MIDISEND_WIDTH;

		y += TERMINAL_GAP;
	}

}
*/


/*
void DrawMidiSend(WINDOW win, int i, RCT *updateRect)
{
	DRAW_CTOOLS ctools;
	int			x, y;
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part = winInfo->part;
	int			j;
	RCT			dummy;
	char		text[30];


	x = part[i].object.x[0] - winInfo->horizontalPosition;
	y = part[i].object.y[0] - winInfo->verticalPosition;

	part[i].state.body.top = y - MIDISEND_HEIGHT;
	part[i].state.body.bottom = y + MIDISEND_HEIGHT;
	part[i].state.body.left = x - MIDISEND_WIDTH;
	part[i].state.body.right = x + MIDISEND_WIDTH;

	part[i].state.footprint = part[i].state.body;

	part[i].state.footprint.top -= 1;
	part[i].state.footprint.bottom += 1;
	part[i].state.footprint.left -= 1 + TERMINAL_WIDTH;
	part[i].state.footprint.right += 1;

	expandFootprintForComment(win, &part[i]);

	if (updateRect != 0)
		if (!xvt_rect_intersect(&dummy, updateRect, &part[i].state.footprint))
			return;

	drawCommentNearPart(win, &part[i]);

	xvt_app_get_default_ctools(&ctools);

	// draw the body 

	if (!part[i].state.outputIsReady)
		ctools.brush.color = UNUSED_PART_COLOR;
	else
		if (part[i].object.driven[0])
			ctools.brush.color = convertDigitalVolts2Color(
				FALSE, part[i].state.terminalValue[0]);
		else
			ctools.brush.color = UNUSED_PART_COLOR;
	if (part[i].state.selected != part[i].state.invertSelection)
		ctools.pen.width = SELECTED_PEN_WIDTH;

	xvt_dwin_set_draw_ctools(win, &ctools);

	xvt_dwin_draw_roundrect(win, &part[i].state.body, BOX_RADIUS, BOX_RADIUS);

	SetMidiSendTerminals(win, i);


	// draw the terminals 

	ctools.brush.color = TERMINAL_COLOR;
	xvt_dwin_set_draw_ctools(win, &ctools);

	for (j = 0; j < part[i].object.terminals; j++)
	{
		xvt_dwin_draw_oval(win, &part[i].state.terminal[j]);

		if (j == 0)
		{

			if (part[i].object.choice)
				xvt_dwin_draw_text(win, part[i].state.terminal[j].right + 2,
					part[i].state.terminal[j].bottom,
					"Gate", -1);
			else
				xvt_dwin_draw_text(win, part[i].state.terminal[j].right + 2,
					part[i].state.terminal[j].bottom,
					"Strobe", -1);
		}
		else
			xvt_dwin_draw_text(win, part[i].state.terminal[j].right + 2,
				part[i].state.terminal[j].bottom,
				inputLabel[j], -1);
	}


	sprintf_s(text, 30, "MIDI Send     %d", part[i].object.shorts[SHORT_PORT]);
	xvt_dwin_draw_text(win, x - 50,
		y - 38,
		text, -1);

	sprintf_s(text, 30, "Channel %d", part[i].object.shorts[SHORT_CHANNEL]);
	xvt_dwin_draw_text(win, x - 40,
		y + 32,
		text, -1);


	if (part[i].object.shorts[SHORT_CHANNEL] == 9)
	{
		j = (int)part[i].state.terminalValue[1] - 35;
		if ((j >= 0) && (j < 47))
			xvt_dwin_draw_text(win, x - 70, y + 49,
				drumTable[j], -1);
		else
		{
			sprintf_s(text, 30, "Percussion %d", (int)part[i].state.terminalValue[1]);
			xvt_dwin_draw_text(win, x - 70, y + 49,
				text, -1);
		}
	}
	else
		xvt_dwin_draw_text(win, x - 70, y + 49,
			programTable[part[i].object.choice2], -1);


	xvt_app_get_default_ctools(&ctools);
	xvt_dwin_set_draw_ctools(win, &ctools);
}
*/



/*
void DrawMidiSendOutline(WINDOW win, int i)
{
	int	x, y;
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part = winInfo->part;
	int			j;

	x = part[i].object.x[0] - winInfo->horizontalPosition;
	y = part[i].object.y[0] - winInfo->verticalPosition;

	part[i].state.body.top = y - MIDISEND_HEIGHT;
	part[i].state.body.bottom = y + MIDISEND_HEIGHT;
	part[i].state.body.left = x - MIDISEND_WIDTH;
	part[i].state.body.right = x + MIDISEND_WIDTH;

	xvt_dwin_draw_roundrect(win, &part[i].state.body, BOX_RADIUS, BOX_RADIUS);

	// draw the terminals 
	SetMidiSendTerminals(win, i);

	for (j = 0; j < part[i].object.terminals; j++)
		xvt_dwin_draw_oval(win, &part[i].state.terminal[j]);

}
*/







/*
BOOLEAN MidiSendDialog(WINDOW win, int i)
{
	DIALOG_DATA	data;
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part = winInfo->part;



	data.trigger = part[i].object.choice;
	data.program = part[i].object.choice2;
	data.port = part[i].object.shorts[SHORT_PORT];
	data.channel = part[i].object.shorts[SHORT_CHANNEL];

	xvt_win_create_res(NEW_MIDISEND_DIALOG, SCREEN_WIN, EM_ALL,
		dialog_handler, (long)&data);
	switch (data.result)
	{
	case NEW_MIDISEND_OK:


		if (data.program < 0)
			data.program = 0;
		if (data.program > 127)
			data.program = 127;

		if (data.port < 0)
			data.port = 0;
		if (data.port > 15)
			data.port = 15;

		if (data.channel < 0)
			data.channel = 0;
		if (data.channel > 15)
			data.channel = 15;

		SetLongParam(lastTrigger, data.trigger);
		SetLongParam(lastProgram, data.program);
		SetLongParam(lastPort, data.port);
		SetLongParam(lastChannel, data.channel);

		ReallyWriteParams();

		if (i == NULL_PART)
		{
			NewMidiSend(win, "MIDI Send ");
		}
		else
		{
			part[i].object.choice = data.trigger;
			part[i].object.choice2 = data.program;
			part[i].object.shorts[SHORT_PORT] = data.port;
			part[i].object.shorts[SHORT_CHANNEL] = data.channel;
		}
		break;

	case NEW_MIDISEND_DELETE:
		DeletePart(win, i);
		break;
	}

	return (BOOLEAN)(data.result != NEW_MIDISEND_CANCEL);
}
*/


/*
BOOLEAN MouseOverMidiSend(WINDOW win, int i, PNT where)
{
	WINDOW_INFO	*winInfo = (WINDOW_INFO*)xvt_vobj_get_data(win);
	PART_INFO	*part = winInfo->part;

	if (!xvt_rect_has_point(&part[i].state.body, where))
		return FALSE;

	return TRUE;
}
*/




typedef struct
{
	int		type;
	int		program;
	int		channel;
	int		port;
} CALLBACK_DATA1;



void InnerPlayMidiSend(void *data, double *value)
{
	CALLBACK_DATA1 *callbackData = (CALLBACK_DATA1*)data;


	/* pitch */
	if (value[0] < 0)
		value[0] = 0;
	if (value[0] > 127)
		value[0] = 127;

	/* velocity */
	if (value[1] < 0)
		value[1] = 0;
	if (value[1] > 127)
		value[1] = 127;


	if (callbackData->channel < 0)
		callbackData->channel = 0;
	if (callbackData->channel > 15)
		callbackData->channel = 15;

	if (callbackData->port < 0)
		callbackData->port = 0;
	if (callbackData->port > 15)
		callbackData->port = 15;


	//sendMidiNote((int)value[0], (int)value[1], callbackData->channel,
	//	callbackData->type, callbackData->program, callbackData->port);
}


/*
void PropagateMidiSend(WINDOW win, PART_INFO *part, int i)
{
	double	oldValue;
	double	newValue;
	BOOLEAN	oldBool;
	BOOLEAN	newBool;
	double	data[4];
	int		k;
	CALLBACK_DATA1	callbackData;
	BOOLEAN	sendIt;


	if (part[i].object.terminals > 4)
		return;

	for (k = 0; k < part[i].object.terminals; k++)
	{
		if (!part[i].object.driven[k])
			return;

		if (!part[part[i].object.driverPart[k]].state.outputIsReady)
			return;

		if (k == 0)
		{
			oldValue = part[i].state.terminalValue[0];
			oldBool = oldValue > LOGIC_THRESHOLD_VOLTS;
		}

		newValue = part[part[i].object.driverPart[k]].state.terminalValue[part[i].object.driverPin[k]];
		part[i].state.terminalValue[k] = newValue; // echo

		if (k == 0)
			newBool = newValue > LOGIC_THRESHOLD_VOLTS;
		else
			data[k] = part[part[i].object.driverPart[k]].state.terminalValue[part[i].object.driverPin[k]];
	}

	part[i].state.outputIsReady = TRUE;

	if (oldBool != newBool)
	{
		sendIt = FALSE;
		if (part[i].object.choice == 0) // Strobe trigger
		{
			if ((!oldBool) && newValue) // positive edge 
				sendIt = TRUE;
		}
		else		// gate trigger
			sendIt = TRUE;


		if (sendIt)
		{

			callbackData.type = newBool;
			callbackData.program = part[i].object.choice2;
			callbackData.port = part[i].object.shorts[SHORT_PORT];
			callbackData.channel = part[i].object.shorts[SHORT_CHANNEL];

			data[0] = data[1];
			data[1] = data[2];
			data[2] = data[3];

			ArrayCallBack(data, 3, 0, part, i, (void*)&callbackData, InnerPlayMidiSend, 0);
		}

		if (win)
			xvt_dwin_invalidate_rect(win, &part[i].state.footprint);
	}
	else
	{
		if (win)
			if (part[i].object.shorts[SHORT_CHANNEL] == 9)
				xvt_dwin_invalidate_rect(win, &part[i].state.footprint);

	}
}
*/


const char *noteTable[128] =
{   "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", // 36 to 47
	"C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3", // 48 to 59
	"C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", // 60 to 71
	"C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5", // 72 to 83
	"C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6", // 84 to 95
};

const char *programTable[128] =
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



const char *drumTable[47] =
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






#define MAX_DEVICES 16
#define MAX_CHANNELS 16

static	BOOLEAN		attached[MAX_DEVICES] = { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
												FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };
static	HMIDIOUT	device[MAX_DEVICES];			// MIDI device interface for sending MIDI output
char		portProgram[MAX_DEVICES][MAX_CHANNELS];



void ShutdownMidi()
{
	int port;

	for (port = 0; port < 16; port++)
	{
		if (attached[port])
		{
			// turn any MIDI notes currently playing:
			midiOutReset(device[port]);

			// Remove any data in MIDI device and close the MIDI Output port
			midiOutClose(device[port]);

			attached[port] = 0;
		}
	}
}



int PlayMidi_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	int ckey;
	int velocity;
	int channel;
	int program;
	int port;

	for (int j = 0; j < loops; j++) {
		ckey = getLoopingValueD(0, j, 65);		// midi note number
		velocity = getLoopingValueD(1, j, 20);
		channel = getLoopingValueD(2, j, 0);
		program = getLoopingValueD(3, j, 0);	// the instrument
		port = getLoopingValueD(4, j, 0);		// select which MIDI output port to open

		int notestate;		// keeping track of when the note is on or off
		int	flag;			// monitor the status of returning functions
		// variable which is both an integer and an array of characters:
		union { unsigned long word; unsigned char data[4]; } message;
		
		// message.data[0] = command byte of the MIDI message, for example: 0x90
		// message.data[1] = first data byte of the MIDI message, for example: 60
		// message.data[2] = second data byte of the MIDI message, for example 100
		// message.data[3] = not used for any MIDI messages, so set to 0

		if (port < 0)
			port = 0;
		else
			if (port >= MAX_DEVICES)
				port = MAX_DEVICES - 1;

		if (channel < 0)
			channel = 0;
		else
			if (channel >= MAX_CHANNELS)
				channel = MAX_CHANNELS - 1;

		if (!attached[port])
		{
			// Open the MIDI output port
			flag = midiOutOpen(&device[port], port, 0, 0, CALLBACK_NULL);
			if (flag != MMSYSERR_NOERROR) {
				//printf("Error opening MIDI Output.\n");
				return 1;
			}
			attached[port] = TRUE;

			// clear the programs
			for (j = 0; j < MAX_CHANNELS; j++)
				portProgram[port][j] = (char)255;
		}

		// what is currently set for this channel?
		if (portProgram[port][channel] != program)
		{
			// set program 
			message.data[0] = 0xC0 + channel;
			message.data[1] = program;
			message.data[2] = 0;	// Unused parameter
			message.data[3] = 0;    // Unused parameter
			flag = midiOutShortMsg(device[port], message.word);

			portProgram[port][channel] = program;
		}


		notestate = 0; // to kill a an error on the next line
		// Main event loop
		if (notestate)
		{
			// turn on note.
			message.data[0] = 0x90 + channel;
			message.data[1] = ckey;
			message.data[2] = velocity;
			message.data[3] = 0;	// Unused parameter
		}
		else
		{
			// turn off note.
			message.data[0] = 0x80 + channel;
			message.data[1] = ckey;
			message.data[2] = velocity;
			message.data[3] = 0;		// Unused parameter
		}

		flag = midiOutShortMsg(device[port], message.word);
		if (flag != MMSYSERR_NOERROR) {
			//printf("Warning: MIDI Output is not open.\n");
		}
	}

	return 0;
}
PlayMidi_Component::PlayMidi_Component()
	: Component(L"PlayMidi", L"Play MIDI", squareShape, 5, 1, 0, 1)
{
	setLeftLabels(5, "Note", "Velocity", "Channel", "Instrument", "Port");
}

// *************************** Beep ***********************

int Beep_Component::method2(Sheet* pg) {

	if (input[0].value.size() == 0)
		return clearOutputs();
	if (input[1].value.size() == 0)
		return clearOutputs();

	int type;
	switch ((int)input[1].value[0]) {
	case 0:	type = MB_OK; break;
	case 1:	type = MB_ICONASTERISK; break;
	case 2:	type = MB_ICONEXCLAMATION; break;
	case 3:	type = MB_ICONERROR; break;
	case 4:	type = MB_ICONHAND; break;
	case 5:	type = MB_ICONINFORMATION; break;
	case 6:	type = MB_ICONQUESTION; break;
	case 7:	type = MB_ICONSTOP; break;
	case 8:	type = MB_ICONWARNING; break;
	default:type = 0xFFFFFFFF;	break; // A simple beep.If the sound card is not available, the sound is generated using the speaker.
	}

	if (input[0].value[0]) {
		if( memory[0] == 0 )
			MessageBeep(type);
		memory[0] = 1;
	}
	else
		memory[0] = 0;
	
	return 1;
}
Beep_Component::Beep_Component()
	: Component(L"Beep", L"Beep", squareShape, 2, 1, 0, 1)
{
	setLeftLabels(2, "Trigger", "Type");
	setMemory(0, 0); // edge detection
}


// *************************** Play Sound File ***********************

int PlaySoundFile_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].value[0]) {
		if (memory[0] == 0) {

			if (!input[1].stringType)
				return 0;
			string fileName = input[1].stringValue[0];
			LPWSTR convertedName = ConvertToLPWSTR(input[1].stringValue[0]);
			PlaySound(convertedName, NULL, SND_FILENAME);
		}
		memory[0] = 1;
	}
	else
		memory[0] = 0;

	return 1;
}
PlaySoundFile_Component::PlaySoundFile_Component()
	: Component(L"PlySndFile", L"Play Sound File", squareShape, 2, 1, 0, 1)
{
	setLeftLabels(2, "Trigger", "File Name");
	setMemory(0, 0); // edge detection
}


// *************************** Play Sound Array ***********************


static int put4chars(char* buffer, int k, char A, char B, char C, char D)
{
	buffer[k++] = A;
	buffer[k++] = B;
	buffer[k++] = C;
	buffer[k++] = D;

	return k;
}

static int putLongInt(char* buffer, int k, int input)
{

	buffer[k++] = input & 0xff;
	input /= 256;
	buffer[k++] = input & 0xff;
	input /= 256;
	buffer[k++] = input & 0xff;
	input /= 256;
	buffer[k++] = input & 0xff;

	return k;
}


static int putShortInt(char* buffer, int k, int input)
{

	buffer[k++] = input & 0xff;
	input /= 256;
	buffer[k++] = input & 0xff;

	return k;
}



// returns buffer
static LPCWSTR BuildSoundData(vector<double> inputArray1, vector<double> inputArray2, 
							int channels, int samplesPerSec, int* bytesReturned)
{
	int	dataBytes;
	int	k;
	int	totalBytes;
	int	bytesPerSec;
	int	bitsPerSample;
	int	blockSize;
	int	arrayPoints;

	*bytesReturned = 0;

	arrayPoints = inputArray1.size();
	if( channels == 2)
		if (arrayPoints != inputArray2.size())
			return NULL;
	if (channels > 2)
		return NULL;
	
	bitsPerSample = 16;
	dataBytes = 2 * arrayPoints * channels;

	totalBytes = 44 + dataBytes;
	blockSize = channels * bitsPerSample / 8;
	bytesPerSec = samplesPerSec * blockSize;

	LPCWSTR outputArray = (LPCWSTR)malloc(totalBytes);

	k = put4chars((char*)outputArray, 0, 'R', 'I', 'F', 'F');		// bytes 0 to 3		52 49 46 46
	k = putLongInt((char*)outputArray, k, totalBytes - 8);			// bytes 4 to 7		

	k = put4chars((char*)outputArray, k, 'W', 'A', 'V', 'E');		// bytes 8 to 11	57 41 56 45
	k = put4chars((char*)outputArray, k, 'f', 'm', 't', ' ');		// bytes 12 to 15	66 6D 74 20
	k = putLongInt((char*)outputArray, k, 16);						// bytes 16 to 19	10 00 00 00

	k = putShortInt((char*)outputArray, k, WAVE_FORMAT_PCM);		// bytes 20 to 21	01 00
	k = putShortInt((char*)outputArray, k, channels);				// bytes 22 to 23	01 00
	k = putLongInt((char*)outputArray, k, samplesPerSec);			// bytes 24 to 27	22 56 00 00 (11025?)
	k = putLongInt((char*)outputArray, k, bytesPerSec);			// bytes 28 to 31	44 AC 00 00
	k = putShortInt((char*)outputArray, k, blockSize);				// bytes 32 to 33	02 00
	k = putShortInt((char*)outputArray, k, bitsPerSample);			// bytes 34 to 35	10 00

	k = put4chars((char*)outputArray, k, 'd', 'a', 't', 'a');		// bytes 36 to 39	64 61 74 61
	k = putLongInt((char*)outputArray, k, dataBytes);				// bytes 40 to 44   80 03 00 00

	// add the wave form data
	for (int j = 0; j < arrayPoints; j++) {
		k = putShortInt((char*)outputArray, k, (int)inputArray1[j]);
		if (channels == 2)
			k = putShortInt((char*)outputArray, k, (int)inputArray2[j]);
	}
	
	*bytesReturned = totalBytes;
	return outputArray;
}




int ArrayToSound_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	int channels = setting[0];
	if( (channels != 1) && (channels != 2))
		return clearOutputs();

	int ratePin = channels;
	int gatePin = channels + 1;

	bool startSound = false;
	bool stopSound = false;

	if (input[gatePin].value[0]) {
		if (memory[0] == 0) {
			startSound = true;
		}
		memory[0] = 1;
	}
	else {
		if (memory[0] == 1) {
			stopSound = true;
		}
		memory[0] = 0;
	}

	if( setting[1] == 0) // play once
		if (!startSound)
			return 1;
	
	if (setting[1] == 1) { // continuous play
		if (stopSound)
			PlaySound(NULL, 0, 0);
		if (!startSound)
			return 1;
	}

	int arraySize = input[0].value.size();
	if (arraySize < 10)
		return 1;
	if (arraySize > 100000)
		return 1;
	if (channels == 2) {
		if (arraySize != input[1].value.size())
			return 1;
	}

	int samplesPerSecond = getLoopingValueD(ratePin, 0, 44000);
	if (samplesPerSecond < 100)
		return 1;
	if (samplesPerSecond > 88000)
		return 1;

	int bytes = 0;
	LPCWSTR buffer;
	
	if( channels == 1)
		buffer = BuildSoundData(input[0].value, { 0 }, 1, samplesPerSecond, &bytes);
	else
		buffer = BuildSoundData(input[0].value, input[1].value, 2, samplesPerSecond, &bytes);

	if (bytes != 0) {
		if( setting[1] == 0 ) // play once
			PlaySound(buffer, 0, SND_MEMORY | SND_NODEFAULT | SND_SYNC);
		else // continuous play
			PlaySound(buffer, 0, SND_MEMORY | SND_NODEFAULT | SND_ASYNC | SND_LOOP);
	}

	if (buffer)
		free((char*)buffer); // think about keeping this around!!
	
	return 1;
}

void ArrayToSound_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[1] = radioValue;
	if (boolValues[0])
		setting[0] = 2;
	else
		setting[0] = 1;
	onLoad(pg);
}
void doubleClickArrayToSound(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels = { "Stereo Inputs" };
	vector<bool> checkValues = { pg->dgrm.cL[i]->setting[0] == 2 };

	vector<string> radioLabels = { "Trigger Mode", "Gate Mode" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*> ArrayToSoundButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickArrayToSound) };

bool ArrayToSound_Component::onLoad(Sheet* pg) {

	if (setting.size() < 2)
		setting.resize(2);

	if (setting[0] == 1) {
		height = pinToHeight(3);
		buildInputPins(3);

		if (setting[1])
			setLeftLabels(3, "Array", "Sample Rate", "Gate");
		else
			setLeftLabels(3, "Array", "Sample Rate", "Trigger");
	}
	if (setting[0] == 2) {
		height = pinToHeight(4);
		buildInputPins(4);

		if (setting[1])
			setLeftLabels(3, "Left Array", "Right Array", "Sample Rate", "Gate");
		else
			setLeftLabels(3, "Left Array", "Right Array", "Sample Rate", "Trigger");
	}
	return true;
}
ArrayToSound_Component::ArrayToSound_Component()
	: Component(L"Arr2Sound", L"Array to Sound", squareShape, 3, 1, 0, 1)
{
	dialogButtonList = ArrayToSoundButtonList;
	setMemory(0, 0); // Play edge detection
	setSetting(0, 1); // channels
	setSetting(1, 0); // mode: 0 play once then stop, 1 play until gate goes low
	width = 40;
}


// ***************************** Harmonic Generator *******************************


int HarmonicGenerator_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	int loops = inputsToLoopsN(3); // just fundamental, amplitude and duration
	if (loops == 0)
		return clearOutputs();

	int rate = getLoopingValueD(4, 0, 44000); //  Sample Rate
	if (rate < 100)
		return clearOutputs();
	if (rate > 88000)
		return clearOutputs();

	double totalDuration = 0;
	double duration;
	for (int L = 0; L < loops; L++) {
		duration = getLoopingValueD(2, L, 1); // Duration
		if (duration > 60)
			return clearOutputs();
		if (duration > 0)
			totalDuration += duration;
	}

	if (totalDuration == 0)
		return clearOutputs();

	bool harmonicConnected = false;
	int harmonicCount = 1;
	if (input[3].drivenByComponent != -1) {
		harmonicCount = input[3].value.size(); // Harmonics
		if (harmonicCount > 20)
			return clearOutputs();
		harmonicConnected = true;
	}

	int samples = rate * totalDuration;
	output[0].value.resize(samples);

	double fundamental;
	double amplitude;
	double harmFactor[20] = { 0 };
	double harmAmp[20] = { 0 };
	double angle;
	double sum;
	int subSamples;

	int outk = 0;
	for( int L = 0; L < loops; L++ ) {

		fundamental = getLoopingValueD(0, L, 440); // Fundamental
		if (fundamental <= 0)
			fundamental = 0;
		amplitude = getLoopingValueD(1, L, 10000); // Amplitude
		if (amplitude <= 0)
			amplitude = 0;
		duration = getLoopingValueD(2, L, 1); // Duration
		if (duration <= 0)
			duration = 0;
	
		if (harmonicConnected) {
			for (int k = 0; k < harmonicCount; k++) {
				harmFactor[k] = (k + 1) * fundamental * TWO_PI / rate;
				harmAmp[k] = amplitude * getLoopingValueD(3, k, 0);
			}
		}else{
			harmFactor[0] = fundamental * TWO_PI / rate;
			harmAmp[0] = amplitude;
		}

		subSamples = rate * duration;
		for (int j = 0; j < subSamples; j++) {
			sum = 0;
			for (int k = 0; k < harmonicCount; k++) {
				angle = outk * harmFactor[k];
				sum += harmAmp[k] * sin(angle);
			}
			if( outk < samples)
				output[0].value[outk++] = sum;
		}
	}

	return 1;
}
HarmonicGenerator_Component::HarmonicGenerator_Component()
	: Component(L"HarmGen", L"Harmonic Gen", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(2, "Fundamental", "Amplitude", "Duration", "Harmonics", "Sample Rate" );
	setSetting(0, 1); // channels
	setSetting(1, 0); // mode: 0) step function
	width = 40;
}



// ***************************** Harmonic Generator 2 *******************************


int HarmonicGenerator2_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	output[0].value.resize(0);

	int harmCount = setting[1];	
	int ratePin = harmCount + 1;
	int durationPin = ratePin + 1;

	int rate = getLoopingValueD(ratePin, 0, 44000);
	if (rate < 100)
		return 1;
	if (rate > 88000)
		return 1;

	double duration = getLoopingValueD(durationPin, 0, 1);
	if (duration <= 0)
		return 1;
	if (duration > 20)
		return 1;

	int samples = rate * duration;
	
	int fundSize = input[0].value.size();
	if (fundSize == 0)
		return 1;
	double fundSteps = samples / fundSize;

	int harmSize[20];
	double harmSteps[20];
	for (int k = 0; k < harmCount; k++) {
		harmSize[k] = input[1 + k].value.size();
		if (harmSize[k] == 0)
			return 1;
		harmSteps[k] = samples / harmSize[k];
	}
	
	output[0].value.resize(samples);
	
	double factor[20] = { 0 };
	double amplitude[20] = { 0 };
	double fundamental;
	double angle;
	double sum;
	int fundIndex = 0;
	int harmIndex[20] = { 0 };

	output[0].value[0] = 0;
	for (int j = 1; j < samples; j++) {

		if (fundIndex * fundSteps < j) {
			fundIndex++;
			fundamental = getLoopingValueD(0, fundIndex-1, 0);

			for (int k = 0; k < harmCount; k++) 
				factor[k] = (k + 1) * fundamental * TWO_PI / rate;
		}
		for (int k = 0; k < harmCount; k++) {
			if ((harmIndex[k] * harmSteps[k] < j) && (harmIndex[k] <= harmSteps[k])) {
				harmIndex[k]++;
				factor[k] = (k + 1) * fundamental * TWO_PI / rate;
				amplitude[k] = getLoopingValueD(1 + k, harmIndex[k]-1, 0);
			}
		}

		sum = 0;
		for (int k = 0; k < harmCount; k++) {
			angle = j * factor[k];
			sum += amplitude[k] * sin(angle);
		}
		output[0].value[j] = sum;
	}

	return 1;
}
void HarmonicGenerator2_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[0] < 0)
		return;
	if (doubleValues[0] > 20)
		return;

	setting[1] = doubleValues[0];
	onLoad(pg);
}
void doubleClickHarmonicGenerator2(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Harmonic Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1] };

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*> HarmonicGenerator2ButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickHarmonicGenerator2) };

bool HarmonicGenerator2_Component::onLoad(Sheet* pg) {

	int n = setting[1];
	height = pinToHeight(1+n+2);

	buildInputPins(1 + n + 2);

	int k = 0;
	input[k++].label = "Fundamental";
	for (int j = 0; j < n; j++)
		input[k++].label = "Harmonic " + tomString(j);
	input[k++].label = "Sample Rate";
	input[k++].label = "Duration";

	return true;
}
HarmonicGenerator2_Component::HarmonicGenerator2_Component()
	: Component(L"HarmGen2", L"Harmonic Gen 2", squareShape, 6, 1, 1, 1)
{
	//setLeftLabels(2, "Fundamental", "Harmonic 0", "Harmonic 1", "Harmonic 2", "Sample Rate", "Duration");
	dialogButtonList = HarmonicGenerator2ButtonList;
	setSetting(0, 1); // channels
	setSetting(1, 3); // harmonics - 10 at most
	width = 40;
}


// ************************* Audio waves **********************************

int AudSine_Component::method2(Sheet* pg) {
	return 1;
}
AudSine_Component::AudSine_Component()
	: Component(L"AudSine", L"Audio Sine", squareShape, 3, 1, 0, 1)
{
	setLeftLabels(3, "Gate", "Frequency", "Amplitude");
}
int AudSquare_Component::method2(Sheet* pg) {
	return 1;
}
AudSquare_Component::AudSquare_Component()
	: Component(L"AudSquare", L"Audio Square", squareShape, 3, 1, 0, 1)
{
	setLeftLabels(3, "Gate", "Frequency", "Amplitude");
}
int AudSawtooth_Component::method2(Sheet* pg) {
	return 1;
}
AudSawtooth_Component::AudSawtooth_Component()
	: Component(L"AudSawtooth", L"Audio Sawtooth", squareShape, 3, 1, 0, 1)
{
	setLeftLabels(3, "Gate", "Frequency", "Amplitude");
}
int AudTriangle_Component::method2(Sheet* pg) {
	return 1;
}
AudTriangle_Component::AudTriangle_Component()
	: Component(L"AudTriangle", L"Audio Triangle", squareShape, 3, 1, 0, 1)
{
	setLeftLabels(3, "Gate", "Frequency", "Amplitude");
}
int AudWave_Component::method2(Sheet* pg) {
	return 1;
}
AudWave_Component::AudWave_Component()
	: Component(L"AudWave", L"Audio Wave", squareShape, 3, 1, 0, 1)
{
	setLeftLabels(3, "Gate", "Frequency", "Amplitude");
}


