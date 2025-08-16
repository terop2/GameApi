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
BD_CALVIN = $(shell uname -n)


LIBPATH=/usr/lib/x86_64-linux-gnu/
GL_PATH=$(LIBPATH)
GL_NAME=libGL.so
GL_CFLAGS=`pkg-config --cflags gl`
GL_PKGCONFIG = `pkg-config --libs gl`
ifndef GL_PKGCONFIG
GL_PKGCONFIG = $(GL_PATH)$(GL_NAME)
endif
SDL_PATH=$(LIBPATH)
SDL_NAME=libSDL2-2.0.so
SDL_CFLAGS=`pkg-config --cflags sdl2`
SDL_PKGCONFIG=`pkg-config --libs sdl2`
ifndef SDL_PKGCONFIG
SDL_PKGCONFIG = $(SDL_PATH)$(SDL_NAME)
endif
SDL_MIXER_PATH=$(SDL_PATH)
SDL_MIXER_NAME=libSDL2_mixer-2.0.so
SDL_MIXER_CFLAGS=`pkg-config --cflags SDL2_mixer`
SDL_MIXER_PKGCONFIG=`pkg-config --libs SDL2_mixer`
ifndef SDL_MIXER_PKGCONFIG
SDL_MIXER_PKGCONFIG = $(SDL_MIXER_PATH)$(SDL_MIXER_NAME)
endif
GLEW_PATH=$(LIBPATH)
GLEW_NAME=libGLEW.so.2.2
GLEW_CFLAGS=`pkg-config --cflags glew`
GLEW_PKGCONFIG = `pkg-config --libs glew`
ifndef GLEW_PKGCONFIG
GLEW_PKGCONFIG = $(GLEW_PATH)$(GLEW_NAME)
endif
FREETYPE_PATH=$(LIBPATH)
FREETYPE_NAME=libfreetype.so
FREETYPE_CFLAGS=`pkg-config --cflags freetype2`
FREETYPE_PKGCONFIG=`pkg-config --libs freetype2`
ifndef FREETYPE_PKGCONFIG
FREETYPE_PKGCONFIG = $(FREETYPE_PATH)$(FREETYPE_NAME)
endif
OPENCV_PATH=$(LIBPATH)
OPENCV_NAME=libopencv.so
OPENCV_CFLAGS=`pkg-config --cflags opencv4`
OPENCV_PKGCONFIG=`pkg-config --libs opencv4`
ifndef OPENCV_PKGCONFIG
OPENCV_PKGCONFIG = $(OPENCV_PATH)$(OPENCV_NAME)
endif

LDFLAGS= $(SDL_MIXER_PKGCONFIG) $(GLEW_PKGCONFIG) $(FREETYPE_PKGCONFIG) $(OPENCV_PKGCONFIG)

CMDFiles = GameApi.cc GameApi_gltf.cc GameApi_vo.cc GameApi_vx.cc GameApi_pl.cc	GameApi_cmd_move.cc \
	GameApi_cmd_bbitmap.cc       GameApi_cmd_pdfield.cc \
	GameApi_cmd_bitmap.cc        GameApi_cmd_point.cc \
	GameApi_cmd_booleanops.cc    GameApi_cmd_points.cc \
	GameApi_cmd_cvolume.cc       GameApi_cmd_polygon2.cc \
	GameApi_cmd_fbitmap.cc       GameApi_cmd_polygon.cc \
	GameApi_cmd_font.cc          GameApi_cmd_framebuffer.cc \
	GameApi_cmd_texture.cc GameApi_cmd_fvolume.cc \
	GameApi_cmd_lines.cc         GameApi_cmd_vector.cc \
	GameApi_cmd_mainloop.cc      GameApi_cmd_volume.cc \
	GameApi_cmd_material.cc      GameApi_cmd_waveform.cc GameApi_cmd_shader.cc

