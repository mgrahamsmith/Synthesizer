#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"

// Converts fequency (Hz) to angular velocity
double w(double dHertz)
{
	return dHertz * 2.0 * PI;
}

enum class OSC {
	SINE,
	TRIANGLE,
	SQUARE,
	SAW_ANA,
	SAW_DIG,
	NOISE
};

double osc(double dHertz, double dTime, OSC nType)
{

	switch (nType)
	{
	case OSC::SINE: // Sine wave
		// return sin(w(dHertz) * dTime);
		return sin(w(dHertz) * dTime + 0.5 * dHertz * sin(w(1.0) * dTime));

	case OSC::TRIANGLE: // Triangle wave
		return asin(sin(w(dHertz) * dTime)) * (2.0 / PI);

	case OSC::SQUARE: // Square wave
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;

	case OSC::SAW_ANA: // Saw wave (analogue / warm / slow)
	{
		double dOutput = 0;

		for (double n = 1.0; n < 100.0; n++)
			dOutput += (sin(n * w(dHertz) * dTime)) / n;
		
		return dOutput* (2.0 / PI);
	}
	case OSC::SAW_DIG: // Saw wave (optimized / harsh / fast)
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));

	case OSC::NOISE: // Pseudo Random Noise
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;

	default:
		return 0.0; // No sound output
	}
}

struct sEnvelopeADSR
{
	double dAttackTime;
	double dDecayTime;
	double dReleaseTime;

	double dSustainAmplitude;
	double dStartAmplitude;

	double dTriggerOnTime;
	double dTriggerOffTime;

	bool bNoteOn;

	sEnvelopeADSR()
	{
		dAttackTime = 0.2;
		dDecayTime = 0.5;
		dStartAmplitude = 1.0;
		dSustainAmplitude = 0.8;
		dReleaseTime = 0.1;
		dTriggerOnTime = 0.0;
		dTriggerOffTime = 0.0;
		bNoteOn = false;
	}

	double GetAmplitude(double dTime)
	{
		double dAmplitude = 0.0;
		double dLifeTime = dTime - dTriggerOnTime;

		if (bNoteOn)
		{
			// ADS

			// Attack
			if (dLifeTime <= dAttackTime)
				dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;
			
			// Decay
			if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime + dDecayTime))
				dAmplitude = ((dLifeTime - dAttackTime) / dDecayTime) * (dSustainAmplitude - dStartAmplitude) + dStartAmplitude;
		
			// Sustain
			if (dLifeTime > (dAttackTime + dDecayTime))
				dAmplitude = dSustainAmplitude;
		}
		else
		{
			// Release
			dAmplitude = ((dTime - dTriggerOffTime) / dReleaseTime) * (0.0 - dSustainAmplitude) + dSustainAmplitude;
		}

		// Epsilon value check.  Stop low signals (amplitude near 0.0) from coming out of the speaker.
		if (dAmplitude <= 0.0001)
		{
			dAmplitude = 0.0;
		}

		return dAmplitude;
	}

	void NoteOn(double dTimeOn)
	{
		dTriggerOnTime = dTimeOn;
		bNoteOn = true;
	}

	void NoteOff(double dTimeOff)
	{
		dTriggerOffTime = dTimeOff;
		bNoteOn = false;
	}
};

// Global synthesizer variables
atomic<double>  dFrequencyOutput = 0.0;			// dominant output frequency of instrument, i.e. the note
double dOctaveBaseFrequency = 110.0; // A2		// frequency of octave represented by ... ?
double d12thRootOf2 = pow(2.0, 1.0 / 12.0);     // assuming western 12 notes per octave
sEnvelopeADSR envelope;							// amplitude modulation of output to 


// Function used by olcNoiseMaker to generate sound waves
// Returns amplitude (-1.0 to +1.0) as a function of time
double MakeNoise(double dTime)
{
	double dOutput = envelope.GetAmplitude(dTime) *
		(
			+ osc(dFrequencyOutput * 0.5, dTime, OSC::SQUARE)
		);

	return dOutput * 0.4; // Master Volume
}

int printKeyboard()
{
	wcout << "_________________________________________\n"
		  << "|  | |  |  | | | |  |  | | | | | |  |  | \n"
		  << "|  |S|  |  |F| |G|  |  |J| |K| |L|  |  | \n"
		  << "|  |_|  |  |_| |_|  |  |_| |_| |_|  |  |_\n"
		  << "|   |   |   |   |   |   |   |   |   |   |\n"
		  << "| Z | X | C | V | B | N | M | , | . | / |\n"
		  << "|___|___|___|___|___|___|___|___|___|___|\n\n";

	return 0;
}


int main()
{
	wcout << "Synthesizer Console App" << endl;

	printKeyboard();

	// Get all sound hardware
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

	// Display findings
	for (auto d : devices) wcout << "Found Output Device:" << d << endl;

	// Create sound machine!
	olcNoiseMaker<short> sound (devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction(MakeNoise);

	double dOctaveBaseFrequency = 220.0; // A2
	double d12thRootOf2 = pow(2.0, 1.0 / 12.0);

	int nCurrentKey = -1;
	bool bKeyPressed = false;
	while (1)
	{
		bKeyPressed = false;
		for (int k = 0; k < 16; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
			{
				if (nCurrentKey != k)
				{
					dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
					envelope.NoteOn(sound.GetTime());
					wcout << "\rNote On : " << sound.GetTime() << "s " << dFrequencyOutput << " Hz";
					nCurrentKey = k;
				}

				bKeyPressed = true;
			}
		}

		if (!bKeyPressed)
		{
			if (nCurrentKey != -1)
			{
				wcout << "\rNote Off: " << sound.GetTime() << "s ";
				envelope.NoteOff(sound.GetTime());
				nCurrentKey = -1;
			}
		}
	}

	return 0;
}
