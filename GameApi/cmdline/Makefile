
BD_CALVIN=$(shell uname -n)

BD_CALVIN_M64= 
ifneq ($(BD_CALVIN),fedora)
ifneq ($(BD_CALVIN),raspberrypi)
BD_CALVIN_M64+=-m64
endif
endif

ADDRESS=
ifeq ($(DEBUG),address)
ADDRESS=-fsanitize=address
endif

STEAM_DEPLOY_ENABLE ?= no
STEAM_PATH ?= /home/terop/cvs/steam/sdk

ifeq ($(STEAM_DEPLOY_ENABLE),yes)
STEAM_APPID ?= 4181720
STEAM_CFLAGS=-DSTEAM -DSTEAM_DEPLOY -I$(STEAM_PATH)/public -DSTEAM_APP_ID=$(STEAM_APPID)
STEAM_LIBS=-L$(STEAM_PATH)/redistributable_bin/linux64 -lsteam_api
else
STEAM_CFLAGS=
STEAM_LIBS=
endif


all:
	@echo "  LD   cmdline/gameapi_cmdline"
	@g++ $(ADDRESS) $(BD_CALVIN_M64) test.cpp -o gameapi_cmdline -I.. -L.. -lGameApi $(STEAM_CFLAGS) $(STEAM_LIBS)

#@echo "  LD   cmdline/gameapi_cmdline.small"
#@g++ -m64 test.cpp -o gameapi_cmdline.small -I.. -L.. -lGameApi_small