CFiles=	GameApi_dep_store.cc GameApi_dep_add_find.cc GameApi_dep_env.cc GameApi_dep_arr_render.cc GameApi_gui.cc  GameApi2.cc GameApi_diag.cc Pieces.cc Map.cc VectorTools.cc Effect.cc Buffer.cc GameApi_main.cc \
	Intersect.cc Bitmap.cc Effect2.cc Shader.cc VolumeObjects.cc \
	Tree.cc Event.cc ObjectCreator.cc IntersectObject.cc Graph.cc \
	Widgets.cc Category.cc Plane.cc Coordinates.cc Triangle.cc Game.cc \
	Editor.cc Web.cc Functions.cc ShaderExpr.cc Font.cc FontEditor.cc \
	 GameApiTest.cc Games.cc HierarchyObject.cc Physics.cc \
        GameApiTest2.cc State.cc Serialize.cc KeyFrameEditor.cc Parser.cc \
	FreeType.cc FreeType2.cc VertexArray.cc StateChange.cc DistanceObject.cc GameRunner.cc \
	GameApi_an.cc GameApi_bm.cc GameApi_co.cc GameApi_dr.cc GameApi_ev.cc \
	GameApi_ex.cc GameApi_fbo.cc GameApi_f.cc GameApi_fn.cc \
	GameApi_fnt.cc GameApi_gr.cc  \
        GameApi_lay.cc \
	GameApi_li.cc GameApi_mx.cc GameApi_pc.cc \
	GameApi_phy.cc GameApi_pla.cc GameApi_pt.cc \
	GameApi_pts.cc GameApi_sh.cc GameApi_shm.cc GameApi_sm.cc \
        GameApi_spa.cc GameApi_sp.cc GameApi_st.cc GameApi_su.cc \
        GameApi_tex.cc GameApi_tr.cc GameApi_trk.cc GameApi_tx.cc \
        GameApi_vbo.cc GameApi_ve.cc GameApi_wmod.cc \
        GameApi_wv.cc GameApi_cut.cc GameApi_in.cc \
	GameApi_imp.cc GameApi_plane.cc GameApi_integrator.cc Main.cc GameApi_vr.cc GameApi_low.cc GameApi_qt.cc Math2.cc \
	GameApi_cmd.cc cpushader.cc \
