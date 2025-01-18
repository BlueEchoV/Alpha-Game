#include "Renderer.h" 
#include "GL_Functions.h"

#define GL_REPEAT                         0x2901
#define GL_MIRRORED_REPEAT                0x8370

enum SHADER_PROGRAM {
	SP_2D_DRAW,
	SP_2D_TEXTURE
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
	GLuint sp_2d_draw = create_shader_program("shaders\\vs_2d_draw.txt", "shaders\\fs_2d_draw.txt");
	shader_programs[SP_2D_DRAW] = sp_2d_draw;

	GLuint sp_2d_texture = create_shader_program("shaders\\vs_2d_texture.txt", "shaders\\fs_2d_texture.txt");
	shader_programs[SP_2D_TEXTURE] = sp_2d_texture;
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
			200, // Where on my screen it is drawn
			100, // ^^^^
			1600,
			900,
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

	int w, h;
	get_window_size(window, w, h);

	// NOTE: Sets the area of the window where the scene will be drawn. In this case, it 
	//		 starts at the bottom-left corner (0, 0) and stretches to the specified width and height.
	glViewport(0, 0, w, h);

	return window_dc;
}

int MP_GetRenderDrawColor(MP_Renderer* renderer, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	*r = (uint8_t)(255.0f * renderer->draw_color.r);
	*g = (uint8_t)(255.0f * renderer->draw_color.g);
	*b = (uint8_t)(255.0f * renderer->draw_color.b);
	*a = (uint8_t)(255.0f * renderer->draw_color.a);

	return 0;
}

V2 mp_pixel_to_uv(int x, int y, int w, int h) {
	V2 result;

	result.x = (float)x / (float)w;
	result.y = (float)y / (float)h;

	return result;
}

V3 mp_pixel_to_ndc(int x, int y, int window_w, int window_h) {
	V3 result;
	
	result.x = (((float)x / (float)window_w) * 2.0f) - 1.0f;
	result.y = (((float)y / (float)window_h) * 2.0f) - 1.0f;
	result.z = 0.0f;

	return result;
}

int mp_render_fill_rect(MP_Renderer* renderer, const MP_Rect* rect) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	int window_w = renderer->window_width;
	int window_h = renderer->window_height;

	MP_Rect dst;
	if (rect == NULL) {
		dst.x = 0;
		dst.y = 0;
		dst.w = window_w;
		dst.h = window_h;
	} else {
		dst = *rect;
	}

	Packet packet = {};

	packet.type = PT_DRAW;
	packet.packet_draw.mode = GL_TRIANGLES;
	packet.packet_draw.render_draw_color = renderer->draw_color;

	Vertex vertex_1 = {};
	Vertex vertex_2 = {};
	Vertex vertex_3 = {};
	Vertex vertex_4 = {};

	vertex_1.pos = mp_pixel_to_ndc(dst.x	    , dst.y        , window_w, window_h); // Bottom Left
	vertex_2.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y        , window_w, window_h); // Bottom Right
	vertex_3.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y + dst.h, window_w, window_h); // Top Right
	vertex_4.pos = mp_pixel_to_ndc(dst.x        , dst.y + dst.h, window_w, window_h); // Top Left

	vertex_1.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };
	vertex_2.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };
	vertex_3.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };
	vertex_4.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };

	int vbo_starting_index = (int)renderer->vertices.size();
	renderer->vertices.push_back(vertex_1);
	renderer->vertices.push_back(vertex_2);
	renderer->vertices.push_back(vertex_3);
	renderer->vertices.push_back(vertex_4);

	int indices_starting_index = (int)renderer->indices.size();
	renderer->indices.push_back(vbo_starting_index + 0);
	renderer->indices.push_back(vbo_starting_index + 1);
	renderer->indices.push_back(vbo_starting_index + 2);
	renderer->indices.push_back(vbo_starting_index + 2);
	renderer->indices.push_back(vbo_starting_index + 3);
	renderer->indices.push_back(vbo_starting_index + 0);

	packet.packet_draw.indices_array_index = indices_starting_index;
	packet.packet_draw.indices_count = (int)renderer->indices.size() - indices_starting_index;

	renderer->packets.push_back(packet);

	return 0;
}

