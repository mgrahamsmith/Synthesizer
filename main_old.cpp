#include <list>
#include <iostream>
#include <algorithm>
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

double osc(double dHertz, double dTime, OSC nType, double dLFOHertz = 0.0, double dLFOAmplitude = 0.0)
{
	double dFreq = w(dHertz) * dTime + dLFOAmplitude * dHertz * sin(w(dLFOHertz) * dTime); // frequency modulated

	switch (nType)
	{
	case OSC::SINE: // Sine wave
		// return sin(w(dHertz) * dTime);
		return sin(dFreq);

	case OSC::TRIANGLE: // Triangle wave
		return asin(sin(dFreq)) * (2.0 / PI);

	case OSC::SQUARE: // Square wave
		return sin(dFreq) > 0.0 ? 1.0 : -1.0;

	case OSC::SAW_ANA: // Saw wave (analogue / warm / slow)
	{
		double dOutput = 0.0;

		for (double n = 1.0; n < 40.0; n++)
			dOutput += (sin(n * dFreq)) / n;
		
		return dOutput* (2.0 / PI);
	}

	// TODO: since Hertz are used here in real time to approximate the saw wave,
	// instead of angular velocity, we can't use dFreq here as in the others, so
	// no frequency modulation inputs have been handled yet.
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
		dAttackTime = 0.01;
		dDecayTime = 1.0;
		dStartAmplitude = 1.0;
		dSustainAmplitude = 0.0;
		dReleaseTime = 1.0;
		bNoteOn = false;
		dTriggerOnTime = 0.0;
		dTriggerOffTime = 0.0;
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

struct instrument
{
	double dVolume;
	sEnvelopeADSR env;
	virtual double sound(double dTime, double dFrequency) = 0;
};

struct bell : public instrument
{
	bell()
	{
		env.dAttackTime = 0.01;
		env.dDecayTime = 1.0;
		env.dStartAmplitude = 1.0;
		env.dSustainAmplitude = 0.0;
		env.dReleaseTime = 1.0;
	}

	double sound(double dTime, double dFrequency)
	{
		double dOutput = env.GetAmplitude(dTime) *
			(
			1.0 * osc(dFrequency * 2.0, dTime, OSC::SINE, 5.0, 0.001)
			+ 0.5 * osc(dFrequency * 3.0, dTime, OSC::SINE)
			+ 0.25 * osc(dFrequency * 4.0, dTime, OSC::SINE)
				);

		return dOutput;
	}
};

struct harmonica : public instrument
{
	harmonica()
	{
		env.dAttackTime = 0.01;
		env.dDecayTime = 1.0;
		env.dStartAmplitude = 1.0;
		env.dSustainAmplitude = 0.0;
		env.dReleaseTime = 1.0;
	}

	double sound(double dTime, double dFrequency)
	{
		double dOutput = env.GetAmplitude(dTime) *
			(
				1.0 * osc(dFrequency, dTime, OSC::SQUARE, 5.0, 0.001)
				+ 1.0 * osc(dFrequency * 1.5, dTime, OSC::SQUARE)
				+ 1.0 * osc(dFrequency * 2.0, dTime, OSC::SQUARE)
				+ 0.05 * osc(0, dTime, OSC::NOISE)
				);
		
		return dOutput;
	}
};


// Global synthesizer variables
atomic<double>  dFrequencyOutput = 0.0;			// dominant output frequency of instrument, i.e. the note
sEnvelopeADSR envelope;							// amplitude modulation of output to 
double dOctaveBaseFrequency = 110.0; // A2		// frequency of octave represented by ... ?
double d12thRootOf2 = pow(2.0, 1.0 / 12.0);     // assuming western 12 notes per octave

instrument* voice = nullptr;

// Function used by olcNoiseMaker to generate sound waves
// Returns amplitude (-1.0 to +1.0) as a function of time
double MakeNoise(double dTime)
{
	double dOutput = voice->sound(dTime, dFrequencyOutput);
	return dOutput * 0.4; // Master Volume
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

	voice = new bell();

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
					voice->env.NoteOn(sound.GetTime());
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
				voice->env.NoteOff(sound.GetTime());
				nCurrentKey = -1;
			}
		}
	}

	return 0;
}