draco/attributes/attribute_octahedron_transform.cc    draco/attributes/geometry_attribute.cc \
draco/attributes/attribute_quantization_transform.cc  draco/attributes/point_attribute.cc \
draco/attributes/attribute_transform.cc \
draco/compression/decode.cc                          draco/compression/encode.cc         \
draco/compression/draco_compression_options.cc       draco/compression/expert_encode.cc \
draco/compression/attributes/attributes_decoder.cc \
draco/compression/attributes/attributes_encoder.cc \
draco/compression/attributes/sequential_attribute_decoder.cc \
draco/compression/attributes/sequential_attribute_decoders_controller.cc \
draco/compression/attributes/sequential_attribute_encoder.cc \
draco/compression/attributes/sequential_attribute_encoders_controller.cc \
draco/compression/attributes/sequential_integer_attribute_decoder.cc \
draco/compression/attributes/sequential_integer_attribute_encoder.cc \
draco/compression/attributes/sequential_normal_attribute_decoder.cc \
draco/compression/attributes/sequential_normal_attribute_encoder.cc \
draco/compression/attributes/sequential_quantization_attribute_decoder.cc \
draco/compression/attributes/sequential_quantization_attribute_encoder.cc \
draco/compression/attributes/prediction_schemes/prediction_scheme_encoder_factory.cc \
draco/compression/bit_coders/adaptive_rans_bit_decoder.cc  draco/compression/bit_coders/direct_bit_encoder.cc   \
draco/compression/bit_coders/adaptive_rans_bit_encoder.cc  draco/compression/bit_coders/rans_bit_decoder.cc    draco/compression/bit_coders/symbol_bit_decoder.cc \
draco/compression/bit_coders/direct_bit_decoder.cc         draco/compression/bit_coders/rans_bit_encoder.cc    draco/compression/bit_coders/symbol_bit_encoder.cc \
draco/compression/entropy/shannon_entropy.cc       draco/compression/entropy/symbol_encoding.cc \
draco/compression/entropy/symbol_decoding.cc \
draco/compression/mesh/mesh_decoder.cc                \
draco/compression/mesh/mesh_edgebreaker_decoder.cc       draco/compression/mesh/mesh_encoder.cc \
draco/compression/mesh/mesh_edgebreaker_decoder_impl.cc  \
draco/compression/mesh/mesh_edgebreaker_encoder.cc       draco/compression/mesh/mesh_sequential_decoder.cc \
draco/compression/mesh/mesh_edgebreaker_encoder_impl.cc  draco/compression/mesh/mesh_sequential_encoder.cc \
draco/compression/point_cloud/point_cloud_decoder.cc        \
draco/compression/point_cloud/point_cloud_encoder.cc \
draco/compression/point_cloud/point_cloud_sequential_decoder.cc \
draco/compression/point_cloud/point_cloud_kd_tree_decoder.cc \
draco/compression/point_cloud/point_cloud_sequential_encoder.cc \
draco/compression/point_cloud/point_cloud_kd_tree_encoder.cc \
draco/compression/point_cloud/algorithms/dynamic_integer_points_kd_tree_decoder.cc  draco/compression/point_cloud/algorithms/float_points_tree_encoder.cc \
draco/compression/point_cloud/algorithms/dynamic_integer_points_kd_tree_encoder.cc  draco/compression/point_cloud/algorithms/integer_points_kd_tree_decoder.cc \
draco/compression/point_cloud/algorithms/float_points_tree_decoder.cc               draco/compression/point_cloud/algorithms/integer_points_kd_tree_encoder.cc \
draco/core/bit_utils.cc               draco/core/divide.cc            draco/core/options.cc \
draco/core/bounding_box.cc            draco/core/quantization_utils.cc \
draco/core/draco_types.cc      \
draco/core/cycle_timer.cc             draco/core/encoder_buffer.cc    draco/core/status.cc \
draco/core/data_buffer.cc             draco/core/hash_utils.cc     \
draco/core/decoder_buffer.cc       \
draco/mesh/corner_table.cc                 draco/mesh/mesh_misc_functions.cc    draco/mesh/mesh_splitter.cc \
draco/mesh/mesh_are_equivalent.cc          draco/mesh/mesh_stripifier.cc \
draco/mesh/mesh_attribute_corner_table.cc  \
draco/mesh/mesh.cc                         draco/mesh/mesh_utils.cc \
draco/mesh/mesh_cleanup.cc                \
draco/mesh/triangle_soup_mesh_builder.cc \
draco/mesh/mesh_features.cc  \
draco/point_cloud/point_cloud_builder.cc       draco/point_cloud/point_cloud.cc \
draco/compression/attributes/kd_tree_attributes_decoder.cc \
draco/compression/attributes/kd_tree_attributes_encoder.cc \
draco/metadata/geometry_metadata.cc  draco/metadata/metadata_encoder.cc       draco/metadata/property_table.cc     \
draco/metadata/metadata.cc     \
draco/metadata/metadata_decoder.cc          draco/metadata/structural_metadata.cc \
webgpu.cc \
GameApi_webcam.cc

#lib/lib_webgpu.cc \
#lib/lib_webgpu_cpp20.cc


CFiles2 = 

Subdirs = 
#module1

OFiles = $(patsubst %.cc,objs/%.o,$(CFiles) $(CFiles2))
CMDOFiles = $(patsubst %.cc,objs/%.o,$(CMDFiles))
CMDOFiles2 = $(patsubst %.cc,objs/%.o,$(CMDFiles))
CMDOFiles2a = $(patsubst %.cc,objs/%_2.o,$(CMDFiles))
O2Files = $(patsubst %.cc,objs/%.obj,$(CFiles) $(CFiles2))
DFiles = $(patsubst %.cc,objs/%.d,$(CFiles) $(CFiles2)) 
D2Files = $(patsubst %.cc,objs/%.d2,$(CFiles) $(CFiles2)) 
HFiles = $(patsubst %.cc,%.hh,$(CFiles))
#GCHFiles = $(patsubst %.cc,objs/%.hh.gch,$(CFiles))
#GCH2Files = $(patsubst %.cc,objs/%.hh.gch2,$(CFiles))
Subdirsx = $(patsubst %,%build,$(Subdirs))

