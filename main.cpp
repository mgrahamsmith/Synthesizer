#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"

atomic<double>  dFrequencyOutput = 0.0;

double MakeNoise(double dTime)
{
	double dOutput = 1.0 * sin(220.0 * 2 * 3.14159 * dTime);

	if (dOutput > 0.0)
		return 0.2;
	else
		return -0.2;

	return 0.3 * sin(220.0 * 2 * 3.14159 * dTime);
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


	while (1)
	{
		// Add a keyboard

		if (GetAsyncKeyState('A') & 0x8000)
		{

		}
	}


	return 0;
}
