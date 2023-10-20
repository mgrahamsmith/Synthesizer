#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"

// Converts fequency (Hz) to angular velocity
double w(double dHertz)
{
	return dHertz * 2.0 * PI;
}

double osc(double dHertz, double dTime, int nType)
{

	switch (nType)
	{
	case 0: // sine wave
		return sin(w(dHertz) * dTime);

	case 1: // square wave
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;

	case 2: // triangle wave
		return asin(sin(w(dHertz) * dTime) * 2.0 / PI);

	default:
		return 0.0; // No sound output
	}
}

atomic<double>  dFrequencyOutput = 0.0;

// Function used by olcNoiseMaker to generate sound waves
// Returns amplitude (-1.0 to +1.0) as a function of time
double MakeNoise(double dTime)
{
	double dOutput = osc(dFrequencyOutput, dTime, 0);

	return dOutput * 0.4; // Master Volume
}

int main()
{
	wcout << "Synthesizer Console App" << endl;

	// Get all sound hardware
	vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

	// Display findings
	for (auto d : devices) wcout << "Found Output Device:" << d << endl;

	// Create sound machine!
	olcNoiseMaker<short> sound (devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction(MakeNoise);

	double dOctaveBaseFrequency = 110.0; // A2
	double d12thRootOf2 = pow(2.0, 1.0 / 12.0);

	while (1)
	{
		// Add a keyboard like a piano

		bool bKeyPressed = false;
		for (int k = 0; k < 15; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbc\xbe"[k])) & 0x8000)
			{
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				bKeyPressed = true;
			}
		}

		if (!bKeyPressed)
		{
			dFrequencyOutput = 0;
		}
	}


	return 0;
}