WARN = -Wno-unused-variable -Wno-sign-compare -Wno-reorder -Wno-unused-but-set-variable -Wno-unused-function -Wno-misleading-indentation -Wno-unused-result -Wno-return-type -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-restrict -Wno-uninitialized -Wno-volatile -Wno-nonnull-compare -Wno-address -Wno-bool-compare -Wno-comment -Wno-unused-local-typedefs -Wno-range-loop-construct -Wno-deprecated-enum-enum-conversion

#-fsanitize=undefined -fsanitize-recover=undefined
#  -I/usr/include/freetype2 -I$(INCWIN) -I$(INC2WIN) -I$(INC3WIN) -I$(INC4WIN) -I$(INC5WIN) -DGLEW_STATIC

ADDRESS=
ifeq (DEBUG,address)
ADDRESS=-fsanitize=address
endif

CCFLAGS = $(ADDRESS) -ffunction-sections -fdata-sections $(GL_CFLAGS) $(SDL_CFLAGS) $(SDL_MIXER_CFLAGS) $(GLEW_CFLAGS) $(FREETYPE_CFLAGS) $(OPENCV_CFLAGS) -I. -ggdb -std=c++20 -fPIC -Wall -I./objs -I/usr/include/freetype2 -I/usr/include/SDL2 -DLINUX  -lpthread -pthread -Ichai/ChaiScript-6.1.0/include -fno-var-tracking-assignments -rdynamic $(WARN) -I./hssynth2/midiplay/midifile/include -I./hssynth2/synthlib -fvisibility=hidden -fvisibility-inlines-hidden -DGAMEAPI_EXPORTS
CC2FLAGS = $(ADDRESS) -ffunction-section -fdata-sections $(GL_CFLAGS) $(SDL_CFLAGS) $(SDL_MIXER_CFLAGS) $(GLEW_CFLAGS) $(FREETYPE_CFLAGS) -I. -ggdb -std=c++20 -Wall -I./objs -DLINUX  -lpthread -pthread -Ichai/ChaiScript-6.1.0/include -fno-var-tracking-assignments -rdynamic $(WARN) -I./hssynth2/midiplay/midifile/include -I./hssynth2/synthlib -fvisibility=hidden -fvisibility-inlines-hidden -DGAMEAPI_EXPORTS
#`pkg-config gtkmm-2.4 --cflags`

BD_CALVIN_M64=
ifeq ($(BD_CALVIN),fedora)
CCFLAGS += -DBD_CALVIN_COMPUTER
CC2FLAGS += -DBD_CALVIN_COMPUTER
PLATFORM_FLAGS += -DARM2
endif

PLATFORM_FLAGS=
ifneq ($(BD_CALVIN),fedora)
CCFLAGS += -m64
CC2FLAGS += -m64
BD_CALVIN_M64 += -m64
endif

default: check_compile dirs libGameApi.so deploytool/deploytool editor/gameapi-builder cmdline/gameapi_cmdline


check_compile:
	@echo "SETUP: $(BD_CALVIN)"
ifeq ($(shell echo $(SDL_PKGCONFIG)),)
	@test -f $(SDL_PATH)$(SDL_NAME) && echo "  TEST   libsdl2=OK" || (echo "  TEST libsdl2=FAIL\n NOTE: recommended command: make -j 8 SDL_PATH=$(SDL_PATH) SDL_NAME=$(SDL_NAME)"; exit 1)
else
	@echo "  PKGCNF libsdl2=OK"
endif
ifeq ($(shell echo $(SDL_MIXER_PKGCONFIG)),)
	@test -f $(SDL_MIXER_PATH)$(SDL_MIXER_NAME) && echo "  TEST   libsdl2_mixer=OK" || (echo "  TEST libsdl2_mixer=FAIL\n NOTE: recommended command: make -j 8 SDL_MIXER_PATH=$(SDL_MIXER_PATH) SDL_MIXER_NAME=$(SDL_MIXER_NAME)"; exit 1)
else
	@echo "  PKGCNF libSDL2_mixer=OK"
