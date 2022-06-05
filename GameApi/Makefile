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


CFiles=	GameApi_dep_add_find.cc GameApi_dep_env.cc GameApi_dep_arr_render.cc GameApi_gui.cc  GameApi2.cc GameApi_diag.cc GameApi_gltf.cc Pieces.cc Map.cc VectorTools.cc Effect.cc Buffer.cc \
	Intersect.cc Bitmap.cc Effect2.cc Shader.cc VolumeObjects.cc \
	Tree.cc Event.cc ObjectCreator.cc IntersectObject.cc Graph.cc \
	Widgets.cc Category.cc Plane.cc Coordinates.cc Triangle.cc Game.cc \
	Editor.cc Web.cc Functions.cc ShaderExpr.cc Font.cc FontEditor.cc \
	GameApi.cc GameApiTest.cc Games.cc HierarchyObject.cc Physics.cc \
        GameApiTest2.cc State.cc Serialize.cc KeyFrameEditor.cc Parser.cc \
	FreeType.cc FreeType2.cc VertexArray.cc StateChange.cc DistanceObject.cc GameRunner.cc \
	GameApi_an.cc GameApi_bm.cc GameApi_co.cc GameApi_dr.cc GameApi_ev.cc \
	GameApi_ex.cc GameApi_fbo.cc GameApi_f.cc GameApi_fn.cc \
	GameApi_fnt.cc GameApi_gr.cc  \
        GameApi_lay.cc \
	GameApi_li.cc GameApi_main.cc GameApi_mx.cc GameApi_pc.cc \
	GameApi_phy.cc GameApi_pla.cc GameApi_pl.cc GameApi_pt.cc \
	GameApi_pts.cc GameApi_sh.cc GameApi_shm.cc GameApi_sm.cc \
        GameApi_spa.cc GameApi_sp.cc GameApi_st.cc GameApi_su.cc \
        GameApi_tex.cc GameApi_tr.cc GameApi_trk.cc GameApi_tx.cc \
        GameApi_vbo.cc GameApi_ve.cc GameApi_vo.cc GameApi_wmod.cc \
        GameApi_wv.cc GameApi_vx.cc GameApi_cut.cc GameApi_in.cc \
	GameApi_imp.cc GameApi_plane.cc GameApi_integrator.cc Main.cc GameApi_vr.cc GameApi_low.cc GameApi_qt.cc Math2.cc \
	GameApi_cmd_move.cc \
	GameApi_cmd_bbitmap.cc       GameApi_cmd_pdfield.cc \
	GameApi_cmd_bitmap.cc        GameApi_cmd_point.cc \
	GameApi_cmd_booleanops.cc    GameApi_cmd_points.cc \
	GameApi_cmd_cvolume.cc       GameApi_cmd_polygon2.cc \
	GameApi_cmd_fbitmap.cc       GameApi_cmd_polygon.cc \
	GameApi_cmd_font.cc          GameApi_cmd_framebuffer.cc \
	GameApi_cmd_texture.cc GameApi_cmd_fvolume.cc \
	GameApi_cmd_lines.cc         GameApi_cmd_vector.cc \
	GameApi_cmd_mainloop.cc      GameApi_cmd_volume.cc \
	GameApi_cmd_material.cc      GameApi_cmd_waveform.cc \
	GameApi_cmd.cc


CFiles2 = 

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

WARN = -Wno-unused-variable -Wno-sign-compare -Wno-reorder -Wno-unused-but-set-variable -Wno-unused-function

CCFLAGS = -fsanitize=address -m64 -std=c++17 -fPIC -Wall -I./objs -I/usr/include/freetype2 -I/usr/include/SDL2 -DLINUX -DAMD -lpthread -pthread -Ichai/ChaiScript-6.1.0/include -fno-var-tracking-assignments -rdynamic $(WARN) 
CC2FLAGS = -fsanitize=address -m64 -std=c++17 -Wall -I./objs -I/usr/include/freetype2 -I$(INCWIN) -I$(INC2WIN) -I$(INC3WIN) -I$(INC4WIN) -I$(INC5WIN) -DGLEW_STATIC -DLINUX -DAMD -lpthread -pthread -Ichai/ChaiScript-6.1.0/include -fno-var-tracking-assignments -rdynamic $(WARN) 
#`pkg-config gtkmm-2.4 --cflags`


default: libGameApi.so


libGameApi.so: $(OFiles) Makefile
	@echo "  LD    libGameApi.so"
	@g++ -m64 -rdynamic -shared -g $(OFiles) -std=c++17 -o libGameApi.so /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/x86_64-linux-gnu/libSDL2_mixer-2.0.so /usr/lib/x86_64-linux-gnu/libGLEW.so.2.2 /usr/lib/x86_64-linux-gnu/libfreetype.so -lstdc++ -lwayland-client

#`pkg-config gtkmm-2.4 --libs` 

#subdirs: $(Subdirs)

#$(Subdirs):
#	$(MAKE) -C $@

#.PHONY: subdirs $(Subdirs)

#ctags: TAGS

#TAGS: $(CFiles)
#	etags -o TAGS -l c++ $(CFiles)

CC = $(CROSS_COMPILE)g++
CCWIN = i586-mingw32msvc-g++

#$(DFiles): objs/%.d: %.cc
#	@echo "  DEP    $@"
#	@$(CC) -M $(CCFLAGS) $< \
#	|sed 's,\($*\)\.o[ :]*,objs/\1.o $@ : ,g' > $@


$(OFiles): objs/%.o : %.cc
	@echo "  CC    $@"
	@$(CC) -std=c++17 -g -c $(CCFLAGS) $< -o $@

touch_all:
	touch GameApi_gui_1.cc
	touch GameApi_gui_2.cc
	touch GameApi_gui_3.cc


clean:
	@rm -rf objs/*.o objs/*.obj objs/*.d objs/*.d2 objs/*.gch objs/*.gch2
	@rm main
	@echo Clean.


#-include $(DFiles)
