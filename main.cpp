#include <iostream>
using namespace std;

#include "olcNoiseMaker.h"

double MakeNoise(double dTime)
{
    return 0.5 * sin(440.0 * 2 * 3.14159 * dTime);
}

int main()
{
    wcout << "Synthesizer Part 1" << endl;

    // Get all sound hardware
    vector<wstring> devices = olcNoiseMaker<short>::Enumerate();

    // Display findings
    for (auto d : devices)
    {
        wcout << "Found Output Device:" << d << endl;
    }

    // Create sound machine!!
    // sound( devices, sample rate, channels, 8, 512)
    //   channels: mono vs. stereo.  Assume 1 speaker.
    //   8, 512: latency management.  Reduce delay between key stroke and sound.
    olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

    // Link noise function with sound machine
    sound.SetUserFunction(MakeNoise);

    while (1)
    {

    }

    return 0;
}