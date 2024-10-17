#ifndef TIMBER_H
#define TIMBER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// This macro should be called instead of calling _log_print() directly. This is to avoid verbosity in function calls
// Example call: LOG(log, DEBUG, "Finished Calculations")
#define LOG(log, level, msg) _log_print(__func__, __LINE__, log, level, msg)

typedef enum {
	COLOR_RESET,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_RED,
	ANSI_RESET,
} color_t;

enum Levels {
	DEBUG,
	INFO,
	WARN,
	ERROR,
	CRITICAL,	
};

typedef struct logger_t {
	int min_level; // Minimum level to be printed
	FILE* stream; 
	bool time_enabled; // Print time along with debug message. Enabled by default
	bool line_enabled; // Print line number that debug message occurs on. Enabled by default
	bool function_enabled; // Print the name of the current function. Enabled by default
	color_t output_colors[5]; // Default colors are: Green, Cyan, Yellow, Red, Red
} logger_t;

// NOTE: Function must be called before you log
// DESC: Sets up the logger, opens the file and validates the inputs are actually usable
void log_init(logger_t *log, const enum Levels min_level, const char *file_name);

// DESC: Kill requested logger
void log_kill(logger_t *log);

// NOTE: This function uses a macro to simplify calling. args "function" and "line" should not be input manually by the user
// DESC: Produces and prints a formatted output using the configuration from the logger struct.
void _log_print(const char *function, const int line, logger_t *log, const enum Levels level, const char *msg);

#endif
