#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#define REF(v) (void)v;

struct V2 {
	int x;
	int y;
};

void log(const char* format, ...);
