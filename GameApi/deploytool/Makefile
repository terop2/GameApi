
ADDRESS=
ifeq ($(DEBUG),address)
ADDRESS=-fsanitize=address
endif

all:
	@echo "  LD     deploytool/deploytool"
	@g++ $(ADDRESS) main.cc -g -o deploytool -I.. -L.. -lGameApi

#-L../hssynth2/synthlib -lsynthlib -L../hssynth2/midiplay/midifile -lmidifile -lasound
