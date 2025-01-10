#pragma once
#include "Windows_Platform.h"
#include <unordered_map>
#include <stdint.h>

enum Color {
	C_Red,
	C_Green,
	C_Blue,
	C_Orange,
	C_Dark_Yellow,
	C_Dark_Blue,
	C_Total
};

typedef unsigned int GLuint;

typedef enum MP_TextureAccess
{
    MP_TEXTUREACCESS_STATIC,    /**< Changes rarely, not lockable */
    MP_TEXTUREACCESS_STREAMING, /**< Changes frequently, lockable */
    MP_TEXTUREACCESS_TARGET     /**< Texture can be used as a render target */
} MP_TextureAccess;

typedef enum MP_BlendMode
{
    MP_BLENDMODE_NONE = 0x00000000,     /**< no blending
                                              dstRGBA = srcRGBA */
    MP_BLENDMODE_BLEND = 0x00000001,    /**< alpha blending
                                              dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
                                              dstA = srcA + (dstA * (1-srcA)) */
    MP_BLENDMODE_ADD = 0x00000002,      /**< additive blending
                                              dstRGB = (srcRGB * srcA) + dstRGB
                                              dstA = dstA */
    MP_BLENDMODE_MOD = 0x00000004,      /**< color modulate
                                              dstRGB = srcRGB * dstRGB
                                              dstA = dstA */
    MP_BLENDMODE_MUL = 0x00000008,      /**< color multiply
                                              dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA))
                                              dstA = dstA */
    MP_BLENDMODE_INVALID = 0x7FFFFFFF
} MP_BlendMode;

typedef enum MP_RendererFlip
{
    SDL_FLIP_NONE = 0x00000000,			 /**< Do not flip */
    SDL_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
    SDL_FLIP_VERTICAL = 0x00000002       /**< flip vertically */
} MP_RendererFlip;

struct MP_Point {
	int x, y;
};

struct MP_Rect {
	int x, y;
	int w, h;
};

struct MP_Texture {
	GLuint gl_handle;

	int access;
	int w, h;
	int pitch;
	void* pixels;
	MP_Rect portion;

	MP_BlendMode blend_mode;
	Color_4F mod;
};

enum PACKET_TYPE {
	PT_DRAW,
	PT_TEXTURE,
	PT_CLEAR
};

struct Packet_Draw {
	uint32_t mode;
	Color_4F render_draw_color;
	int indices_array_index;
	int indices_count;
};

struct Packet_Texture {
	MP_Texture* texture;
	int indices_array_index;
	int indices_count;
};

struct Packet_Clear {
	Color_4F clear_color;
};

struct Packet {
	PACKET_TYPE type;

	Packet_Draw packet_draw;
	Packet_Texture packet_texture;
	Packet_Clear packet_clear;
};

struct Open_GL {
	HWND window_handle;
	HDC window_dc;
	// NOTE: Going to use only one of each for now
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

// 2D Vertex for now
struct Vertex {
	V3_F pos;
	Color_4F color;
	V2_F texture_coor;
};

struct MP_Renderer {
	Open_GL open_gl;

	Color_4F draw_color;

	std::vector<Packet> packets;

	std::vector<Vertex> vertices;
	std::vector<int> indices;

	int window_width;
	int window_height;
};

void get_window_size(HWND window, int& w, int& h);

HDC init_open_gl(HWND window);
void load_shaders();

int mp_get_render_draw_color(MP_Renderer* renderer, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
int mp_set_render_draw_color(MP_Renderer* renderer, Color c);

int mp_render_fill_rect(MP_Renderer* renderer, const MP_Rect* rect);
int mp_render_fill_rects(MP_Renderer* renderer, const MP_Rect* rects, int count);
int mp_render_draw_line(MP_Renderer* renderer, int x1, int y1, int x2, int y2);
int mp_render_draw_lines(MP_Renderer* renderer, const MP_Point* points, int count);
int mp_render_draw_point(MP_Renderer* renderer, int x, int y);
int mp_render_draw_points(MP_Renderer* renderer, const MP_Point* points, int count);
int mp_render_draw_rect(MP_Renderer* renderer, const MP_Rect* rect);
int mp_render_draw_rects(MP_Renderer* renderer, const MP_Rect* rects, int count);

int mp_set_texture_blend_mode(MP_Texture* texture, MP_BlendMode blend_mode);
int mp_set_texture_color_mod(MP_Texture* texture, uint8_t r, uint8_t g, uint8_t b);
int mp_get_texture_color_mod(MP_Texture* texture, uint8_t* r, uint8_t* g, uint8_t* b);
int mp_set_texture_alpha_mod(MP_Texture* texture, uint8_t alpha);
int mp_get_texture_alpha_mod(MP_Texture* texture, uint8_t* alpha);

MP_Texture* mp_create_texture(MP_Renderer* renderer, uint32_t format, int access, int w, int h);
void mp_destroy_texture(MP_Texture* texture);
void mp_render_copy(MP_Renderer* renderer, MP_Texture* texture, const MP_Rect* src_rect, const MP_Rect* dst_rect);
int mp_render_copy_ex(MP_Renderer* renderer, MP_Texture* texture, const MP_Rect* src_rect, const MP_Rect* dst_rect, 
				      const float angle, const MP_Point* center, const MP_RendererFlip flip);

int mp_lock_texture(MP_Texture* texture, const MP_Rect* rect, void** pixels, int* pitch);
void mp_unlock_texture(MP_Texture* texture);

int mp_set_render_draw_color(MP_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

MP_Renderer* mp_create_renderer(HINSTANCE hInstance);
// void MP_DestroyRenderer(SDL_Renderer* renderer);
void mp_render_clear(MP_Renderer* renderer);
void mp_render_present(MP_Renderer* renderer);

uint64_t mp_get_ticks_64();

#if 0
int MP_SetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode);
int MP_GetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode* blendMode);
int MP_SetRenderDrawBlendMode(SDL_Renderer* sdl_renderer, SDL_BlendMode blendMode);
int MP_GetRenderDrawBlendMode(SDL_Renderer* sdl_renderer, SDL_BlendMode *blendMode);

int MP_UpdateTexture(SDL_Texture* texture, const SDL_Rect* rect, const void *pixels, int pitch);
void MP_DestroyTexture(SDL_Texture* texture);
int MP_RenderCopyEx(SDL_Renderer* sdl_renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, const float angle, const SDL_Point* center, const SDL_RendererFlip flip);

int MP_RenderSetClipRect(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
void MP_RenderGetClipRect(SDL_Renderer* sdl_renderer, SDL_Rect* rect);
SDL_bool MP_RenderIsClipEnabled(SDL_Renderer* sdl_renderer);

int MP_RenderSetViewport(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
void MP_RenderGetViewport(SDL_Renderer* sdl_renderer, SDL_Rect* rect);

#endif 
