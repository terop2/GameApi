cmake_minimum_required(VERSION 2.6)
project(GameApi)
include_directories(d:\\cvs\\sdl2_vc\\SDL2-2.0.3\\include
		    d:\\cvs\\sdl2_vc\\SDL2_image-2.0.0\\include
		    d:\\cvs\\glew-1.10.0\\include
		    d:\\cvs\\freetype\\include
		    d:\\cvs\\freetype\\include\\freetype2
		    )
add_library(GameApi Pieces.cc Map.cc VectorTools.cc Effect.cc Buffer.cc
	Intersect.cc Bitmap.cc Effect2.cc Shader.cc VolumeObjects.cc 
	Tree.cc Event.cc ObjectCreator.cc IntersectObject.cc Graph.cc 
	Widgets.cc Category.cc Plane.cc Coordinates.cc Triangle.cc Game.cc 
	Editor.cc Web.cc Functions.cc ShaderExpr.cc Font.cc FontEditor.cc 
	GameApi.cc GameApiTest.cc Games.cc HierarchyObject.cc Physics.cc 
        GameApiTest2.cc State.cc Serialize.cc KeyFrameEditor.cc Parser.cc 
	FreeType.cc VertexArray.cc StateChange.cc DistanceObject.cc Main.cc)

include_directories(d:\\cvs\\gameapi-code\\GameApi)
add_executable(main GameApiTest3.cc GameApiTest4.cc GameApiWorld.cc GameApiTest5.cc GameApiTest6.cc GameApiTest7.cc GameApiTest8.cc GameApiTest9.cc GameApiTestA.cc ExampleMain.cc)
target_link_libraries(main d:\\cvs\\freetype\\lib\\freetype.lib d:\\cvs\\glew-1.10.0\\lib\\Release\\Win32\\glew32s.lib d:\\cvs\\glew-1.10.0\\lib\\Release\\Win32\\glew32.lib d:\\cvs\\sdl2_vc\\SDL2-2.0.3\\lib\\x86\\SDL2.lib d:\\cvs\\sdl2_vc\\SDL2_image-2.0.0\\lib\\x86\\SDL2_image.lib opengl32.lib)
