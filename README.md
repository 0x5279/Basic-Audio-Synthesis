The code in main.cpp is only for testing purposes, and can be removed.
Only the contents of the wav directory are needed for the lib to function.

There are 4 different datatypes in wave.h:

wave:
a wrapper for the main oscillator, or wave::Wave.
allows for sampling, and gives more room to expand on in the future.

the struct wave::Wave itself consists of:
 - wave::WaveType type (can be SAW, TRIANGLE, SQUARE, or SINE)
   
 - float pitch
   - can represent frequency or a factor, more on that in the cwave section
     
 - float amplitude
   - represents volume from 0 - 1. KEEP THIS LOW, A VOLUME OF 1 WILL BE VERY LOUD.
     
 - float phase
   - represents the phase of a wave. theoretically could be anything,
       but it's good practice to keep it between 0 - 1.

Once a wave is fully defined, calling wave.sample(t) (t being seconds) 
  will return the value of the wave function at that time.

cwave:
stands for compound wave, consists of multiple child wave functions and one base wave function.

cwaves can be sampled in much the same way as normal waves, but have a few key differences:
 - the pitch of child waves is a factor applied to a base wave.
     (ex. if the base wave has a pitch of 220hz, a child with a pitch of 2
       will have an absolute pitch of (220 * 2), or 440 hz.)
       This is to make harmonics parametric, so changing the base wave changes the children accordingly.

 - the amplitudes of the child waves are relative to the base wave.
     (ex. if the base wave has an amplitude of 0.1, a child with an amplitude of 1
       will have an absolute amplitude of 0.1 as well.)

 - at the end, the base wave and child wave samples are accumulated and passed as one sample.
     This can allow for emulation of different instruments, chords,
     and can even roughly emulate speech with enough patience.

twave:
a time-dependent wrapper for the cwave.

on top of the cwave itself, it contains a start time and end time.

but you can also just initialize it with only a start time and set a duration with the
twave.setDuration(t) function.

can also be sampled in the same way as cwave, but is dependent on t being between the start and end
of the twave.

waveformat:
a collection of twaves that can then be compiled and written to a .wav file.
Bit depth and sample rate can both be set, and you can add or remove twaves at your discretion, 
before compilation.

once all the twaves are in place and you are happy with your bit depth and sample rate,
you can use the waveformat.compile() function. This will make a readable set of samples.
From there, you can write your samples to a .wav file, where they can then be stored indefinitely.

For further information, feel free to look at the source code itself. The documentation should 
be easy enough to follow, and if a function is defined in a separate file, the file it is defined in
will always be titled {{datatype}}.cpp.
