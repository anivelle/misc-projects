# Fun Miscellaneous Projects

Realized it would probably be better to organize all my small projects into one
location. Will likely all be in C but potentially also C++ and other languages I
feel like trying.

## [Writing to nRF52840 flash memory][flashGithub]
### Current progress:
  - Broke my first Arduino Nano 33 BLE, I think I erased a necessary portion
    of memory, possibly the bootloader?
  - On hold for now, but I think I know which part of memory I should have 
      used
### What this is  
  - Just a test on writing data to the flash memory on the Arduino Nano 33 BLE,
    which I am using for my [drone project][drone]. Might use it to write
    configuration values to avoid performing calibration every single time I
    turn the drone on.

## [Fast Fourier Transform Visualizer][fftGithub]
### Current progress:
  - FFT was remarkably easy to implement after writing the pseudocode, though
    I got some help on that from Wikipedia
  - Music plays and visualization is synced as far as I can tell. It appears
    to fit the music in terms of frequencies, and at high numbers of bars it
    is possible to see the symmetry of the Fourier Transform when operating
    on real numbers
  - Still has some visual glitches (some frequencies do not render?) and
    choosing certain numbers of bars can crash the whole program. Need to
    debug this
### What this is
  - Following in the footsteps of [Tsoding][FFT] because the topic is
    interesting and I want to put _some_ of the DSP I've learned to use
  - Diverge from Tsoding in using `libsndfile` and PortAudio for audio handling
    (so I'm not completely copying him), but I will use `raylib` for graphics.
    Probably a convoluted way of doing this but oh well.
  - Not sure how far I'll go with this but it's likely to explore some of the
    possible FFT algorithms depending on how hard they are

## [Ray Tracing with OpenGL][ray-tracing]
### Current progress:
  - Triangle rendered :)
  - Learned about VBOs and VAOs, though more research is necessary to fully
    grasp what they actually are and do
  - Worked with simple shaders
### What this is
  - Exploration of OpenGL with a focus on getting to ray tracing because
    graphics programming (and particularly ray tracing) interests me
  - Not exactly at ray tracing quite yet, as I have not worked with OpenGL much
  - Not sure why people use C++ when OpenGL is very clearly C-oriented but
    presumably C++ makes some data easier to work with
  - Learning from [Learn OpenGL](https://learnopengl.com/) and will probably
    diverge when I get to a point where I feel I can move to ray tracing

[flashGithub]: https://github.com/anivelle/misc-projects/tree/main/write-flash
[fftGithub]: https://github.com/anivelle/misc-projects/tree/main/FFT
[FFT]: https://www.youtube.com/watch?v=Xdbk1Pr5WXU
[drone]: https://github.com/anivelle/drone-controller
[ray-tracing]: https://github.com/anivelle/misc-projects/tree/main/ray-tracing
