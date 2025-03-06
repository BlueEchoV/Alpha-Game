#pragma once
#include <Audioclient.h>
#include <vector>
#include <unordered_map>

extern std::unordered_map<std::string, Sound> sounds;

// ***My own custom audio library***

// Rendering a stream example:
// https://learn.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream

struct Sound {
    std::vector<unsigned char> file_data; // Entire WAV file loaded into memory
    uint32_t format_offset; // Offset of "fmt " chunk data in fileData
    uint32_t format_size;   // Size of "fmt " chunk data
    uint32_t data_offset; // Offset of "data" chunk data in fileData
    uint32_t data_size;   // Size of "data" chunk data
    WAVEFORMATEX wave_format; // Parsed format for XAudio2
};

enum Chunk_Type {
    CT_Format,
    CT_Data
};

int init_xAudio2();
