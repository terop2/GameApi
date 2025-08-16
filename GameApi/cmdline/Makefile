
BD_CALVIN=$(shell uname -n)

BD_CALVIN_M64= 
ifneq ($(BD_CALVIN),fedora)
BD_CALVIN_M64+=-m64
endif

all:
	@echo "  LD   cmdline/gameapi_cmdline"
	@g++ $(BD_CALVIN_M64) test.cpp -o gameapi_cmdline -I.. -L.. -lGameApi

#@echo "  LD   cmdline/gameapi_cmdline.small"
#@g++ -m64 test.cpp -o gameapi_cmdline.small -I.. -L.. -lGameApi_small
