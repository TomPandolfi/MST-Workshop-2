#pragma once



#define NEW_MIDISEND_DIALOG 9943


#define NEW_MIDISEND_OK	 		 1
#define	NEW_MIDISEND_CANCEL		 2
#define NEW_MIDISEND_DELETE		 3
#define	NEW_MIDISEND_TEXT1		 10
#define NEW_MIDISEND_TRIGGER	 11
#define	NEW_MIDISEND_TEXT2		 12
#define NEW_MIDISEND_CHANNEL	 13
#define	NEW_MIDISEND_TEXT3		 14
#define NEW_MIDISEND_PROGRAM	 15
#define	NEW_MIDISEND_TEXT4		 16
#define NEW_MIDISEND_PORT		 17


class AudSine_Component : public Component {
public:	AudSine_Component();
	int method2(Sheet* pg);
};
class AudSquare_Component : public Component {
public:	AudSquare_Component();
	int method2(Sheet* pg);
};
class AudSawtooth_Component : public Component {
public:	AudSawtooth_Component();
	int method2(Sheet* pg);
};
class AudTriangle_Component : public Component {
public:	AudTriangle_Component();
	int method2(Sheet* pg);
};
class AudWave_Component : public Component {
public:	AudWave_Component();
	int method2(Sheet* pg);
};

class PlayMidi_Component : public Component {
public:	PlayMidi_Component();
	int method2(Sheet* pg);
};

class Beep_Component : public Component {
public:	Beep_Component();
	int method2(Sheet* pg);
}; 

class PlaySoundFile_Component : public Component {
public:	PlaySoundFile_Component();
	int method2(Sheet* pg);
};

class ArrayToSound_Component : public Component {
public:	ArrayToSound_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
	bool onLoad(Sheet*);
};

class HarmonicGenerator_Component : public Component {
public:	HarmonicGenerator_Component();
	int method2(Sheet* pg);
};

class HarmonicGenerator2_Component : public Component {
public:	HarmonicGenerator2_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
	bool onLoad(Sheet*);
};
