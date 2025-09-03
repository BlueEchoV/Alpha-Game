#include "GL_Functions.h"

glCreateShaderFunc glCreateShader = nullptr;
glShaderSourceFunc glShaderSource = nullptr;
glCompileShaderFunc glCompileShader = nullptr;
glGetShaderivFunc glGetShaderiv = nullptr;
glGetShaderInfoLogFunc glGetShaderInfoLog = nullptr;
glCreateProgramFunc glCreateProgram = nullptr;
glDeleteProgramFunc glDeleteProgram = nullptr;
glAttachShaderFunc glAttachShader = nullptr;
glLinkProgramFunc glLinkProgram = nullptr;
glGetProgramivFunc glGetProgramiv = nullptr;
glGetProgramInfoLogFunc glGetProgramInfoLog = nullptr;
glDetachShaderFunc glDetachShader = nullptr;
glDeleteShaderFunc glDeleteShader = nullptr;
glUseProgramFunc glUseProgram = nullptr;
glGenVertexArraysFunc glGenVertexArrays = nullptr;
glGenBuffersFunc glGenBuffers = nullptr;
glVertexAttribPointerFunc glVertexAttribPointer = nullptr;
glEnableVertexAttribArrayFunc glEnableVertexAttribArray = nullptr;
glDisableVertexAttribArrayFunc glDisableVertexAttribArray = nullptr;
glBindVertexArrayFunc glBindVertexArray = nullptr;
glBindBufferFunc glBindBuffer = nullptr;
glBufferDataFunc glBufferData = nullptr;
glDeleteBuffersFunc glDeleteBuffers = nullptr;
glGetUniformLocationFunc glGetUniformLocation = nullptr;
glUniformMatrix4fvFunc glUniformMatrix4fv = nullptr;
wglCreateContextAttribsARBFunc wglCreateContextAttribsARB = nullptr;
glActiveTextureFunc glActiveTexture = nullptr;
glUniform1iFunc glUniform1i = nullptr;
glUniform1fFunc glUniform1f = nullptr;
glUniform2fFunc glUniform2f = nullptr;
glUniform3fFunc glUniform3f = nullptr;
glBufferSubDataFunc glBufferSubData = nullptr;
glGenerateMipmapFunc glGenerateMipmap = nullptr;

void load_open_gl_functions() {
	glCreateShader = (glCreateShaderFunc)wglGetProcAddress("glCreateShader");
	glShaderSource = (glShaderSourceFunc)wglGetProcAddress("glShaderSource");
	glCompileShader = (glCompileShaderFunc)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (glGetShaderivFunc)wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (glGetShaderInfoLogFunc)wglGetProcAddress("glGetShaderInfoLog");

	glCreateProgram = (glCreateProgramFunc)wglGetProcAddress("glCreateProgram");

	glDeleteProgram = (glDeleteProgramFunc)wglGetProcAddress("glDeleteProgram");
	glAttachShader = (glAttachShaderFunc)wglGetProcAddress("glAttachShader");
	glLinkProgram = (glLinkProgramFunc)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (glGetProgramivFunc)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (glGetProgramInfoLogFunc)wglGetProcAddress("glGetProgramInfoLog");
	glDetachShader = (glDetachShaderFunc)wglGetProcAddress("glDetachShader");
	glDeleteShader = (glDeleteShaderFunc)wglGetProcAddress("glDeleteShader");
	glUseProgram = (glUseProgramFunc)wglGetProcAddress("glUseProgram");

	glGenVertexArrays = (glGenVertexArraysFunc)wglGetProcAddress("glGenVertexArrays");
	glGenBuffers = (glGenBuffersFunc)wglGetProcAddress("glGenBuffers");
	glVertexAttribPointer = (glVertexAttribPointerFunc)wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (glEnableVertexAttribArrayFunc)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (glDisableVertexAttribArrayFunc)wglGetProcAddress("glDisableVertexAttribArray");
	glBindVertexArray = (glBindVertexArrayFunc)wglGetProcAddress("glBindVertexArray");
	glBindBuffer = (glBindBufferFunc)wglGetProcAddress("glBindBuffer");
	glBufferData = (glBufferDataFunc)wglGetProcAddress("glBufferData");
	glBufferSubData = (glBufferSubDataFunc)wglGetProcAddress("glBufferSubData");
	glDeleteBuffers = (glDeleteBuffersFunc)wglGetProcAddress("glDeleteBuffers");

	glGetUniformLocation = (glGetUniformLocationFunc)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (glUniform1iFunc)wglGetProcAddress("glUniform1i");
	glUniform1f = (glUniform1fFunc)wglGetProcAddress("glUniform1f");
	glUniform2f = (glUniform2fFunc)wglGetProcAddress("glUniform2f");
	glUniform3f = (glUniform3fFunc)wglGetProcAddress("glUniform3f");
	glUniformMatrix4fv = (glUniformMatrix4fvFunc)wglGetProcAddress("glUniformMatrix4fv");
	glActiveTexture = (glActiveTextureFunc)wglGetProcAddress("glActiveTexture");
	
	wglCreateContextAttribsARB = (wglCreateContextAttribsARBFunc)wglGetProcAddress("wglCreateContextAttribsARB");

	glGenerateMipmap = (glGenerateMipmapFunc)wglGetProcAddress("glGenerateMipmap");
}

