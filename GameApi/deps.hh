
#define F_MAINLOOP_API 1
#define F_SPRITE_API 1
#define F_TEXTURE_API 1
#define F_GRID_API 1
#define F_BITMAP_API 1
#define F_FLOAT_API 1
#define F_ARRAY_API 1
#define F_FLOAT_ARRAY_API 1
#define F_FONT_API 1
#define F_ANIM_API 1
#define F_EVENT_API 1
#define F_CURVE_API 1
#define F_MATRIX_CURVE_API 1
#define F_CURVES_API 1
#define F_FUNCTION_API 1
#define F_SPACE_API 1
#define F_DUP_API 1
#define F_IMPLICIT_API 1
#define F_VOLUME_API 1
#define F_SHADER_MODULE_API 1
#define F_INTERACTION_API 1
#define F_PICKING_API 1
#define F_TREE_API 1
#define F_MATERIALS_API 1
#define F_INPUT_API 1
#define F_PHYSICS_API 1
#define F_COLLISION_PLANE 1
#define F_SKELETAL 1
#define F_MOVEMENT_NODE 1
#define F_EXPR_API 1
#define F_GUI_API 1
#define F_WMOD_API 1
#define F_FLOAT_VOLUME_API 1
#define F_COLOR_VOLUME_API 1
#define F_VECTOR_VOLUME_API 1
#define F_DISTANCE_FLOAT_VOLUME_API 1
#define F_SEPARATE_API 1
#define F_SURFACE_API 1
#define F_TRISTRIP_API 1
#define F_CUTTER_API 1
#define F_BOOLEAN_OPS 1
#define F_POLYGON_DISTANCE_FIELD 1
#define F_POLYGON_ARRAY_API 1
#define F_POLYGON_API 1
#define F_WAVEFORM_API 1
#define F_SAMPLE_COLLECTION_API 1
#define F_TRACKER_API 1
#define F_STATE_CHANGE_API 1
#define F_NEW_PLANE_API 1
#define F_PLANE_API 1
#define F_BOOL_BITMAP_API 1
#define F_FLOAT_BITMAP_API 1
#define F_VECTOR_BITMAP_API 1
#define F_CONTINUOUS_BITMAP_API 1
#define F_VOXEL_API 1
#define F_COLOR_API 1
#define F_POINT_API 1
#define F_POINT_COLLECTION_API 1
#define F_MATRICES_API 1
#define F_POINTS_API 1
#define F_LINES_API 1
#define F_VECTOR_API 1
#define F_SPACE_VECTOR_API 1
#define F_MATRIX_API 1
#define F_OBJECT_MOVE_API 1
#define F_VBO_API 1
#define F_PATH_API 1
#define F_UBER_SHADER_API 1
#define F_SHADER_API 1
#define F_FRAMEBUFFER_API 1
#define F_LAYOUT_API 1
#define F_DRAW_API 1
#define F_GAMES_API 1
#define F_TEXT_API 1

#ifdef F_TEXT_API
// from class definition
#define F_BITMAP_API 1
#define F_SPRITE_API 1
#endif

#ifdef F_WMOD_API
// from class definition
#define F_GUI_API 1
#endif

#ifdef F_STATE_CHANGE_API
#define F_SHADER_API 1
#endif
