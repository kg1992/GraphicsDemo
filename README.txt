	[ Project Name ]
Graphics Demo

	[ Creation Date ]
2019 / 02 / 21

	[ References ]
https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
https://boycoding.tistory.com/134?category=990597
https://learnopengl.com/In-Practice/Text-Rendering
https://www.mixamo.com/#/

	[ External Dependencies ]
OpenGL
glad
glm
FbxSdk
Freetype2

	[ Control ]
Wheel = Magnify/Minify
Wheel Down + Mouse Move = Revolve around view center
Ctrl + Wheel Down + Mosue Move = Magnify/Minify
Shift + Wheel Down + Mouse Move = Slide Camera
W, A, S, D, Shift, Space = Control camera

R = Reset Camera

` = trigger command input
	save scene [scenename] - generates [scenename].dat to Resources/Scene folder.
	load scene [scenename] - loads [sceneanme].dat