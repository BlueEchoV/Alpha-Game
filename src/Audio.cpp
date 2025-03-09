#include "Audio.h"

#include <windows.h>
#include <stdexcept>
#include <xaudio2.h>
#include <array>
#include <stdio.h>

// *****Constant Literals*****
// 16 bits per sample.
constexpr uint16_t BITS_PER_SAMPLE = 16;
// 44,100 samples per second.
constexpr uint32_t SAMPLES_PER_SEC = 44100;
// 220 cycles per second (frequency of the audible tone).
constexpr double CYCLES_PER_SEC = 220.0;
// 50% volume.
constexpr double VOLUME = 0.5;
// 10 cycles per audio buffer.
constexpr uint16_t AUDIO_BUFFER_SIZE_IN_CYCLES = 10;
constexpr double PI = 3.14159265358979323846;

// *****Calculated Constants*****
// 200 samples per cycle.
constexpr uint16_t SAMPLES_PER_CYCLE = (uint16_t)(SAMPLES_PER_SEC / CYCLES_PER_SEC);
// 2,000 samples per buffer.
constexpr uint16_t AUDIO_BUFFER_SIZE_IN_SAMPLES = SAMPLES_PER_CYCLE * AUDIO_BUFFER_SIZE_IN_CYCLES;
// 4,000 bytes per buffer.
constexpr uint32_t AUDIO_BUFFER_SIZE_IN_BYTES = uint32_t(AUDIO_BUFFER_SIZE_IN_SAMPLES * (BITS_PER_SAMPLE / 8));

// The actual audio buffer
std::array<byte, AUDIO_BUFFER_SIZE_IN_BYTES> audio_buffer = {};

// Pointer to the main XAudio2 engine instance. This represents the core audio system 
// object responsible for managing audio processing, creating voices, and handling audio 
// playback on the system. 
IXAudio2* m_xAudio2 = nullptr; 
// Pointer to the mastering voice, which is the final output voice in the XAudio2 system. 
// It handles the audio output to the system’s audio device (e.g., speakers or headphones) 
// and applies global audio effects or volume control before the sound is played. 
IXAudio2MasteringVoice* m_masteringVoice = nullptr; 
// Pointer to a source voice, which is used to play a specific audio buffer or stream 
// (e.g., a sound effect or music track). It manages the playback, pitch, volume, and 
// other properties of an individual audio source, feeding data into the mastering 
// voice for output. This handles the threading in the XAudio2 api.
IXAudio2SourceVoice* m_pXAudio2SourceVoice = nullptr; 

int init_xAudio2() {
    // #1 - Initialize COM
    // HRRESULT is for error checking
	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (SUCCEEDED(hr)) {
        // #2 - Create the XAudio2 engine object
        hr = ::XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if (FAILED(hr)) {
            // Clean up COM
            ::CoUninitialize(); 
            return -1;        
        }

        // #3 - Create a mastering voice (connect to the default audio device)
        hr = m_xAudio2->CreateMasteringVoice(
            &m_masteringVoice,              // Pointer to receive the mastering voice
            XAUDIO2_DEFAULT_CHANNELS,       // Use default number of channels (e.g., stereo)
            XAUDIO2_DEFAULT_SAMPLERATE,     // Use default sample rate (e.g., 44.1 kHz)
            0,                              // No flags
            nullptr,                        // No device ID (use default device)
            nullptr                         // No effect chain
        );
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        // Define a format
        WAVEFORMATEX wave_format_ex = {};
        wave_format_ex.wFormatTag = WAVE_FORMAT_PCM;
        wave_format_ex.nChannels = 1; // 1 channel
        wave_format_ex.nSamplesPerSec = SAMPLES_PER_SEC;
        wave_format_ex.nBlockAlign = (wave_format_ex.nChannels * BITS_PER_SAMPLE) / 8;
        wave_format_ex.nAvgBytesPerSec = wave_format_ex.nSamplesPerSec * wave_format_ex.nBlockAlign;
        wave_format_ex.wBitsPerSample = BITS_PER_SAMPLE;
        wave_format_ex.cbSize = 0;

        // CreateSourceVoice allows you to play multiple independent sounds simultaneously 
        // (e.g., background music, sound effects, or tones) by creating multiple source voices, 
        // each with its own configuration.
        // XAudio2 makes an internal copy of the buffer when you submit it.
        hr = m_xAudio2->CreateSourceVoice(&m_pXAudio2SourceVoice, &wave_format_ex);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

		double phase = {};
        uint32_t buffer_index = {};
        while (buffer_index < AUDIO_BUFFER_SIZE_IN_BYTES)
        {
            phase += (2 * PI) / SAMPLES_PER_CYCLE;
            int16_t sample = (int16_t)(sin(phase) * INT16_MAX * VOLUME);
            // Values are little-endian.
            audio_buffer[buffer_index++] = (byte)sample; 
            audio_buffer[buffer_index++] = (byte)(sample >> 8);
        }

		XAUDIO2_BUFFER x_audio_2_buffer = {};
        x_audio_2_buffer.Flags = XAUDIO2_END_OF_STREAM;
        x_audio_2_buffer.AudioBytes = AUDIO_BUFFER_SIZE_IN_BYTES;
        x_audio_2_buffer.pAudioData = audio_buffer.data();
        x_audio_2_buffer.PlayBegin = 0;
        x_audio_2_buffer.PlayLength = 0;
        x_audio_2_buffer.LoopBegin = 0;
        x_audio_2_buffer.LoopLength = 0;
        x_audio_2_buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

        hr = m_pXAudio2SourceVoice->SubmitSourceBuffer(&x_audio_2_buffer);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        // This function returns immediately, but the audio continues playing independently.
        hr = m_pXAudio2SourceVoice->Start(0);
        if (FAILED(hr)) {
            if (m_xAudio2) m_xAudio2->Release(); 
            ::CoUninitialize(); 
            return -1; 
        }

        return 0; 
    }
    return -1;
}