endif
ifeq ($(shell echo $(GL_PKGCONFIG)),)
	@test -f $(GL_PATH)$(GL_NAME) && echo "  TEST   libGL=OK" || (echo "  TEST libGL=FAIL\n NOTE: recommended command: make -j 8 GL_PATH=$(GL_PATH) GL_NAME=$(GL_NAME)"; exit 1)
else
	@echo "  PKGCNF libGL=OK"
endif
ifeq ($(shell echo $(GLEW_PKGCONFIG)),)
	@test -f $(GLEW_PATH)$(GLEW_NAME) && echo "  TEST   libGLEW=OK" || (echo "  TEST libGLEW=FAIL\n NOTE: recommended command: make -j 8 GLEW_PATH=$(GLEW_PATH) GLEW_NAME=$(GLEW_NAME)" ; exit 1)
else
	@echo "  PKGCNF libGLEW=OK"
endif
ifeq ($(shell echo $(FREETYPE_PKGCONFIG)),)
	@test -f $(FREETYPE_PATH)$(FREETYPE_NAME) && echo "  TEST   libfreetype=OK" || (echo "  TEST libfreetype=FAIL\n NOTE: recommended command: make -j 8 FREETYPE_PATH=$(FREETYPE_PATH) FREETYPE_NAME=$(FREETYPE_NAME)" ; exit 1)
else
	@echo "  PKGCNF libfreetype=OK"
endif
ifeq ($(shell echo $(OPENCV_PKGCONFIG)),)
	@test -f $(OPENCV_PATH)$(OPENCV_NAME) && echo "  TEST   libopencv=OK" || (echo "  TEST libopencv=FAIL\n NOTE: recommended command: make -j 8 OPENCV_PATH=$(FREETYPE_PATH) OPENCV_NAME=$(FREETYPE_NAME)" ; exit 1)
else
	@echo "  PKGCNF libopencv=OK"
endif


dirs:
	@echo "  MKDIR  objs"
	@mkdir -p objs
	@echo "  MKDIR  objs/lib"
	@mkdir -p objs/lib
	@echo "  MKDIR  objs/draco/attributes"
	@mkdir -p objs/draco/attributes
	@echo "  MKDIR  objs/draco/compression"
	@mkdir -p objs/draco/compression
	@echo "  MKDIR  objs/draco/compression/attributes"
	@mkdir -p objs/draco/compression/attributes
	@echo "  MKDIR  objs/draco/compression/attributes/prediction_schemes"
	@mkdir -p objs/draco/compression/attributes/prediction_schemes
	@echo "  MKDIR  objs/draco/compression/bit_coders"
	@mkdir -p objs/draco/compression/bit_coders
	@echo "  MKDIR  objs/draco/compression/entropy"
	@mkdir -p objs/draco/compression/entropy
	@echo "  MKDIR  objs/draco/compression/mesh"
	@mkdir -p objs/draco/compression/mesh
	@echo "  MKDIR  objs/draco/compression/point_cloud"
	@mkdir -p objs/draco/compression/point_cloud
	@echo "  MKDIR  objs/draco/compression/point_cloud/algorithms"
	@mkdir -p objs/draco/compression/point_cloud/algorithms
	@echo "  MKDIR  objs/draco/core"
	@mkdir -p objs/draco/core
	@echo "  MKDIR  objs/draco/mesh"
	@mkdir -p objs/draco/mesh
	@echo "  MKDIR  objs/draco/point_cloud"
	@mkdir -p objs/draco/point_cloud
	@echo "  MKDIR  objs/draco/metadata"
	@mkdir -p objs/draco/metadata


libGameApi.so: $(OFiles) $(CMDOFiles) Makefile
	@echo "  LD     libGameApi.so"
	@$(CC) $(ADDRESS) -DDEFINE_ALL -Wl,--gc-sections $(BD_CALVIN_M64) -rdynamic -shared $(OFiles) $(CMDOFiles) -std=c++20 -o libGameApi.so  $(LDFLAGS) -lstdc++ -lwayland-client 

