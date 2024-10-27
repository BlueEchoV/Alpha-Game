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

int mp_set_render_draw_color(MP_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	renderer->draw_color = { (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f };

	return 0;
}

MP_Renderer* mp_create_renderer(HINSTANCE hInstance) {
	MP_Renderer* renderer = new MP_Renderer();

	renderer->draw_color = { 255, 255, 255, 255 };

	renderer->open_gl.window_handle = init_windows(hInstance);
	renderer->open_gl.window_dc = init_open_gl(renderer->open_gl.window_handle);

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

	// For transparency in textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	load_shaders();

	return renderer;
}

void mp_render_clear(MP_Renderer* renderer) {
	if (renderer == NULL) {
		log("Error: renderer is NULL");
		return;
	}

	Packet packet = {};
	packet.type = PT_CLEAR;
	packet.packet_clear.clear_color = { renderer->draw_color };

	renderer->packets.push_back(packet);
}

// int mp_set_texture_color_mod(MP_Texture* texture) {
// 
// }

// Copy a portion of the texture to the current renderer target

V2_F mp_pixel_to_uv(int x, int y, int w, int h) {
	V2_F result;

	result.x = (float)x / (float)w;
	result.y = (float)y / (float)h;

	return result;
}

V3_F mp_pixel_to_ndc(int x, int y, int w, int h) {
	V3_F result;
	
	result.x = (((float)x / (float)w)  * 2.0f) - 1.0f;
	result.y = (((float)y / (float)h) * 2.0f) - 1.0f;
	result.z = 0.0f;

	return result;
}

// NULL for the entire texture. NULL for the entire rendering target.
void mp_render_copy(MP_Renderer* renderer, MP_Texture* texture, const MP_Rect* src_rect, const MP_Rect* dst_rect) {
	// Texture Packet
	Packet result;

	result.type = PT_TEXTURE;
	result.packet_texture.texture = texture;

	Vertex vertex_1 = {};
	Vertex vertex_2 = {};
	Vertex vertex_3 = {};
	Vertex vertex_4 = {};

	MP_Rect dst = {};
	if (dst_rect == NULL) {
		dst.x = 0;
		dst.y = 0;
		get_window_size(renderer->open_gl.window_handle, dst.w, dst.h);
	} else {
		dst = *dst_rect;
	}

	// NOTE: My coordinate system draws from the bottom left
	vertex_1.pos = mp_pixel_to_ndc(dst.x	    , dst.y		   , dst.w, dst.h); // Bottom left (Starting point)
	vertex_2.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y		   , dst.w, dst.h); // Bottom right 
	vertex_3.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y + dst.h, dst.w, dst.h); // Top right 
	vertex_4.pos = mp_pixel_to_ndc(dst.x		, dst.y + dst.h, dst.w, dst.h); // Top left 

	MP_Rect src = {};
	if (src_rect == NULL) {
		src.x = 0;
		src.y = 0;
		src.w = texture->w;
		src.h = texture->h;
	} else {
		src = *src_rect;
	}

	vertex_1.texture_coor = mp_pixel_to_uv(src.x		, src.y        , src.w, src.h); // Bottom left
	vertex_2.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y		   , src.w, src.h); // Bottom right
	vertex_3.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y + src.h, src.w, src.h); // Top right
	vertex_4.texture_coor = mp_pixel_to_uv(src.x		, src.y + src.h, src.w, src.h); // Top left

	vertex_1.color = { 1.0f, 0.0f, 0.0f };
	vertex_2.color = { 0.0f, 1.0f, 0.0f };
	vertex_3.color = { 0.0f, 0.0f, 1.0f };
	vertex_4.color = { 1.0f, 1.0f, 1.0f };

	int vbo_starting_index = (int)renderer->vertices.size();
	renderer->vertices.push_back(vertex_1);
	renderer->vertices.push_back(vertex_2);
	renderer->vertices.push_back(vertex_3);
	renderer->vertices.push_back(vertex_4);

	// retain the indices positions (size subtraction)
	// push back the indices (Draw Order - 6)

	result.packet_texture.indices_array_index = (int)renderer->indices.size();

	int current_indices = (int)renderer->indices.size();
	renderer->indices.push_back(vbo_starting_index + 0);
	renderer->indices.push_back(vbo_starting_index + 1);
	renderer->indices.push_back(vbo_starting_index + 2);
	renderer->indices.push_back(vbo_starting_index + 2);
	renderer->indices.push_back(vbo_starting_index + 3);
	renderer->indices.push_back(vbo_starting_index + 0);
	result.packet_texture.indices_count = (int)renderer->indices.size() - current_indices;

	renderer->packets.push_back(result);
}

