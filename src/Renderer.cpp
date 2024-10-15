#include "Renderer.h" 
#include "GL_Functions.h"

#define GL_REPEAT                         0x2901
#define GL_MIRRORED_REPEAT                0x8370

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

enum SHADER_PROGRAM {
	SP_2D_BASIC
};

GLuint create_shader(const char* file_path, GLenum gl_shader_type) {
	FILE* file = fopen(file_path, "rb");
	if (file == NULL) {
		log("Error: Failed to open shader file");
		return 0;
	}
	
	fseek(file, 0, SEEK_END);
	UINT32 length = ftell(file);
	rewind(file);

	// NOTE: Allocate a empty buffer
	std::string shader_source_buffer(length, ' ');

	// NOTE: Buffer to the pointer to the first character
	fread(&shader_source_buffer[0], 1, length, file);

	fclose(file);

	GLuint shader;
	shader = glCreateShader(gl_shader_type);

	const char* shader_source_c_str = shader_source_buffer.c_str();
	glShaderSource(shader, 1, &shader_source_c_str, NULL);
	glCompileShader(shader);

	int success;
	char info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		log(info_log);
		return 0;
	}

	return shader;
}

GLuint create_shader_program(const char* vs_file_path, const char* fs_file_path) {
	GLuint vs = create_shader(vs_file_path, GL_VERTEX_SHADER);
	GLuint fs = create_shader(fs_file_path, GL_FRAGMENT_SHADER);

	GLuint shader_program;
	shader_program = glCreateProgram();

	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	int success;
	char info_log[512];
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		log(info_log);
		return 0;
	}

	// NOTE: The program stores and uses its own copy of the compiled shader code 
	glDeleteShader(vs);
	glDeleteShader(fs);

	return shader_program;
}

std::unordered_map<SHADER_PROGRAM, GLuint> shader_programs;
void load_shaders() {
	GLuint sp_2d_basic = create_shader_program("shaders\\vs_2d_basic.txt", "shaders\\fs_2d_basic.txt");
	shader_programs[SP_2D_BASIC] = sp_2d_basic;
}

LRESULT wind_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	// Returning 0 means we processed the message
	LRESULT result = 0;

	switch(message) {
	case WM_SIZE: {
		OutputDebugStringA("WM_SIZE\n");
		break;
	}
	case WM_ACTIVATEAPP: {
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;
	} 	
	case WM_CLOSE: {
		log("Destroying window");
		DestroyWindow(window);
		break; 
	} 
	case WM_DESTROY: {
		PostQuitMessage(0);
		OutputDebugStringA("WM_DESTROY\n");
		break;
	}
	default: {
		result = DefWindowProc(window, message, wparam, lparam);
		break;
	}
	}

	return result;
}

HWND init_windows(HINSTANCE hInstance) {
	WNDCLASSA wnd_class = {};

	wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wnd_class.lpfnWndProc = wind_proc;
	wnd_class.hInstance = hInstance;
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class.lpszClassName = "Alpha Game";

	HWND window_handle = {};
	if(RegisterClassA(&wnd_class)) {
		window_handle = CreateWindowExA(
			0,
			wnd_class.lpszClassName,	
			"Alpha Game",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			hInstance,
			0);
	}

	return window_handle;
}

HDC init_open_gl(HWND window) {
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

	HGLRC temp_context = wglCreateContext(window_dc);
	wglMakeCurrent(window_dc, temp_context);
	load_open_gl_functions();

	int attrib_list[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
#if _DEBUG
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB, //WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	HGLRC gl_renderering_context = wglCreateContextAttribsARB(window_dc, 0, attrib_list);
	// Make the new context current
	wglMakeCurrent(window_dc, gl_renderering_context);
	// Delete the garbage context
	wglDeleteContext(temp_context);

	int width = 0;
	int height = 0;
	get_window_size(window, width, height);

	// NOTE: Sets the area of the window where the scene will be drawn. In this case, it 
	//		 starts at the bottom-left corner (0, 0) and stretches to the specified width and height.
	glViewport(0, 0, width, height);

	return window_dc;
}

MP_Renderer* mp_create_renderer(HINSTANCE hInstance) {
	MP_Renderer* renderer = new MP_Renderer();

	HWND window_handle = init_windows(hInstance);
	renderer->open_gl.window_dc = init_open_gl(window_handle);

	glGenVertexArrays(1, &renderer->open_gl.vao);
	glGenBuffers(1, &renderer->open_gl.vbo);
	glGenBuffers(1, &renderer->open_gl.ebo);

	glBindVertexArray(renderer->open_gl.vao);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->open_gl.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->open_gl.ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(0);  
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);  
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coor));
	glEnableVertexAttribArray(2);  

	load_shaders();

	return renderer;
}