int mp_render_fill_rects(MP_Renderer* renderer, const MP_Rect* rects, int count) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	for (int i = 0; i < count; i++) {
		mp_render_fill_rect(renderer, &rects[i]);
	}

	return 0;
}

int mp_render_draw_line(MP_Renderer* renderer, int x1, int y1, int x2, int y2) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}
	Packet packet = {};

	packet.type = PT_DRAW;
	packet.packet_draw.mode = GL_LINES;

	Vertex vertex_1 = {};
	Vertex vertex_2 = {};

	vertex_1.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };
	vertex_2.color = { renderer->draw_color.r, renderer->draw_color.g, renderer->draw_color.b, 1.0f };

	vertex_1.pos = mp_pixel_to_ndc(x1, y1, renderer->window_width, renderer->window_height);
	vertex_2.pos = mp_pixel_to_ndc(x2, y2, renderer->window_width, renderer->window_height);

	int vertices_starting_index = (int)renderer->vertices.size();

	renderer->vertices.push_back(vertex_1);
	renderer->vertices.push_back(vertex_2);

	int indices_starting_index = (int)renderer->indices.size();

	renderer->indices.push_back(vertices_starting_index + 0);
	renderer->indices.push_back(vertices_starting_index + 1);

	int indices_ending_index = (int)renderer->indices.size();

	packet.packet_draw.indices_array_index = indices_starting_index;
	packet.packet_draw.indices_count = indices_ending_index - indices_starting_index;

	renderer->packets.push_back(packet);

	return 0;
}

int mp_render_draw_lines(MP_Renderer* renderer, const MP_Point* points, int count) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	for (int i = 0; i < count - 1; i++) {
		mp_render_draw_line(renderer, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
	}

	return 0;
}

int mp_render_draw_point(MP_Renderer* renderer, int x, int y) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}
	MP_Rect rect = {};

	rect.w = 4;
	rect.h = 4;
	rect.x = x - rect.w / 2;
	rect.y = y - rect.h / 2;

	mp_render_fill_rect(renderer, &rect);

	return 0;
}

int mp_render_draw_points(MP_Renderer* renderer, const MP_Point* points, int count) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	for (int i = 0; i < count; i++) {
		mp_render_draw_point(renderer, points[i].x, points[i].y);
	}

	return 0;
}

int mp_render_draw_rect(MP_Renderer* renderer, const MP_Rect* rect) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	mp_render_draw_line(renderer, rect->x		   , rect->y          , rect->x + rect->w, rect->y			);
	mp_render_draw_line(renderer, rect->x + rect->w, rect->y          , rect->x + rect->w, rect->y + rect->h);
	mp_render_draw_line(renderer, rect->x + rect->w, rect->y + rect->h, rect->x          , rect->y + rect->h);
	mp_render_draw_line(renderer, rect->x		   , rect->y + rect->h, rect->x          , rect->y			);

	return 0;
}

int mp_render_draw_rects(MP_Renderer* renderer, const MP_Rect* rects, int count) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	for (int i = 0; i < count; i++) { 
		mp_render_draw_rect(renderer, &rects[i]);
	}

	return 0;
}

#if 0
int mp_set_texture_color_mod(MP_Texture* texture, uint8_t r, uint8_t g, uint8_t b);
int mp_get_texture_color_mod(MP_Texture* texture, uint8_t* r, uint8_t* g, uint8_t* b);
int mp_set_texture_alpha_mod(MP_Texture* texture, uint8_t alpha);
int mp_get_texture_alpha_mod(MP_Texture* texture, uint8_t* alpha);
#endif

