#pragma once
#include "Utility.h"
#include <unordered_map>
#include <stdint.h>

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

struct MP_Rect {
	int x, y;
	int w, h;
};

struct MP_Texture {
	GLuint gl_handle;

	UINT32 format;
	int access;
	int w, h;
	int pitch;
	void* pixels;

	MP_BlendMode blend_mode;
	Color_4F mod;
};

enum PACKET_TYPE {
	PT_TEXTURE
};

struct Packet_Texture {
	MP_Texture texture;
};

struct Packet {
	PACKET_TYPE type;

	Packet_Texture packet_texture;

	Color_3F clear_color;
};

struct Open_GL {
	HDC window_dc;
	// NOTE: Going to use only one of each for now
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

struct MP_Renderer {
	Open_GL open_gl;

	std::vector<Packet> packets;

	std::vector<Vertex> vertices;
	std::vector<int> indices;
};

// 2D Vertex for now
struct Vertex {
	V3_F pos;
	Color_3F color;
	V2_F texture_coor;
};

void get_window_size(HWND window, int& w, int& h);
HDC init_open_gl(HWND window);
void load_shaders();

int mp_set_texture_blend_mode(MP_Texture* texture, MP_BlendMode blend_mode);
int mp_set_texture_color_mod(MP_Texture* texture, uint8_t r, uint8_t g, uint8_t b);
int mp_set_texture_alpha_mod(MP_Texture* texture, uint8_t alpha);

MP_Texture* mp_create_texture(MP_Renderer* renderer, uint32_t format, int access, int w, int h);
void mp_destroy_texture(MP_Texture* texture);

int mp_lock_texture(MP_Texture* texture, const MP_Rect* rect, void** pixels, int* pitch);
void mp_unlock_texture(MP_Texture* texture);

MP_Renderer* mp_create_renderer(HINSTANCE hInstance);
MP_Texture mp_create_texture();
void mp_render_clear();
void mp_render_present(MP_Renderer* renderer);
GLuint create_gl_texture(const char* file_path);

void init_texture();
void draw_texture(GLuint texture);

#if 0
struct SDL_Renderer;
SDL_Renderer* MP_CreateRenderer(SDL_Window* window);
int MP_GetRendererOutputSize(SDL_Renderer* sdl_renderer, int* w, int* h);
void MP_DestroyRenderer(SDL_Renderer* renderer);

int MP_SetRenderDrawColor(SDL_Renderer* sdl_renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int MP_GetRenderDrawColor(SDL_Renderer* sdl_renderer, Uint8* r, Uint8* g, Uint8* b, Uint8* a);
int MP_RenderClear(SDL_Renderer* sdl_renderer);
int MP_RenderFillRect(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
int MP_RenderFillRects(SDL_Renderer* sdl_renderer, const SDL_Rect* rects, int count);
int MP_RenderDrawLine(SDL_Renderer* sdl_renderer, int x1, int y1, int x2, int y2);
int MP_RenderDrawLines(SDL_Renderer* sdl_renderer, const SDL_Point* points, int count);
int MP_RenderDrawPoint(SDL_Renderer* sdl_renderer, int x, int y);
int MP_RenderDrawPoints(SDL_Renderer* sdl_renderer, const SDL_Point* points, int count);
int MP_RenderDrawRect(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
int MP_RenderDrawRects(SDL_Renderer* sdl_renderer, const SDL_Rect* rects, int count);

int MP_SetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode);
int MP_GetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode* blendMode);
int MP_SetRenderDrawBlendMode(SDL_Renderer* sdl_renderer, SDL_BlendMode blendMode);
int MP_GetRenderDrawBlendMode(SDL_Renderer* sdl_renderer, SDL_BlendMode *blendMode);

int MP_SetTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b);
int MP_GetTextureColorMod(SDL_Texture* texture, Uint8* r, Uint8* g, Uint8* b);
int MP_SetTextureAlphaMod(SDL_Texture* texture, Uint8 alpha);
int MP_GetTextureAlphaMod(SDL_Texture* texture, Uint8* alpha);

void MP_UnlockTexture(SDL_Texture* texture);
int MP_LockTexture(SDL_Texture* texture, const SDL_Rect* rect, void **pixels, int *pitch);
SDL_Texture* MP_CreateTexture(SDL_Renderer* sdl_renderer, uint32_t format, int access, int w, int h);
int MP_UpdateTexture(SDL_Texture* texture, const SDL_Rect* rect, const void *pixels, int pitch);
void MP_DestroyTexture(SDL_Texture* texture);
int MP_RenderCopy(SDL_Renderer* sdl_renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
int MP_RenderCopyEx(SDL_Renderer* sdl_renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, const float angle, const SDL_Point* center, const SDL_RendererFlip flip);
void MP_RenderPresent(SDL_Renderer* sdl_renderer);

int MP_RenderSetClipRect(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
void MP_RenderGetClipRect(SDL_Renderer* sdl_renderer, SDL_Rect* rect);
SDL_bool MP_RenderIsClipEnabled(SDL_Renderer* sdl_renderer);

int MP_RenderSetViewport(SDL_Renderer* sdl_renderer, const SDL_Rect* rect);
void MP_RenderGetViewport(SDL_Renderer* sdl_renderer, SDL_Rect* rect);

#endif 
