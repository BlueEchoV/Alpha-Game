#include "Audio_xAudio2.h"

#include <stdexcept>
#include <array>
#include <stdio.h>

#include <windows.h>    // Windows API header; provides core types and functions like CreateFile, ReadFile, and definitions for HRESULT, DWORD, etc.
#include <xaudio2.h>    // XAudio2 header; includes interfaces like IXAudio2 and structures like WAVEFORMATEX for audio processing.
#include <stdio.h>      // Standard C I/O header; used for fopen, fread, fclose to handle file operations in a C-style manner.
#include <stdlib.h>     // Standard C library header; provides malloc, free for memory allocation.

// Global variables for XAudio2 engine and mastering voice.
// These are shared to simplify access across functions, avoiding passing pointers repeatedly.
IXAudio2* g_pXAudio2 = nullptr;  // Pointer to the XAudio2 interface; nullptr is a C++ null pointer constant, used here for compatibility.
IXAudio2MasteringVoice* g_pMasteringVoice = nullptr;  // Pointer to the mastering voice interface.

// HRESULT is a 32-bit signed integer type defined in Windows API (winerror.h, included via windows.h).
// It represents return codes for functions: success (S_OK = 0), or error codes where negative values indicate failures.
// This type originates from COM (Component Object Model) in Windows, used for standardized error handling.

// DWORD is an unsigned 32-bit integer (typedef unsigned long DWORD in windef.h, via windows.h).
// Commonly used in Windows API for sizes, flags, and counts due to its fixed size across platforms.

// BYTE is an unsigned 8-bit integer (typedef unsigned char BYTE in windef.h).
// Used for raw data buffers like audio samples.

// WAVEFORMATEX is a structure defined in mmreg.h (included via xaudio2.h).
// It describes the format of waveform audio data, including sample rate, channels, bits per sample, etc.
// This is part of the Windows Multimedia API, essential for specifying audio formats to XAudio2.

// XAUDIO2_BUFFER is a structure defined in xaudio2.h.
// It holds audio data pointers, sizes, and flags for submission to source voices.

// 1: Initialize XAudio2 Engine
// This function sets up the XAudio2 engine and COM for multithreaded use.
void init_XAudio2() {
    HRESULT hr;  // Local variable to store function return codes; allows checking for success or failure.

    // Initialize COM with multithreaded model.
    // CoInitializeEx prepares the current thread for COM operations.
    // nullptr for reserved parameter (always null), COINIT_MULTITHREADED flag enables concurrent access from multiple threads.
    // Reason: XAudio2 requires COM initialization; multithreaded mode supports efficient audio processing without thread serialization.
    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Check if initialization failed (HRESULT < 0 indicates error).
    // If failed, print error and return early to avoid further issues.
    // Reason: Prevents proceeding with uninitialized COM, which would cause crashes or undefined behavior.
    if (FAILED(hr)) {
        printf("Failed to initialize COM: %ld\n", hr);  // printf is C-style output; %ld for long (HRESULT).
        return;
    }

    // Create the XAudio2 engine instance.
    // XAudio2Create takes pointer to interface, flags (0 for default), and processor (default).
    // Reason: This instantiates the core audio engine for creating voices and processing sound.
    hr = XAudio2Create(&g_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    // Check for failure and clean up if needed.
    // If failed, print error, uninitialize COM, and return.
    // Reason: Ensures resources are released on error to prevent leaks.
    if (FAILED(hr)) {
        printf("Failed to create XAudio2 engine: %ld\n", hr);
        CoUninitialize();  // Balances CoInitializeEx call.
        return;
    }
}

// 2: Create a Mastering Voice
// This sets up the final output voice for audio rendering.
void create_mastering_voice() {
    // Check if engine is initialized.
    // If not, print error and return.
    // Reason: Mastering voice requires a valid engine; avoids null pointer dereference.
    if (g_pXAudio2 == nullptr) {
        printf("XAudio2 not initialized!\n");
        return;
    }

    HRESULT hr;  // Local for return code.

    // Create mastering voice with default parameters.
    // g_pXAudio2->CreateMasteringVoice stores the interface in the pointer.
    // Reason: This voice mixes all source audio and sends it to hardware (e.g., speakers).
    hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);

    // Check for failure and print if so.
    // Reason: Informs user of setup issues without crashing.
    if (FAILED(hr)) {
        printf("Failed to create mastering voice: %ld\n", hr);
    }
}

// 3: Load a .wav file
// This reads the entire WAV file into memory using C file functions.
// Returns buffer and size; caller must free buffer with free().
// NOTE: The advantage of a pointer to a pointer in C or C++ is that it 
//       allows a function to modify the original pointer's target address 
//       indirectly, enabling dynamic memory allocation or reconfiguration 
//       without returning values, much like using a remote control to adjust 
//       a signpost's direction from afar.
// NOTE: Double pointers (e.g., BYTE**) are used here to allow the function 
//       to allocate memory and update the caller's original pointer with the new 
//       address, ensuring the allocated buffer is accessible outside the function 
//       while handling errors via HRESULT return values.

