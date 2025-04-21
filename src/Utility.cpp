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

CSV_Data create_csv_data(std::string file_name) {
	CSV_Data result = {};

	result.file_name = file_name;

	return result;
}

// Return 0 on success
void open_csv_data_file(CSV_Data* data) {
	data->file = fopen(data->file_name.c_str(), "r");
	if (data->file == NULL) {
		log("Error: CSV File did not open correctly.");
		assert(false);
	}
}

// Return 0 on success
void close_csv_data_file(CSV_Data* data) {
	if (fclose(data->file) != 0) {
		log("Error: csv file did not close properly.");
		assert(false);
	}
}

void load_csv_data() {

}

// 1) Open the CSV file
// 2) Load the data off the CSV file
// 2.b) Hot loading
// 3) Close the CSV file