void render_clear(MP_Renderer* renderer) {
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	// NOTE: Clears the window to the color set by glClearColor. It refreshes the color buffer, 
	//		 preparing it for new drawing.
	glClear(GL_COLOR_BUFFER_BIT);
	// NOTE: Swaps the front and back buffers, showing the newly drawn frame on the screen 
	//		 (double buffering). window_dc is the window's device context.
}

int mp_set_texture_color_mod(MP_Texture* texture, ) {

}

// Copy a portion of the texture to the current renderer target
// NULL for the entire texture. NULL for the entire rendering target.
void render_copy(MP_Renderer* renderer, MP_Texture* texture, const Rect* src_rect, const Rect* dst_rect) {
	// Texture Packet
	Packet result;

// Square
Vertex vertices[] = {
	//     Position			      Color          Texture Coor
	{{-0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}}, // Bottom left
	{{ 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f}}, // Bottom Right
	{{ 0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, {1.0f, 1.0f}}, // Top Right
	{{-0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, 1.0f}}, // Top Left
};  
	// 1 
	Vertex vertex = {};
	vertex.pos = {};
	vertex.texture_coor = {};
	vertex.color = {};
	renderer->vertices.push_back(vertex);

	// 2
	vertex = {};
	vertex.pos = {};
	vertex.texture_coor = {};
	vertex.color = {};
	renderer->vertices.push_back(vertex);

	// 3
	vertex = {};
	vertex.pos = {};
	vertex.texture_coor = {};
	vertex.color = {};
	renderer->vertices.push_back(vertex);

	// 4
	vertex = {};
	vertex.pos = {};
	vertex.texture_coor = {};
	vertex.color = {};
	renderer->vertices.push_back(vertex);

	// retain the indices positions (size subtraction)
	// push back the indices (Draw Order - 6)

	renderer->indices.push_back(0);
	renderer->indices.push_back(1);
	renderer->indices.push_back(2);
	renderer->indices.push_back(2);
	renderer->indices.push_back(3);
	renderer->indices.push_back(0);

	renderer->packets.push_back(result);
	// push back the packet
}

void mp_render_present(MP_Renderer* renderer) {
	glBufferData(GL_ARRAY_BUFFER, renderer->vertices.size() * sizeof(Vertex), renderer->vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// NOTE: GL_BUFFER_SUB_DATA TO SAVE TIME

	// NOTE: One VAO for now
	glBindVertexArray(renderer->open_gl.vao);
	for (Packet& packet : renderer->packets) {
		if (packet.type == PT_TEXTURE) {
			glBindTexture(GL_TEXTURE_2D, packet.packet_texture.texture.gl_handle);

			// ***Why choose glDrawElements***
			// As you can see, there is some overlap on the vertices specified. 
			// We specify bottom right and top left twice! This is an overhead 
			// of 50% since the same rectangle could also be specified with only 
			// 4 vertices, instead of 6. This will only get worse as soon as we 
			// have more complex models that have over 1000s of triangles where 
			// there will be large chunks that overlap
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	SwapBuffers(renderer->open_gl.window_dc);
}

bool mp_is_valid_blend_mode(MP_BlendMode blend_mode) {
	switch(blend_mode) {
	case MP_BLENDMODE_NONE: {
		return true;
	} 
	case MP_BLENDMODE_BLEND: {
		return true;
	} 
	case MP_BLENDMODE_ADD: {
		return true;
	} 
	case MP_BLENDMODE_MOD: {
		return true;
	} 
	case MP_BLENDMODE_MUL: {
		return true;
	} 
	default: {
		return false;
	}
	}
}
// Returns 0 on success, -1 on failure
int mp_set_texture_blend_mode(MP_Texture* texture, MP_BlendMode blend_mode) {
	if (texture == NULL) {
		log("Error: Texture is NULL");
		return -1;
	}

	if (!mp_is_valid_blend_mode(blend_mode)) {
		return -1;
	}

	if (blend_mode != texture->blend_mode) {
		texture->blend_mode = blend_mode;
	}
	return 0;
}

int mp_set_texture_color_mod(MP_Texture* texture, uint8_t r, uint8_t g, uint8_t b) {
	if (texture == NULL) {
		log("Error: Texture is NULL");
		return -1;
	}
}
int mp_set_texture_alpha_mod(MP_Texture* texture, uint8_t a) {
	if (texture == NULL) {
		log("Error: Texture is NULL");
		return -1;
	}

	texture->mod.a = a;

	return 0;
}

// NOTE: Creates a blank texture
MP_Texture* mp_create_texture(MP_Renderer* renderer, uint32_t format, int access, int w, int h) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return nullptr;
	}

	MP_Texture* result = new MP_Texture();
	result->w = w;
	result->h = h;
	result->pitch = 0;
	result->pixels = NULL;

	// Default values
	mp_set_texture_blend_mode(result, MP_BLENDMODE_NONE);
	mp_set_texture_color_mod(result, 255, 255, 255);
	mp_set_texture_alpha_mod(result, 255);

	glGenTextures(1, &result->gl_handle);  

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, result->gl_handle);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// NOTE: A common mistake is to set one of the mipmap filtering options as the magnification 
	// filter. This doesn't have any effect since mipmaps are primarily used for when textures
	// get downscaled: texture magnification doesn't use mipmaps and giving it a mipmap filtering
	// option will generate an OpenGL GL_INVALID_ENUM error code.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Allocate empty data now
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return result;
}

void mp_destroy_texture(MP_Texture* texture) {
	if (texture->gl_handle != 0) {
		glDeleteTextures(1, &texture->gl_handle);
	}

	if (texture->pixels != NULL) {
		delete texture->pixels;
	}

	delete texture;
}

// NOTE: Uploads to the GPU
int mp_lock_texture(MP_Texture* texture, const MP_Rect* rect, void** pixels, int* pitch) {

}

// NOTE: Allocates a buffer on the CPU side
void mp_unlock_texture(MP_Texture* texture) {
	glGenerateMipmap(GL_TEXTURE_2D);
	// glTexSubImage2D
}

// Image* create_image() {
// 
// }

MP_Texture mp_create_texture(const char* file_path) {
	MP_Texture result;
	glGenTextures(1, &result.gl_handle);  

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, result.gl_handle);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// NOTE: A common mistake is to set one of the mipmap filtering options as the magnification 
	// filter. This doesn't have any effect since mipmaps are primarily used for when textures
	// get downscaled: texture magnification doesn't use mipmaps and giving it a mipmap filtering
	// option will generate an OpenGL GL_INVALID_ENUM error code.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image, 
	// but images usually have 0.0 at the top of the y-axis.
	stbi_set_flip_vertically_on_load(true); 

	// Load and generate the texture
	int width, height, n_channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &n_channels, 4);
	DEFER{
		stbi_image_free(data);
	};
	assert(n_channels == 4);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		log("Error: Failed to stbi_load texture");
		return result;
	}

	return result;
}

// Square
Vertex vertices[] = {
	//     Position			      Color          Texture Coor
	{{-0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f}}, // Bottom left
	{{ 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, {1.0f, 0.0f}}, // Bottom Right
	{{ 0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, {1.0f, 1.0f}}, // Top Right
	{{-0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {0.0f, 1.0f}}, // Top Left
};  
unsigned int indices[] = {
	// The indices order in which we draw the two triangles
	0, 1, 2, 2, 3, 0 
};

void init_texture() {

	glUseProgram(shader_programs[SP_2D_BASIC]);
}

void draw_texture(GLuint texture) {
}