#if 0
// Example 1
void allocate_array(int* p, int size) {
    p = malloc(size * sizeof(int));  // p now points to 1000, but this is local.
}

int main() {
    int* arr = NULL;  // arr initially points to 0 (NULL), stored at, say, address 500.
    allocate_array(arr, 5);  // Passes copy of arr (0); function updates copy to 1000, but arr remains 0.
    // arr is still NULL (0), so no memory is allocated for use here.
    return 0;
}

// Example 2 
void allocate_array(int** pp, int size) {
    *pp = malloc(size * sizeof(int));  // Updates the pointer at the address pp references to 1000.
}

int main() {
    int* arr = NULL;  // arr initially points to 0, stored at address 500.
    allocate_array(&arr, 5);  // Passes address of arr (500); function sets value at 500 to 1000.
    // arr now points to 1000, allowing access to the allocated array.
    free(arr);  // Proper cleanup.
    return 0;
}
#endif

HRESULT load_wav_file(const char* filename, BYTE** ppWavBuffer, DWORD* pWavSize) {
    FILE* file = nullptr;  // FILE is a C standard type for file handles, defined in stdio.h.

    // Open file in binary read mode.
    // fopen returns FILE* or nullptr on failure.
    // "rb" mode ensures binary reading without text conversions.
    // Reason: WAV files are binary; text mode could corrupt data.
    file = fopen(filename, "rb");

    // Check if open failed.
    // If so, return E_FAIL (Windows API error code for general failure).
    // Reason: Prevents proceeding with invalid file.
    if (file == nullptr) {
        return E_FAIL;
    }

    // Seek to end to get size.
    // fseek moves file pointer; SEEK_END to end.
    // Reason: Determines total bytes in file.
    fseek(file, 0, SEEK_END);

    // Get current position (file size).
    // ftell returns long; cast to DWORD.
    // Reason: Stores size for allocation and reading.
    *pWavSize = (DWORD)ftell(file);

    // Seek back to start.
    // Reason: Prepares for reading from beginning.
    fseek(file, 0, SEEK_SET);

    // Allocate buffer.
    // malloc returns void*; cast to BYTE*.
    // Reason: Dynamically allocates memory for file contents.
    *ppWavBuffer = (BYTE*)malloc(*pWavSize);

    // Check allocation failure.
    // If failed, close file and return out of memory error.
    // Reason: Avoids reading into null pointer.
    if (*ppWavBuffer == nullptr) {
        fclose(file);
        return E_OUTOFMEMORY;  // Windows API code for memory allocation failure.
    }

    // Read entire file into buffer.
    // fread returns bytes read; check against size.
    // Reason: Loads raw file data for parsing.
    size_t bytesRead = fread(*ppWavBuffer, 1, *pWavSize, file);

    // Close file.
    // Reason: Releases file handle.
    fclose(file);

    // Verify full read.
    // If not, free buffer and return failure.
    // Reason: Ensures complete data; partial reads are errors.
    if (bytesRead != *pWavSize) {
        free(*ppWavBuffer);
        *ppWavBuffer = nullptr;
        return E_FAIL;
    }

    return S_OK;  // Success code.
}

