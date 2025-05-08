#include "Utility.h" 
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

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

bool open_csv_data_file(CSV_Data* data) {
	data->file = fopen(data->file_name.c_str(), "r");
	if (data->file == NULL) {
		log("Error: CSV File did not open correctly.");
		assert(false);
		return false;
	}
	return true;
}

bool close_csv_data_file(CSV_Data* data) {
	if (fclose(data->file) != 0) {
		log("Error: csv file did not close properly.");
		assert(false);
		return false;
	}
	// Set it to NULL to avoid issues
	data->file = NULL;
	return true;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream my_string(str);

	for (std::string token; std::getline(my_string, token, delimiter);) {
		tokens.push_back(token);
	}

	return tokens;
}

int count_csv_data_columns(CSV_Data* data) {
	bool file_was_not_open = false;
	if (data->file == NULL) {
		if (open_csv_data_file(data) == false) {
			return -1;
		}
		file_was_not_open = true;
	}

	char buffer[50];
	if (fgets(buffer, sizeof(buffer), data->file) != 0) {
		log("Error: fgets failed.");
		return -1;
	}
	std::string line = buffer;
	std::vector<std::string> tokens = split(line, ',');

	rewind(data->file);

	if (file_was_not_open) {
		close_csv_data_file(data);
	}

	return (int)tokens.size();
}

int count_csv_data_rows(CSV_Data* data) {
	bool file_was_not_open = false;
	if (data->file == NULL) {
		if (open_csv_data_file(data) == false) {
			return 0;
		}
		file_was_not_open = true;
	}
	int result = 0;

	char buffer[50];

	// Ignore the first row
	if (fgets(buffer, sizeof(buffer), data->file) == NULL) {
		return 0;
	}

	while (fgets(buffer, sizeof(buffer), data->file) != NULL) {
		result++;
	}
	rewind(data->file);

	if (file_was_not_open) {
		close_csv_data_file(data);
	}
	return result;
}

CSV_Data create_open_csv_data(std::string file_name) {
	CSV_Data result = {};

	result.file_name = file_name;

	open_csv_data_file(&result);
	result.total_rows = count_csv_data_rows(&result);

	return result;
}

int get_column_index(std::vector<std::string>& column_names, std::string name) {
	int index = -1;
	for (int i = 0; i < column_names.size(); i++) {
		if (column_names[i] == name) {
			index = i;
		}
	}
	return index;
}

void load_csv_data_file(CSV_Data* data, char* destination, std::span<Type_Descriptor> type_descriptors, size_t stride) {
	if (data->file == NULL) {
		open_csv_data_file(data);
	}

	// TODO: INCREASE BUFFER SIZE
	char buffer[50];
	// Get the first line
	if (fgets(buffer, sizeof(buffer), data->file) == NULL) {
		return;
	}

	std::string line = buffer;
	std::vector<std::string> columns_names = split(line, ',');

	int current_row = 0;
	while (fgets(buffer, sizeof(buffer), data->file) != NULL) {
		std::vector<std::string> column_values = split(line, ',');

		//			      std::vector	 bytes of struct
		void* write_ptr = destination + (stride * current_row);

		for (int i = 0; i < type_descriptors.size(); i++) {
			Type_Descriptor current = type_descriptors[i];
			int column_index = get_column_index(columns_names, current.variable_name);
			current_row++;

			if (current.variable_type == VT_Int) {
				int* destination_ptr = (int*)((char*)write_ptr + current.offset);
				*destination_ptr = std::stoi(column_values[column_index]);
			}
			else if (current.variable_type == VT_Float) {
				float* destination_ptr = (float*)((char*)write_ptr + current.offset);
				*destination_ptr = std::stof(column_values[column_index]);
			}
			else if (current.variable_type == VT_String) {
				std::string* destination_ptr = (std::string*)((char*)write_ptr + current.offset);
				*destination_ptr = column_values[column_index];
			} 
			else {
				log("ERROR: Type Descriptor variable type not found");
			}
		}
	}
}

// 1) Open the CSV file
// 2) Load the data off the CSV file
// 2.b) Hot loading
// 3) Close the CSV file
