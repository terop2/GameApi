# 
# Copyright (C) 2009 Tero Pulkkinen
# Released under LGPL License.
#
# This file is part of Polygon.
#
# Polygon is free software: you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Polygon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
#


CFiles=	Pieces.cc Map.cc VectorTools.cc Effect.cc Buffer.cc \
	Intersect.cc Bitmap.cc Effect2.cc Shader.cc VolumeObjects.cc \
	Tree.cc Event.cc ObjectCreator.cc IntersectObject.cc Graph.cc \
	Widgets.cc Category.cc Plane.cc Coordinates.cc Triangle.cc Game.cc \
	Editor.cc Web.cc Functions.cc ShaderExpr.cc Font.cc FontEditor.cc \
	GameApi.cc GameApiTest.cc Games.cc HierarchyObject.cc Physics.cc \
        GameApiTest2.cc State.cc Serialize.cc KeyFrameEditor.cc Parser.cc \
	FreeType.cc VertexArray.cc StateChange.cc DistanceObject.cc GameRunner.cc \
	GameApi_an.cc GameApi_bm.cc GameApi_co.cc GameApi_dr.cc GameApi_ev.cc \
	GameApi_ex.cc GameApi_fbo.cc GameApi_f.cc GameApi_fn.cc \
	GameApi_fnt.cc GameApi_gr.cc GameApi_gui.cc GameApi_lay.cc \
	GameApi_li.cc GameApi_main.cc GameApi_mx.cc GameApi_pc.cc \
	GameApi_phy.cc GameApi_pla.cc GameApi_pl.cc GameApi_pt.cc \
	GameApi_pts.cc GameApi_sh.cc GameApi_shm.cc GameApi_sm.cc \
        GameApi_spa.cc GameApi_sp.cc GameApi_st.cc GameApi_su.cc \
        GameApi_tex.cc GameApi_tr.cc GameApi_trk.cc GameApi_tx.cc \
        GameApi_vbo.cc GameApi_ve.cc GameApi_vo.cc GameApi_wmod.cc \
        GameApi_wv.cc GameApi_vx.cc GameApi_cut.cc GameApi_in.cc

CFiles2 = Main.cc

CFiles_examples = GameApiTest3.cc GameApiTest4.cc GameApiWorld.cc GameApiTest5.cc GameApiTest6.cc GameApiTest7.cc GameApiTest8.cc GameApiTest9.cc GameApiTestA.cc
CFiles2_examples = ExampleMain.cc
Subdirs = 
#module1

OFiles = $(patsubst %.cc,objs/%.o,$(CFiles) $(CFiles2))
O2Files = $(patsubst %.cc,objs/%.obj,$(CFiles) $(CFiles2))
DFiles = $(patsubst %.cc,objs/%.d,$(CFiles) $(CFiles2)) 
D2Files = $(patsubst %.cc,objs/%.d2,$(CFiles) $(CFiles2)) 
HFiles = $(patsubst %.cc,%.hh,$(CFiles))
#GCHFiles = $(patsubst %.cc,objs/%.hh.gch,$(CFiles))
#GCH2Files = $(patsubst %.cc,objs/%.hh.gch2,$(CFiles))
Subdirsx = $(patsubst %,%build,$(Subdirs))

OFiles_examples = $(patsubst %.cc,objs/%.o,$(CFiles_examples) $(CFiles2_examples))
O2Files_examples = $(patsubst %.cc,objs/%.obj,$(CFiles_examples) $(CFiles2_examples))
DFiles_examples = $(patsubst %.cc,objs/%.d,$(CFiles_examples) $(CFiles2_examples)) 
D2Files_examples = $(patsubst %.cc,objs/%.d2,$(CFiles_examples) $(CFiles2_examples)) 
HFiles_examples = $(patsubst %.cc,%.hh,$(CFiles_examples))
#GCHFiles_examples = $(patsubst %.cc,objs/%.hh.gch,$(CFiles_examples))
#GCH2Files_examples = $(patsubst %.cc,objs/%.hh.gch2,$(CFiles_examples))

LIBWIN = /home/terop/cvs/sdl2_win/SDL2-2.0.1/i686-w64-mingw32/bin
LIB2WIN = /home/terop/cvs/sdl2_win/
LIB3WIN = /home/terop/cvs/sdl2_win/glew-1.10.0/bin/Release/Win32
LIB4WIN = /home/terop/cvs/sdl2_win/lib
LIB5WIN = /home/terop/cvs/sdl2_win/SDL2_image-2.0.0/i686-w64-mingw32/lib
LIB6WIN = /home/terop/cvs/sdl2_win/glew-1.10.0/lib/
INCWIN = /home/terop/cvs/sdl2_win/SDL2-2.0.1/i686-w64-mingw32/include/SDL2
INC2WIN = /home/terop/cvs/sdl2_win/glew-1.10.0/include
INC3WIN = /home/terop/cvs/sdl2_win/SDL2_image-2.0.0/i686-w64-mingw32/include/SDL2
INC4WIN = /home/terop/cvs/sdl2_win/SDL2-2.0.1/i686-w64-mingw32/include/SDL2
INC5WIN = /home/terop/cvs/sdl2_win/include

CCFLAGS = -std=c++11 -fPIC -Wall -O3 -I./objs -I/usr/include/freetype2 -I/usr/include/SDL2 -DRASBERRY
CC2FLAGS = -std=c++11 -Wall -O3 -I./objs -I/usr/include/freetype2 -I$(INCWIN) -I$(INC2WIN) -I$(INC3WIN) -I$(INC4WIN) -I$(INC5WIN) -DGLEW_STATIC -DRASBERRY
#`pkg-config gtkmm-2.4 --cflags`