#libGameApi_small.so: $(OFiles) $(CMDOFiles2a) Makefile
#	@echo "  LD     libGameApi_small.so"
#	@$(CC) -Wl,--gc-sections $(BD_CALVIN_M64) -rdynamic -shared $(OFiles) $(CMDOFiles2a) -std=c++20 -o libGameApi_small.so  $(LDFLAGS) -lstdc++ -lwayland-client 


.PHONY: dirs

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


deploytool/deploytool: deploytool/Makefile deploytool/main.cc libGameApi.so
	@(cd deploytool; make -f Makefile DEBUG="$(DEBUG)")

cmdline/gameapi_cmdline: cmdline/Makefile cmdline/test.cpp libGameApi.so
	@(cd cmdline; make -f Makefile DEBUG="$(DEBUG)")

editor/gameapi-builder: editor/Makefile.Linux editor/test.cpp libGameApi.so
	@(cd editor; make -f Makefile.Linux DEBUG="$(DEBUG)" LDFLAGS="$(LDFLAGS)")

$(OFiles): objs/%.o : %.cc
	@echo "  CC     $@"
	@$(CC) $(PLATFORM_FLAGS) -std=c++20 -g -c $(CCFLAGS) $< -o $@

$(CMDOFiles): objs/%.o : %.cc
	@echo "  CC     $@"
	@$(CC) -DDEFINE_ALL -std=c++20 -g -c $(CCFLAGS) $< -o $@

$(CMDOFiles2a): objs/%_2.o : %.cc
	@echo "  CC     $@"
	@$(CC) -std=c++20 -g -c $(CCFLAGS) $< -o $@


touch_all:
	touch GameApi_gui_1.cc
	touch GameApi_gui_2.cc
	touch GameApi_gui_3.cc

DESTDIR ?= 

install:
	@echo "Installling to '$(DESTDIR)'"
	mkdir -p $(DESTDIR)
	mkdir -p $(DESTDIR)/usr
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/usr/share
	cp editor/gameapi-builder $(DESTDIR)/usr/bin/gameapi-builder
	cp cmdline/gameapi_cmdline $(DESTDIR)/usr/bin/gameapi_cmdline
	cp deploytool/deploytool $(DESTDIR)/usr/bin/gameapi_deploytool
	cp libGameApi.so $(DESTDIR)/usr/lib/x86_64-linux-gnu/libGameApi.so
	strip $(DESTDIR)/usr/bin/gameapi-builder
	strip $(DESTDIR)/usr/lib/x86_64-linux-gnu/libGameApi.so
	cp editor/atlas0.txt $(DESTDIR)/usr/share/atlas0.txt
	cp editor/atlas1.txt $(DESTDIR)/usr/share/atlas1.txt
	cp editor/atlas2.txt $(DESTDIR)/usr/share/atlas2.txt
	cp editor/atlas_bm0.ppm $(DESTDIR)/usr/share/atlas_bm0.ppm
	cp editor/atlas_bm1.ppm $(DESTDIR)/usr/share/atlas_bm1.ppm
	cp editor/atlas_bm2.ppm $(DESTDIR)/usr/share/atlas_bm2.ppm
	cp display/gameapi_0.html $(DESTDIR)/usr/share/gameapi_0.html
	cp display/gameapi_0_seamless.html $(DESTDIR)/usr/share/gameapi_0_seamless.html
	cp display/gameapi_1.html $(DESTDIR)/usr/share/gameapi_1.html
	cp display/gameapi_2.html $(DESTDIR)/usr/share/gameapi_2.html
	cp display/gameapi_3.html $(DESTDIR)/usr/share/gameapi_3.html
	cp display/gameapi_3_seamless.html $(DESTDIR)/usr/share/gameapi_3_seamless.html
	cp display/gameapi_display.zip $(DESTDIR)/usr/share/gameapi_display.zip
	cp display/get_file_size.php $(DESTDIR)/usr/share/get_file_size.php
	cp editor/deploy.sh $(DESTDIR)/usr/share/deploy.sh
	cp editor/arrow.ppm $(DESTDIR)/usr/share/arrow.ppm
	@echo "Install Done."

clean:
	@rm -rf objs/*.o objs/*.obj objs/*.d objs/*.d2 objs/*.gch objs/*.gch2
	@echo Clean.


#-include $(DFiles)
