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
	case 0: // Sine wave
		return sin(w(dHertz) * dTime);

	case 1: // Square wave
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;

	case 2: // Triangle wave
		return asin(sin(w(dHertz) * dTime)) * (2.0 / PI);

	case 3: // Saw wave (analogue / warm / slow)
	{
		double dOutput = 0;

		for (double n = 1.0; n < 100.0; n++)
			dOutput += (sin(n * w(dHertz) * dTime)) / n;
		
		return dOutput* (2.0 / PI);
	}
	case 4: // Saw wave (optimized / harsh / fast)
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));

	case 5: // Pseudo Random Noise
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;

	default:
		return 0.0; // No sound output
	}
}

atomic<double>  dFrequencyOutput = 0.0;

// Function used by olcNoiseMaker to generate sound waves
// Returns amplitude (-1.0 to +1.0) as a function of time
double MakeNoise(double dTime)
{
	double dOutput = osc(dFrequencyOutput, dTime, 3);

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
