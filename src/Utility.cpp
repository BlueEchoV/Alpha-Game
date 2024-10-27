#include "Utility.h" 

void log(const char* format, ...) {
	char buffer[256];
	va_list arguments;

	va_start(arguments, format);
	vsnprintf(buffer, 256, format, arguments);
	va_end(arguments);

	OutputDebugString(buffer);
	OutputDebugString("\n");
}

void my_mem_copy(const void* src, void* dst, size_t n) {
	const char* src_byte = (const char*)src;
	char* dst_byte = (char*)dst;

	for (int i = 0; i < n; i++) {
		// Copy byte to byte (char is 1 byte)
		dst_byte[i] = src_byte[i];
	}
}
