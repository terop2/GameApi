#!/bin/bash
(cd hssynth2/synthlib;cmake .)
(cd hssynth2/synthlib;make -j 8)
(cd hssynth2/midiplay;cmake .)
(cd hssynth2/midiplay;make -j 8)
#(cd hssynth2/guisynth;cmake .)
#(cd hssynth2/guisynth;make -j 8)
(cd hssynth2/midiplay/midifile;cmake .)
(cd hssynth2/midiplay/midifile;make -j 8)
(cd hssynth2/midiplay/emscripten;cmake .)
(cd hssynth2/midiplay/emscripten;make -j 8)
#(cd hssynth2/synthlib/emscripten;cmake .)
#(cd hssynth2/synthlib/emscripten;make -j 8)