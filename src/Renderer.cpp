#include "Renderer.h" 
#include "GL_Functions.h"

void get_window_size(HWND window, int& w, int& h) {
	RECT rect = {};
	if (GetClientRect(window, &rect) != 0) {
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
	} else {
		w = 0;
		h = 0;
		log("Window width and height are 0");
	}
}

void init_open_gl(HWND window) {
	HDC window_dc = GetDC(window);

	// IMPORTANT: There is a lot of randomness to this. Don't be discouraged if 
	// you don't remember it all.

	// PIXEL FORMAT DESCRIPTOR: The Pixel Format Descriptor (PFD) is a structure 
	// used in Windows-based OpenGL applications to define the properties and 
	// Windows content will be drawn. This structure provides essential information 
	// about how pixels are handled in the rendering context, including color depth, 
	// buffer types, transparency, and other graphical features like depth and stencil 
	// buffers.
	
	// NOTE: The pixel format descriptor is a negotiation we don't 
	// actually fill out a format descriptor, we find one
	PIXELFORMATDESCRIPTOR desired_pixel_format = {};
	desired_pixel_format.nSize = sizeof(desired_pixel_format);
	desired_pixel_format.nVersion = 1;
	desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	desired_pixel_format.cColorBits = 32;
	desired_pixel_format.cAlphaBits = 8;
	desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

	// NOTE: We ask for the pixel format the the one above matches the closes
	int suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
	PIXELFORMATDESCRIPTOR suggested_pixel_format;
	// NOTE: Sets the members of the PIXELFORMATDESCRIPTOR structure pointed to by ppfd 
	// with that pixel format data.
	DescribePixelFormat(
		window_dc,
		suggested_pixel_format_index,
		sizeof(suggested_pixel_format),
		&suggested_pixel_format
	);
	SetPixelFormat(window_dc, suggested_pixel_format_index, &suggested_pixel_format);

	HGLRC open_gl_rc = wglCreateContext(window_dc);
	if (wglMakeCurrent(window_dc, open_gl_rc)) {
		// Success
	} else {
		// failure
	}

	int width = 0;
	int height = 0;
	get_window_size(window, width, height);

	// NOTE: Sets the area of the window where the scene will be drawn. In this case, it 
	//		 starts at the bottom-left corner (0, 0) and stretches to the specified width and height.
	glViewport(0, 0, width, height);
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	// NOTE: Clears the window to the color set by glClearColor. It refreshes the color buffer, 
	//		 preparing it for new drawing.
	glClear(GL_COLOR_BUFFER_BIT);
	// NOTE: Swaps the front and back buffers, showing the newly drawn frame on the screen 
	//		 (double buffering). window_dc is the window's device context.
	SwapBuffers(window_dc);

	ReleaseDC(window, window_dc);
}

void create_renderer() {
	load_open_gl_functions();
}

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  

GLuint vbo;
void draw_triangle() {
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	const char* vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		log("Error: Vertex shader compilation failed");
	}

	const char* fragmentShaderSource = 
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n;"
		"{\n"
		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n";
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		log("Error: Fragment shader compilation failed");
	}

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		log("Error: Shader program linking failed");
	}

	glUseProgram(shaderProgram);

	// NOTE: Delete the shaders once they are linked to the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  
}
