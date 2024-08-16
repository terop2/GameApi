<h1>GameApi FeatureList</h1>

<ul>
<li>(bm_render/bm_2d) Sprites
<li>(BM) Bitmaps
<li>(P) 3D Models
<li>(P) .obj, .mtl, .stl, .glb, .gltf files support
<li>(bg_image) background images
<li>(heighmap) heightmaps
<li>(p_or_elem|array) combining 3d models
<li>(translate/rotatex|y|z/scale) translate/rotate/scale
<li>(recalc_normals) recalculating normals
<li>(smooth_normals) smooth normals
<ul>Materials
<li>m_colour
<li>m_textures
<li>m_phong
<li>m_snow
<li>m_edge_glow
<li>m_toon_border
<li>m_gltf material
<li>m_fog
<li>m_dir_shadow
<li>m_or_elem combining materials
</ul>
<li>custom shader support
<ul>lines rendering
<li>face edge lines
<li>translate/scale lines
</ul>
<ul>points rendering
<li>loading points from file
<li>random points picked from quads
<li>random points picked from volumes
</ul>
<ul>minecraft rendering
<li>default cubes
<li>default material
<li>voxels
</ul>
<ul>moving 3d models
<li>translate/rotate/scale
<li>local move
<li>anim translate/rotate/scale
</ul>
<ul>voxels
<li>(vx_or_elem) combining voxels
<li>(vx_blit) volume to voxel conversion
<li>(vx_inst) voxel to points conversion
<li>(vx_render) rendering voxels via cubes and points
<li>(vx_bind) rendering voxels with materials
<li>(vx_static) combining voxels to larger mesh
</ul>
<ul>volumes
<li>sphere/cube/cone/torus
<li>colour
<li>move/rotate
<li>not/min/max/andnot
<li>(o_inst) instantiation
</ul>
<ul>fonts
<li>FI_load
<li>FI_drawtext
<li>FI_largetext
</ul>
<ul>chooser tools
<li>bm_chooser
<li>p_chooser
<li>ml_chooser
<li>ml_anim
</ul>
<ul>conditional execution
<li>area
<li>toggle
<li>timed
<li>move
<li>repeat
<li>keypress
<li>statemachine
<li>hmd
</ul>
<ul>tree rendering
<li>levels
<li>(tree_p) tree to mesh conversion
</ul>
<ul>textures
<li>binding
<li>conversion to bitmap
<li>fbos
<li>(pbos)
</ul>
<ul>boolean operations on meshes
<li>cube/sphere
<li>combining
<li>and_not
<li>intersect
</ul>
<ul>MainLoop features
<li>(t_start|t_time|t_time_switch|t_exit) Sequencing effects
<li>(html_url/bm|p|ml|mn|mt_script) scripting support
<li>generating bitmaps via scripts
<li>(ml_gltf_all) gltf rendering
<li>(gltf_load2) fetching assets from web
<li>(async_gltf) async loading of assets
<li>(or_elem_ml|array_ml) combining mainloops
<li>(move_ml) moving 3d models
<li>(repeat_ml) repeating events
<li>(sp_render) sprite rendering
<li>(p_render|_inst|_txid) 3d model rendering
<li>(pts_render) points rendering
<li>(m_bind|_many|_inst|_instmat|_instmany) 3d model rendering with materials
<li>(b_divide|b_bind|b_draw) splitting 3d models to smaller pieces and rendering
<li>(wasd_ml|quake_ml|quake_ml2) input handling
<li>(key_activate|comb_key_activate) keyboard handling
<li>(touch_rotate) rotating with touchscreen/mouse
<li>(mouse_zoom|2) zooming with mouse wheel
<li>(mouse_pan) moving with right mouse button
<li>(key_record/key_playback) recording and playback of key events
<li>(key_printer_ml) debugging key events
<li>(mesh_stats_ml) debugging mesh properties
<li>(print_deps) printing internal dependencies
<li>(joy_printer_ml) joystick printer
<li>(fps_display) fps display
<li>(score|time_display) displaying score and time
<li>(restart_key|restart_screen) displaying restart screen
<li>(ogl_|disable_polygons|depthfunc|depthmask|blendfunc|zbuf_disable|transparency|perspective) low level opengl operations
<li>(concurrent_download) loading sanmiguel concurrently
<li>(eng_face_count) passing face count to javascript
<li>(song_ml) playing music via .ogg files
<li>(sound_ml) playing sound effects via .wav files
<li>(run_window) fullscreen window
<li>(html_run|html_ml|html_p|html_mt) converting engine to html5
<li>(looking_glass) holographic screen support (windows only)
<li>(vr_window) virtual reality window support
<li>(pkg_window|2) packing engine features to independent binary files
<li>(save_deploy) converting html5 to .zip file for deploying at hosting space
</ul>
