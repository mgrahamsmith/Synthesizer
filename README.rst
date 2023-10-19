================================================================================
Synthesizer
================================================================================

A simple audio synthesizer with a command line interface, written in C++, and compiled 
with MSVC.

This video_ by javidx9_ inspired the project.  See the `OLC Synth Github Project`_ 
for reference.

.. _video: https://www.google.com
.. _javidx9: https://www.youtube.com/@javidx9
.. _Developer Command Prompt for VS 2022: https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022
.. _OLC Synth Github Project: https://github.com/OneLoneCoder/synth/tree/master


Building and Running
--------------------------------------------------------------------------------

Instructions for building and running the program:

#. Open `Synthesizer.sln`_ in Visual Sutdio 2022.  
#. Make sure `x86` is selected in the drop down just to the left of the "Local Windows Debugger" button.

.. _Synthesizer.sln: ./Synthesizer.sln



Future Implementations
--------------------------------------------------------------------------------

Guitar String simulation
^^^^^^^^^^^^^^^^^^^^^^^^

* *Guitar string simulation pseudo-code:*
* sampleRate
* each sample, send speaker a number representing diaphram position.
* For every string that was plucked, create a ring buffer.
* Ring buffer:
    * When you get to the end of the array, jump back to the beginning.
* Every time you pluck a string, it creates a new ring buffer and sets it to whatever lenght needed for the note being plucked.
* Go through each element of ring buffer once per sample.
* ringBuf[200] is filled with random values between 1 and -1.
* output 1 element of ringBuf per sample.
* Every 200 samples it repeats.
* For A, make the note long enough to repeat every 440 seconds.
* Determined by the sample rate.
* Go back and mulitply every ringBuf element by 0.9 and it will fade to 0 after enough iterations.