int mp_set_render_draw_color(MP_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	if (renderer == NULL) {
		log("Error: renderer is null");
		return -1;
	}

	renderer->draw_color = { (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f };

	return 0;
}

void mp_render_set_viewport(MP_Renderer* renderer, const MP_Rect* rect) {
	if (renderer == NULL) {
		log("Error: Renderer is NULL");
		return;
	}

	glViewport(rect->x, rect->y, rect->w, rect->h);
}

int mp_set_render_draw_color(MP_Renderer* renderer, Color c) {
    switch (c) {
        case C_Red:
            mp_set_render_draw_color(renderer, 255, 0, 0, 255);
            break;
        case C_Green:
            mp_set_render_draw_color(renderer, 0, 255, 0, 255); 
            break;
        case C_Blue:
            mp_set_render_draw_color(renderer, 0, 0, 255, 255);
            break;
        case C_Orange:
            mp_set_render_draw_color(renderer, 255, 165, 0, 255); 
            break;
        case C_Dark_Yellow:
            mp_set_render_draw_color(renderer, 204, 204, 0, 255); 
            break;
        case C_Dark_Blue:
            mp_set_render_draw_color(renderer, 0, 0, 139, 255);
            break;
        default:
            return -1;
    }
    return 0; 
}

static inline int64_t GetTicks() {
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks)) {
		log("Error: QueryPerformanceCounter failed.");
		assert(false);
    }
    return ticks.QuadPart;
}

LARGE_INTEGER start_time = {};
LARGE_INTEGER frequency = {};

void mp_init_ticks() {
	// The count itself
    QueryPerformanceCounter(&start_time); 
	// How many counts there are per second
	QueryPerformanceFrequency(&frequency);  
}

// Get the number of milliseconds since MP library initialization.
// Requires for the renderer to be created for it to work
uint64_t mp_get_ticks_64() {
	uint64_t elapsed = {};
	LARGE_INTEGER current_time = {};

    QueryPerformanceCounter(&current_time); 

	// Time in seconds
	elapsed = (current_time.QuadPart - start_time.QuadPart) / (frequency.QuadPart / 1000);

	return elapsed;
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
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);  
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coor));
	glEnableVertexAttribArray(2);  

	// For transparency in textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	load_shaders();

	mp_init_ticks();

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

// NULL for the entire texture. NULL for the entire rendering target.
void mp_render_copy(MP_Renderer* renderer, MP_Texture* texture, const MP_Rect* src_rect, const MP_Rect* dst_rect) {
	if (renderer == NULL) {
		log("Error: renderer is NULL");
		return;
	}
	// Texture Packet
	Packet result;

	result.type = PT_TEXTURE;
	result.packet_texture.texture = texture;

	Vertex vertex_1 = {};
	Vertex vertex_2 = {};
	Vertex vertex_3 = {};
	Vertex vertex_4 = {};

	MP_Rect dst = {};
	int window_w = renderer->window_width; 
	int	window_h = renderer->window_height;
	if (dst_rect == NULL) {
		dst.x = 0;
		dst.y = 0;
		dst.w = window_w;
		dst.h = window_h;
	} else {
		dst = *dst_rect;
	}

	// NOTE: My coordinate system draws from the bottom left
	vertex_1.pos = mp_pixel_to_ndc(dst.x	    , dst.y		   , window_w, window_h); // Bottom left (Starting point)
	vertex_2.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y		   , window_w, window_h); // Bottom right 
	vertex_3.pos = mp_pixel_to_ndc(dst.x + dst.w, dst.y + dst.h, window_w, window_h); // Top right 
	vertex_4.pos = mp_pixel_to_ndc(dst.x		, dst.y + dst.h, window_w, window_h); // Top left 

	MP_Rect src = {};
	if (src_rect == NULL) {
		src.x = 0;
		src.y = 0;
		src.w = texture->w;
		src.h = texture->h;
	} else {
		src = *src_rect;
	}

	vertex_1.texture_coor = mp_pixel_to_uv(src.x		, src.y        , texture->w, texture->h); // Bottom left
	vertex_2.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y		   , texture->w, texture->h); // Bottom right
	vertex_3.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y + src.h, texture->w, texture->h); // Top right
	vertex_4.texture_coor = mp_pixel_to_uv(src.x		, src.y + src.h, texture->w, texture->h); // Top left

	Color_4F m = texture->mod;
	vertex_1.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_2.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_3.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_4.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};

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

