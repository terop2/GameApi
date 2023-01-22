cd hssynth2/synthlib
cmake . -G "MinGW Makefiles"
make -j 8
cd ../..
cd hssynth2/midiplay
cmake . -G "MinGW Makefiles"
make -j 8
cd ../..
cd hssynth2/midiplay/midifile
cmake . -G "MinGW Makefiles"
make -j 8
cd ../../..