default: libGameApi.so main

all: libGameApi.so libGameApi.dll install main

examples: main

main: $(OFiles_examples) Makefile GameApi.hh libGameApi.so
	@echo g++ main
	@g++ -g $(OFiles_examples) -std=c++0x -o main -L. -lGameApi -lGL -lm -lstdc++

main.exe: $(O2Files_examples) Makefile GameApi.hh libGameApi.dll
	@echo mingw main.exe
	@$(CCWIN) -g $(O2Files_examples) -o main.exe -L. -lGameApi -lopengl32 -lm -lstdc++


libGameApi.so: $(OFiles) Makefile
	@echo g++ libGameApi.so
	@g++ -shared -g $(OFiles) -std=c++0x -o libGameApi.so -lSDL2 -lGL -lSDL2_image -lSDL2_mixer -lGLEW -lfreetype -lstdc++ 
#`pkg-config gtkmm-2.4 --libs` 

libGameApi.dll: $(O2Files) Makefile glew.o
	@echo mingw libGameApi.dll
	@i586-mingw32msvc-g++ -L$(LIBWIN) -L$(LIB2WIN) -L$(LIB3WIN) -L$(LIB4WIN) -L$(LIB5WIN) -L$(LIB6WIN) -shared -g $(O2Files) -DGLEW_STATIC glew.o -o libGameApi.dll -lSDL2 -lSDL2_image -lfreetype -lstdc++ -lgcc -lwsock32 -lopengl32 

subdirs: $(Subdirs)

$(Subdirs):
	$(MAKE) -C $@

.PHONY: subdirs $(Subdirs)

ctags: TAGS

TAGS: $(CFiles)
	etags -o TAGS -l c++ $(CFiles)

CC = g++
CCWIN = i586-mingw32msvc-g++

$(DFiles): objs/%.d: %.cc
	@gcc -M $(CCFLAGS) $< \
	|sed 's,\($*\)\.o[ :]*,objs/\1.o $@ : ,g' > $@

$(D2Files): objs/%.d2: %.cc
	@$(CCWIN) -M $(CC2FLAGS) $< \
	|sed 's,\($*\)\.obj[ :]*,objs/\1.obj $@ : ,g' > $@

$(OFiles): objs/%.o : %.cc
	@echo g++ $< -o $@
	@$(CC) -std=c++0x -g -c $(CCFLAGS) $< -o $@

$(O2Files): objs/%.obj : %.cc
	@echo mingw $< -o $@
	@$(CCWIN) -g -c $(CC2FLAGS) $< -o $@

glew:
	$(CCWIN) -g -c $(CC2FLAGS) /home/terop/cvs/sdl2_win/glew-1.10.0/src/glew.c -o glew.o


$(OFiles_examples): objs/%.o : %.cc GameApi.hh
	@echo g++ $< -o $@
	@$(CC) -std=c++0x -g -c -I. $(CCFLAGS) $< -o $@ 

$(O2Files_examples): objs/%.obj : %.cc GameApi.hh
	@echo mingw $< -o $@
	@$(CCWIN) -g -c -I. $(CC2FLAGS) $< -o $@ 


#$(GCHFiles): objs/%.hh.gch : %.hh
#	@echo g++ $< -o $@
#	@$(CC) $(CCFLAGS) -std=c++0x -g -x c++-header $< -o $@ 
#
#$(GCH2Files): objs/%.hh.gch2 : %.hh
#	@echo mingw $< -o $@
#	@$(CCWIN) $(CC2FLAGS) -g -x c++-header $< -o $@ 

#$(GCHFiles_examples): objs/%.hh.gch : %.hh GameApi.hh
#	@echo g++ $< -o $@
#	@$(CC) $(CCFLAGS) -std=c++0x -g -I. -x c++-header $< -o $@ 
#
clean:
	@rm -rf objs/*.o objs/*.obj objs/*.d objs/*.d2 objs/*.gch objs/*.gch2
	@rm main
	@echo Clean.

profile:
	rm profile.txt
	gprof ./main ./gmon.out >profile.txt


make_sourcedist:
	rm -f GameApi.tar.gz
	rm -rf GameApi
	mkdir GameApi
	cp *.cc *.hh GameApi/
	cp Makefile GameApi/
	cp Makefile_bindist GameApi/
	cp COPYING.LESSER GameApi/
	cp README.INSTALL GameApi/
	mkdir GameApi/objs
	tar czvf GameApi.tar.gz GameApi
	rm -rf GameApi

make_binarydist:
	rm -f GameApi-binary.tar.gz
	rm -rf GameApiBinary
	mkdir GameApiBinary
	cp libGameApi.so GameApiBinary/
	cp GameApi.hh GameApiBinary/
	cp COPYING.LESSER GameApiBinary/
	cp README.INSTALL GameApiBinary/
	cp Makefile_bindist GameApiBinary/Makefile
	tar czvf GameApi-binary.tar.gz GameApiBinary
	rm -rf GameApiBinary

install:
	make -f Makefile_bindist install


make_commit:
	git commit -a

make_push:
	git push ~/cvs/demo-git-push/demo3d.git master

-include $(DFiles)
-include $(D2Files)
-include $(DFiles_examples)
-include $(D2Files_examples)