int mp_render_copy_ex(MP_Renderer* renderer, MP_Texture* texture, const MP_Rect* src_rect, const MP_Rect* dst_rect, 
					  const float angle, const MP_Point* center, const MP_RendererFlip flip) {
	if (renderer == NULL) {
		log("Error: Renderer is null");
		return -1;
	}

	REF(angle);
	// Texture Packet
	Packet result;

	result.type = PT_TEXTURE;
	result.packet_texture.texture = texture;

	Vertex vertex_1 = {};
	Vertex vertex_2 = {};
	Vertex vertex_3 = {};
	Vertex vertex_4 = {};

	MP_Rect dst = {};
	int window_w = renderer->window_width; 
	int	window_h = renderer->window_height;
	if (dst_rect == NULL) {
		dst.x = 0;
		dst.y = 0;
		dst.w = window_w;
		dst.h = window_h;
	} else {
		dst = *dst_rect;
	}

	MP_Point c = {};
	if (center == NULL) {
		c.x = (dst.w / 2) + dst.x;
		c.y = (dst.h / 2) + dst.y;
	} else {
		c = *center;
	}

	V2 bottom_left =  { (float)dst.x 	           , (float)dst.y	             };
	V2 top_left =     { (float)dst.x		       , (float)dst.y + (float)dst.h };
	V2 top_right =    { (float)dst.x + (float)dst.w, (float)dst.y + (float)dst.h };
	V2 bottom_right = { (float)dst.x + (float)dst.w, (float)dst.y		         };

	if (angle != 0) {
		bottom_left = rotate_point_based_off_angle(angle,  (float)c.x, (float)c.y, bottom_left.x, bottom_left.y);
		top_left = rotate_point_based_off_angle(angle,     (float)c.x, (float)c.y, top_left.x, top_left.y);
		top_right = rotate_point_based_off_angle(angle,    (float)c.x, (float)c.y, top_right.x, top_right.y);
		bottom_right = rotate_point_based_off_angle(angle, (float)c.x, (float)c.y, bottom_right.x, bottom_right.y);
	}

	// NOTE: My coordinate system draws from the bottom left
	vertex_1.pos = mp_pixel_to_ndc((int)bottom_left.x,  (int)bottom_left.y, window_w, window_h); 
	vertex_2.pos = mp_pixel_to_ndc((int)bottom_right.x, (int)bottom_right.y, window_w, window_h);
	vertex_3.pos = mp_pixel_to_ndc((int)top_right.x,    (int)top_right.y, window_w, window_h);
	vertex_4.pos = mp_pixel_to_ndc((int)top_left.x,     (int)top_left.y, window_w, window_h); 

	MP_Rect src = {};
	if (src_rect == NULL) {
		src.x = 0;
		src.y = 0;
		src.w = texture->w;
		src.h = texture->h;
	} else {
		src = *src_rect;
	}

	if (flip == SDL_FLIP_HORIZONTAL) {
		vertex_1.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y        , texture->w, texture->h); // Bottom right 
		vertex_2.texture_coor = mp_pixel_to_uv(src.x	    , src.y        , texture->w, texture->h); // Bottom left 
		vertex_3.texture_coor = mp_pixel_to_uv(src.x        , src.y + src.h, texture->w, texture->h); // Top left 
		vertex_4.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y + src.h, texture->w, texture->h); // Top right 
	} else if (flip == SDL_FLIP_VERTICAL) {
		vertex_1.texture_coor = mp_pixel_to_uv(src.x + src.h, src.y + src.h, texture->w, texture->h); // Top right 
		vertex_2.texture_coor = mp_pixel_to_uv(src.x        , src.y + src.h, texture->w, texture->h); // Top left
		vertex_3.texture_coor = mp_pixel_to_uv(src.x        , src.y        , texture->w, texture->h); // Bottom left
		vertex_4.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y        , texture->w, texture->h); // Bottom right
	} else if (flip == SDL_FLIP_NONE) {
		vertex_1.texture_coor = mp_pixel_to_uv(src.x        , src.y        , texture->w, texture->h); // Bottom left
		vertex_2.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y        , texture->w, texture->h); // Bottom right
		vertex_3.texture_coor = mp_pixel_to_uv(src.x + src.w, src.y + src.h, texture->w, texture->h); // Top right
		vertex_4.texture_coor = mp_pixel_to_uv(src.x        , src.y + src.h, texture->w, texture->h); // Top left
	}

	Color_4F m = texture->mod;
	vertex_1.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_2.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_3.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};
	vertex_4.color = { 1.0f * m.r, 1.0f * m.g, 1.0f * m.b, 1.0f * m.a};

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

	return 0;
}