void clean_up_audio() {
    ::CoUninitialize(); 
}

/*
void read_chunck(Chunk_Type chunk_type, ) {
    if (chunk_type == );

}
*/

// Load the wav file and grab all the data inside of it
bool load_wav_file(const char* file_name, Sound& sound) {
    // Step 1: Open the file with fopen in binary mode
    // NOTE: Read as raw bytes (8-bit value (0 - 255))
    FILE* file = fopen(file_name, "rb");
    if (!file) {
        log("Error: Wave file did not open correctly.");
        return false;
    }

    // Step 2: Get the file size
    fseek(file, 0, SEEK_END); // Move to the end of the file
    uint32_t file_size_in_bytes = ftell(file); // Get position in bytes (size of file)
    fseek(file, 0, SEEK_SET); // Move back to the start

    // Step 3: Allocate buffer and read entire file into memory
    sound.file_data.resize(file_size_in_bytes);
    size_t bytes_read = fread(sound.file_data.data(), 1, file_size_in_bytes, file);
    fclose(file);

    if (bytes_read != (size_t)file_size_in_bytes) {
        log("Error: fread returned invalid number of bytes read for loading the wav file");
        return false;
    }

    // Step 4: Verify it's a WAVE file (check RIFF and WAVE identifiers)
    // NOTE: These three checks are enough to verify if it's a WAV file
    if (file_size_in_bytes < 12 || // At least 12 bytes (4 bytes for RIFF, 4 bytes for the file size, 4 bytes for the 'WAVE')
        memcmp(sound.file_data.data(), "RIFF", 4) != 0 ||
        memcmp(sound.file_data.data() + 8, "WAVE", 4) != 0) {

	    unsigned char* buffer = sound.file_data.data();
	    REF(buffer);

        return false;
    }

    // Step 5: Scan the buffer to find "fmt" and "data" chunks
    uint32_t offset = 12; // Start after the RIFF header
    bool found_format = false;
    bool found_data = false;

	while (offset + 8 <= file_size_in_bytes) { // Ensure space for chunk ID (4) + size (4)
        // Read chunk ID and size
        char chunk_id[4];
        memcpy(chunk_id, sound.file_data.data() + offset, 4);
        uint32_t chunk_size;
        memcpy(&chunk_size, sound.file_data.data() + offset + 4, 4);

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            // Handle format chunk
            if (found_format) {
                log("Error: Multiple fmt chunks found");
                return false; // Only one "fmt " chunk allowed
            }
            found_format = true;
            if (chunk_size != 16) { // PCM format chunk size is 16 bytes
                log("Error: Invalid fmt chunk size");
                return false;
            }

            // Parse and verify format (little-endian)
            uint16_t format_tag;
            memcpy(&format_tag, sound.file_data.data() + offset + 8, 2); // Offset 8: wFormatTag
            uint16_t channels;
            memcpy(&channels, sound.file_data.data() + offset + 10, 2);  // Offset 10: nChannels
            uint32_t samples_per_sec;
            memcpy(&samples_per_sec, sound.file_data.data() + offset + 12, 4); // Offset 12: nSamplesPerSec
            uint32_t avg_bytes_per_sec;
            memcpy(&avg_bytes_per_sec, sound.file_data.data() + offset + 16, 4); // Offset 16: nAvgBytesPerSec
            uint16_t block_align;
            memcpy(&block_align, sound.file_data.data() + offset + 20, 2); // Offset 20: nBlockAlign
            uint16_t bits_per_sample;
            memcpy(&bits_per_sample, sound.file_data.data() + offset + 22, 2); // Offset 22: wBitsPerSample

            // Verify format matches init_xAudio2 expectations
            if (format_tag != WAVE_FORMAT_PCM || // PCM = 1
                channels != 1 ||                 // Mono
                samples_per_sec != 44100 ||      // 44.1kHz
                bits_per_sample != 16 ||         // 16-bit
                block_align != 2 ||              // 1 channel * 16 bits / 8 = 2 bytes
                avg_bytes_per_sec != 88200) {    // 44100 * 2 = 88200
                log("Error: WAV format does not match expected PCM, mono, 16-bit, 44.1kHz");
                return false;
            }
        }
        else if (memcmp(chunk_id, "data", 4) == 0) {
            // Handle data chunk
            if (found_data) {
                log("Error: Multiple data chunks found");
                return false; // Only one "data" chunk allowed
            }
            found_data = true;
            sound.data_offset = offset + 8; // Audio data starts after ID (4) + size (4)
            sound.data_size = chunk_size;   // Size of audio data
        }

        // Move to the next chunk
        offset += 8 + chunk_size;
    }

    // Step 6: Ensure both chunks were found
    if (!found_format || !found_data) {
        log("Error: Missing fmt or data chunk");
        return false;
    }

    return true;
}
// play_sound
// pause_sound