void mp_render_present(MP_Renderer* renderer) {
	// NOTE: Buffer the data when I create the renderer (empty buffer)
	glBindBuffer(GL_ARRAY_BUFFER, renderer->open_gl.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->open_gl.ebo);

	// NOTE: GL_BUFFER_SUB_DATA TO SAVE TIME
	glBufferData(GL_ARRAY_BUFFER, renderer->vertices.size() * sizeof(Vertex), renderer->vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderer->indices.size() * sizeof(unsigned int), renderer->indices.data(), GL_STATIC_DRAW);

	// NOTE: One VAO for now
	glBindVertexArray(renderer->open_gl.vao);
	
	for (Packet& packet : renderer->packets) {
		if (packet.type == PT_TEXTURE) {
			glBindTexture(GL_TEXTURE_2D, packet.packet_texture.texture->gl_handle);

			GLuint shader = shader_programs[SP_2D_BASIC];
			glUseProgram(shader);

			// ***Why choose glDrawElements***
			// As you can see, there is some overlap on the vertices specified. 
			// We specify bottom right and top left twice! This is an overhead 
			// of 50% since the same rectangle could also be specified with only 
			// 4 vertices, instead of 6. This will only get worse as soon as we 
			// have more complex models that have over 1000s of triangles where 
			// there will be large chunks that overlap
			int index = packet.packet_texture.indices_array_index;
			int count = packet.packet_texture.indices_count;
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(index * sizeof(unsigned int)));
		} 
		if (packet.type == PT_CLEAR) {
			Color_4F c = packet.packet_clear.clear_color;
			glClearColor(c.r, c.g, c.b, c.r);

			// NOTE: Clears the window to the color set by glClearColor. It refreshes the color buffer, 
			//		 preparing it for new drawing.
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
	SwapBuffers(renderer->open_gl.window_dc);

	renderer->indices.clear();
	renderer->vertices.clear();
	renderer->packets.clear();
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

	texture->mod.r = (float)r / 255.0f;
	texture->mod.g = (float)g / 255.0f; 
	texture->mod.b = (float)b / 255.0f;

	return 0;
}

int mp_set_texture_alpha_mod(MP_Texture* texture, uint8_t a) {
	if (texture == NULL) {
		log("Error: Texture is NULL");
		return -1;
	}

	texture->mod.a = (float)a / 255.0f;

	return 0;
}

// ***Parameters***
// Format: Just set it to 0
//		   RGBA format by default
// Access: Just set it to 0
//		   SDL_TEXTUREACCESS_STATIC	   → glTexImage2D			(for rarely changing textures)
//		   SDL_TEXTUREACCESS_STREAMING → glTexSubImage2D		(for frequently updated textures)
//		   SDL_TEXTUREACCESS_TARGET    → glFramebufferTexture2D (for render targets)
MP_Texture* mp_create_texture(MP_Renderer* renderer, uint32_t format, int access, int w, int h) {
	REF(format);
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return nullptr;
	}

	MP_Texture* result = new MP_Texture();
	result->w = w;
	result->h = h;
	result->pitch = 0;
	result->pixels = NULL;
	result->access = access;

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

// NOTE: Allocates a buffer on the CPU side
int mp_lock_texture(MP_Texture* texture, const MP_Rect* rect, void** pixels, int* pitch) {
	if (texture == NULL) {
		log("Error: texture is NULL");
		return -1;
	}

	if (rect == NULL) {
		texture->portion.w = texture->w;
		texture->portion.h = texture->h;
		texture->portion.x = 0;
		texture->portion.y = 0;
	} else {
		if (rect->x < 0 || 
			rect->y < 0 || 
			rect->x > (rect->x + rect->w) || 
			rect->y > (rect->y + rect->h)) {
			log("Error: texture portion out of bounds");
			return -1;
		} else {
			texture->portion = *rect;
		}
	}

	//											  4 bytes per pixel
	int buffer_size = (texture->w * texture->h) * 4;
	uint8_t* buffer = new uint8_t[buffer_size];

	*pixels = buffer;
	*pitch = texture->w * 4;
	texture->pixels = buffer;

	return 0;
}

// NOTE: Uploads to the GPU
void mp_unlock_texture(MP_Texture* texture) {
	if (texture == NULL) {
		log("Error: texture is NULL");
		return;
	}

	glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		texture->portion.x,
		texture->portion.y,
		texture->portion.w,
		texture->portion.h,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		texture->pixels
	);

	delete texture->pixels;
	texture->pixels = nullptr;
}

#if 0
MP_Texture* mp_create_texture(const char* file_path) {
	MP_Texture* result;
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
#endif