void gl_set_blend_mode(MP_BlendMode blend_mode) {
    switch (blend_mode) {
        case MP_BLENDMODE_NONE:
            glDisable(GL_BLEND);
            break;

        case MP_BLENDMODE_BLEND:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case MP_BLENDMODE_ADD:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;

        case MP_BLENDMODE_MOD:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;

        case MP_BLENDMODE_MUL:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;

        default:
            glDisable(GL_BLEND);
			log("Error: Blend mode not supported");
            break;
    }
}

void mp_render_present(MP_Renderer* renderer) {
	if (renderer == NULL) {
		log("Error: renderer is NULL");
		return;
	}
	// NOTE: Buffer the data when I create the renderer (empty buffer)
	glBindBuffer(GL_ARRAY_BUFFER, renderer->open_gl.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->open_gl.ebo);

	// NOTE: GL_BUFFER_SUB_DATA TO SAVE TIME
	glBufferData(GL_ARRAY_BUFFER, renderer->vertices.size() * sizeof(Vertex), renderer->vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderer->indices.size() * sizeof(unsigned int), renderer->indices.data(), GL_STATIC_DRAW);

	// NOTE: One VAO for now
	glBindVertexArray(renderer->open_gl.vao);
	
	for (Packet& packet : renderer->packets) {
		if (packet.type == PT_DRAW) {
			GLuint shader = shader_programs[SP_2D_DRAW];
			glUseProgram(shader);

			int index = packet.packet_draw.indices_array_index;
			int count = packet.packet_draw.indices_count;
			glDrawElements((GLenum)packet.packet_draw.mode, count, GL_UNSIGNED_INT, (void*)(index * sizeof(unsigned int)));
		}
		else if (packet.type == PT_TEXTURE) {
			// gl_set_blend_mode(packet.packet_texture.texture->blend_mode);

			GLuint shader = shader_programs[SP_2D_TEXTURE];
			glUseProgram(shader);

			// GLint samplerLocation = glGetUniformLocation(shader, "my_texture");
			// glUniform1i(samplerLocation, 0); // Assign texture unit 0 to the sampler

			glActiveTexture(GL_TEXTURE0);      // Activate texture unit 0
			glBindTexture(GL_TEXTURE_2D, packet.packet_texture.texture->gl_handle);

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
		else if (packet.type == PT_CLEAR) {
			Color_4F c = packet.packet_clear.clear_color;
			glClearColor(c.r, c.g, c.b, c.r);

			// NOTE: Clears the window to the color set by glClearColor. It refreshes the color buffer, 
			//		 preparing it for new drawing.
			glClear(GL_COLOR_BUFFER_BIT);
		} else {
			log("Error: Packet type isn't specified");
			assert(false);
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

	if (r > 255 || g > 255 || b > 255 ||
		r < 0   || g < 0   || b < 0) {
		log("Error: Color mod not supported");
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
	mp_set_texture_blend_mode(result, MP_BLENDMODE_BLEND);
	mp_set_texture_color_mod(result, 255, 255, 255);
	mp_set_texture_alpha_mod(result, 255);

	glGenTextures(1, &result->gl_handle);  

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, result->gl_handle);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// NOTE: A common mistake is to set one of the mipmap filtering options as the magnification 
	// filter. This doesn't have any effect since mipmaps are primarily used for when textures
	// get downscaled: texture magnification doesn't use mipmaps and giving it a mipmap filtering
	// option will generate an OpenGL GL_INVALID_ENUM error code.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
