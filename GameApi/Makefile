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
	FreeType.cc VertexArray.cc StateChange.cc DistanceObject.cc

CFiles2 = Main.cc

CFiles_examples = GameApiTest3.cc GameApiTest4.cc GameApiWorld.cc GameApiTest5.cc GameApiTest6.cc GameApiTest7.cc GameApiTest8.cc GameApiTest9.cc GameApiTestA.cc
CFiles2_examples = ExampleMain.cc
Subdirs = 
#module1

OFiles = $(patsubst %.cc,objs/%.o,$(CFiles) $(CFiles2))
DFiles = $(patsubst %.cc,objs/%.d,$(CFiles) $(CFiles2)) 
HFiles = $(patsubst %.cc,%.hh,$(CFiles))
GCHFiles = $(patsubst %.cc,objs/%.hh.gch,$(CFiles))
Subdirsx = $(patsubst %,%build,$(Subdirs))

OFiles_examples = $(patsubst %.cc,objs/%.o,$(CFiles_examples) $(CFiles2_examples))
DFiles_examples = $(patsubst %.cc,objs/%.d,$(CFiles_examples) $(CFiles2_examples)) 
HFiles_examples = $(patsubst %.cc,%.hh,$(CFiles_examples))
GCHFiles_examples = $(patsubst %.cc,objs/%.hh.gch,$(CFiles_examples))

CCFLAGS = -fPIC -Wall -O3 -I./objs -I/usr/include/freetype2
#`pkg-config gtkmm-2.4 --cflags`

default: libGameApi.so main

all: libGameApi.so install main

examples: main

main: $(GCHFiles_examples) $(OFiles_examples) Makefile GameApi.hh libGameApi.so
	@echo g++ main
	@g++ -g $(OFiles_examples) -std=c++0x -o main -L. -lGameApi -lGL -lm -lstdc++

libGameApi.so: $(GCHFiles) $(OFiles) Makefile
	@echo g++ libGameApi.so
	@g++ -shared -g $(OFiles) -std=c++0x -o libGameApi.so -lSDL2 -lGL -lSDL2_image -lGLEW -lfreetype -lstdc++ 
#`pkg-config gtkmm-2.4 --libs` 

subdirs: $(Subdirs)

$(Subdirs):
	$(MAKE) -C $@

.PHONY: subdirs $(Subdirs)

ctags: TAGS

TAGS: $(CFiles)
	etags -o TAGS -l c++ $(CFiles)

CC = g++

$(DFiles): objs/%.d: %.cc
	@gcc -M $(CCFLAGS) $< \
	|sed 's,\($*\)\.o[ :]*,objs/\1.o $@ : ,g' > $@

$(OFiles): objs/%.o : %.cc
	@echo g++ $< -o $@
	@$(CC) -std=c++0x -g -c $(CCFLAGS) $< -o $@

$(OFiles_examples): objs/%.o : %.cc GameApi.hh Makefile
	@echo g++ $< -o $@
	@$(CC) -std=c++0x -g -c -I. $(CCFLAGS) $< -o $@ 


$(GCHFiles): objs/%.hh.gch : %.hh
	@echo g++ $< -o $@
	@$(CC) $(CCFLAGS) -std=c++0x -g -x c++-header $< -o $@ 

$(GCHFiles_examples): objs/%.hh.gch : %.hh GameApi.hh Makefile
	@echo g++ $< -o $@
	@$(CC) $(CCFLAGS) -std=c++0x -g -I. -x c++-header $< -o $@ 

clean:
	@rm -rf objs/*.o objs/*.d objs/*.gch
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