// 4: Extract the audio data
// This parses the WAV buffer manually to find format and data chunks.
// Assumes standard WAV RIFF format; extracts WAVEFORMATEX and audio bytes.
HRESULT extract_audio_data(BYTE* pWavBuffer, DWORD wavSize, WAVEFORMATEX** ppWfx, BYTE** ppData, DWORD* pDataSize) {
    // Check input validity.
    // Reason: Prevents crashes from null or empty buffer.
    if (pWavBuffer == nullptr || wavSize < 44) {  // Minimum WAV header size.
        return E_INVALIDARG;  // Invalid argument error.
    }

    // Verify RIFF header.
    // First 4 bytes should be 'RIFF'.
    // Reason: Confirms it's a RIFF file (WAV is RIFF-based).
    if (*(DWORD*)pWavBuffer != *(DWORD*)"RIFF") {
        return E_FAIL;
    }

    // Skip RIFF size (bytes 4-7).
    // Then check 'WAVE' (bytes 8-11).
    // Reason: Verifies it's a WAVE RIFF.
    if (*(DWORD*)(pWavBuffer + 8) != *(DWORD*)"WAVE") {
        return E_FAIL;
    }

    // Start parsing chunks from offset 12.
    BYTE* pCurrent = pWavBuffer + 12;  // After RIFF/WAVE header.
    DWORD fmtSize = 0;  // To store 'fmt ' chunk size.

    // Loop to find 'fmt ' chunk.
    // Continue while within buffer.
    // Reason: WAV chunks vary; search for specific ones.
    while (pCurrent < pWavBuffer + wavSize) {
        DWORD chunkId = *(DWORD*)pCurrent;  // Chunk ID (4 bytes).
        DWORD chunkSize = *(DWORD*)(pCurrent + 4);  // Chunk size (4 bytes).

        // If 'fmt ', process it.
        if (chunkId == *(DWORD*)"fmt ") {
            // Allocate for WAVEFORMATEX.
            // Size is chunkSize, but at least sizeof(WAVEFORMATEX).
            // Reason: 'fmt ' contains format data; copy it.
            *ppWfx = (WAVEFORMATEX*)malloc(chunkSize);
            if (*ppWfx == nullptr) {
                return E_OUTOFMEMORY;
            }
            memcpy(*ppWfx, pCurrent + 8, chunkSize);  // Copy after header.
            fmtSize = chunkSize;  // Store for later checks.
            pCurrent += 8 + chunkSize;  // Advance.
            continue;  // Continue to find 'data'.
        }

        // If 'data', process it.
        if (chunkId == *(DWORD*)"data") {
            // Check if format was found first.
            // Reason: Format must precede data in standard WAV.
            if (fmtSize == 0) {
                return E_FAIL;  // Missing fmt.
            }
            // Allocate and copy data.
            *pDataSize = chunkSize;
            *ppData = (BYTE*)malloc(*pDataSize);
            if (*ppData == nullptr) {
                free(*ppWfx);  // Clean up previous alloc.
                *ppWfx = nullptr;
                return E_OUTOFMEMORY;
            }
            memcpy(*ppData, pCurrent + 8, *pDataSize);
            return S_OK;  // Success; data extracted.
        }

        // Advance to next chunk.
        // Add 8 (header) + size; pad to even if odd.
        // Reason: Chunks are padded to even bytes.
        pCurrent += 8 + chunkSize + (chunkSize % 2);
    }

    // If no 'data' found, clean up.
    if (fmtSize > 0) {
        free(*ppWfx);
        *ppWfx = nullptr;
    }
    return E_FAIL;  // No data chunk.
}

// 5: Create a source voice
// Creates a voice for playing a specific audio format.
// Multiple can be created for simultaneous playback.
IXAudio2SourceVoice* create_source_voice(WAVEFORMATEX* pWfx) {
    // Check engine.
    if (g_pXAudio2 == nullptr) {
        printf("XAudio2 not initialized!\n");
        return nullptr;
    }

    IXAudio2SourceVoice* pSourceVoice = nullptr;  // Local pointer.

    // Create source voice.
    // Takes format; defaults otherwise.
    // Reason: Source voice decodes and plays audio in given format.
    HRESULT hr = g_pXAudio2->CreateSourceVoice(&pSourceVoice, pWfx);

    // On failure, print and return null.
    if (FAILED(hr)) {
        printf("Failed to create source voice: %ld\n", hr);
        return nullptr;
    }

    return pSourceVoice;
}

// 6: Prepare and Submit the Audio Buffer
// Submits data to voice for playback.
HRESULT submit_audio_buffer(IXAudio2SourceVoice* pSourceVoice, BYTE* pData, DWORD dataSize) {
    // Check input.
    if (pSourceVoice == nullptr) {
        return E_INVALIDARG;
    }

    XAUDIO2_BUFFER buffer = {0};  // Zero-initialize structure.

    // Set buffer fields.
    // pAudioData points to data, AudioBytes is size.
    // Flags: END_OF_STREAM to auto-stop when done.
    // Reason: Prepares buffer for submission; flag handles end automatically.
    buffer.pAudioData = pData;
    buffer.AudioBytes = dataSize;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    // Submit to voice.
    // Reason: Adds buffer to playback queue.
    HRESULT hr = pSourceVoice->SubmitSourceBuffer(&buffer);

    // On failure, print.
    if (FAILED(hr)) {
        printf("Failed to submit buffer: %ld\n", hr);
    }

    return hr;
}

// 7: Start a source voice
// Begins playback.
HRESULT start_source_voice(IXAudio2SourceVoice* pSourceVoice) {
    // Check input.
    if (pSourceVoice == nullptr) {
        return E_INVALIDARG;
    }

    // Start voice.
    // 0 for default, COMMIT_NOW to apply immediately.
    // Reason: Initiates audio rendering from submitted buffers.
    HRESULT hr = pSourceVoice->Start(0);

    // On failure, print.
    if (FAILED(hr)) {
        printf("Failed to start source voice: %ld\n", hr);
    }

    return hr;
}