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
