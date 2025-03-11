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

bool my_mem_compare(const void* src, const void* dst, size_t n) {
	const char* src_byte = (const char*)src;
	const char* dst_byte = (const char*)dst;

	for (int i = 0; i < n; i++) {
		// Copy byte to byte (char is 1 byte)
		if (dst_byte[i] != src_byte[i]) {
			return false;
		}
	}

	return true;
}

void my_mem_copy(const void* src, void* dst, size_t n) {
	const char* src_byte = (const char*)src;
	char* dst_byte = (char*)dst;

	for (int i = 0; i < n; i++) {
		// Copy byte to byte (char is 1 byte)
		dst_byte[i] = src_byte[i];
	}
}
