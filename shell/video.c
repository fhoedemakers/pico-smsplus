#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <epoxy/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "shared.h"

#include "smsplus.h"
#include "video.h"

static uint8_t *pixels = NULL;

static int renderwidth, renderheight;

// Shader sources
static const GLchar* vshader_src =
	"#version 150 core\n"
	"in vec2 position;"
	"in vec2 texcoord;"
	"out vec2 outcoord;"
	"void main() {"
	"	outcoord = texcoord;"
	"	gl_Position = vec4(position, 0.0, 1.0);"
	"}";

static const GLchar* fshader_src =
	"#version 150 core\n"
	"in vec2 outcoord;"
	"out vec4 fragcolor;"
	"uniform sampler2D gametex;"
	"void main() {"
	"	fragcolor = texture(gametex, outcoord);"
	"}";

static GLuint vao;
static GLuint vbo;
static GLuint vshader;
static GLuint fshader;
static GLuint gl_shader_prog = 0;
static GLuint gl_texture_id = 0;

static int ggoffset[2] = {0,0};

void smsp_video_create_buffer() {
	// Create video buffer
	pixels = calloc(VIDEO_WIDTH_SMS * VIDEO_HEIGHT_SMS * 4, 1);
}

uint8_t *smsp_video_pixels_ptr() { return pixels; }

void ogl_render() {
	// Render the scene
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB,
				VIDEO_WIDTH_SMS,
				VIDEO_HEIGHT_SMS,
				0,
				GL_BGRA,
				GL_UNSIGNED_BYTE,
		pixels + ggoffset[0]);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void ogl_init() {
	// Initialize OpenGL
	
	// Grab settings pointer
	settings_t *settings = smsp_settings_ptr();
	
	float vertices[] = {
		-1.0f, -1.0f,	// Vertex 1 (X, Y)
		-1.0f, 1.0f,	// Vertex 2 (X, Y)
		1.0f, -1.0f,	// Vertex 3 (X, Y)
		1.0f, 1.0f,		// Vertex 4 (X, Y)
		0.0, 1.0,		// Texture 1 (X, Y)
		0.0, 0.0,		// Texture 2 (X, Y)
		1.0, 1.0,		// Texture 3 (X, Y)
		1.0, 0.0		// Texture 4 (X, Y)
	};
	
	GLint status;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vshader_src, NULL);
	glCompileShader(vshader);
	
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) { fprintf(stderr, "Failed to compile vertex shader\n"); }
	
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fshader_src, NULL);
	glCompileShader(fshader);
	
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) { fprintf(stderr, "Failed to compile fragment shader\n"); }
	
	GLuint gl_shader_prog = glCreateProgram();
	glAttachShader(gl_shader_prog, vshader);
	glAttachShader(gl_shader_prog, fshader);
	
	glLinkProgram(gl_shader_prog);
	
	glValidateProgram(gl_shader_prog);
	glGetProgramiv(gl_shader_prog, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) { fprintf(stderr, "Failed to link shader program\n"); }
	
	glUseProgram(gl_shader_prog);
	
	GLint posAttrib = glGetAttribLocation(gl_shader_prog, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	GLint texAttrib = glGetAttribLocation(gl_shader_prog, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)(8 * sizeof(GLfloat)));
	
	glGenTextures(1, &gl_texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_texture_id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings->video_filter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	renderwidth = VIDEO_WIDTH_SMS * settings->video_scale;
	renderheight = VIDEO_HEIGHT_SMS * settings->video_scale;
	
	if (sms.console == CONSOLE_GG) {
		ggoffset[0] = VIDEO_HEIGHT_SMS;
		ggoffset[1] = 24 * settings->video_scale;
	}
	
	glViewport(0, 0 - ggoffset[1], renderwidth, renderheight);
	
	glUniform1i(glGetUniformLocation(gl_shader_prog, "gametex"), 0);
}

void ogl_deinit() {
	// Deinitialize OpenGL
	if (gl_texture_id) { glDeleteTextures(1, &gl_texture_id); }
	if (gl_shader_prog) { glDeleteProgram(gl_shader_prog); }
	if (vshader) { glDeleteShader(vshader); }
	if (fshader) { glDeleteShader(fshader); }
	if (vao) { glDeleteVertexArrays(1, &vao); }
	if (vbo) { glDeleteBuffers(1, &vbo); }
	if (pixels) { free(pixels); }
}

static void smsp_video_screenshot_flip(unsigned char *pixbuf, int width, int height, int bytes) {
	// Flip the pixels
	int rowsize = width * bytes;
	unsigned char *row = (unsigned char*)malloc(rowsize);
	unsigned char *low = pixbuf;
	unsigned char *high = &pixbuf[(height - 1) * rowsize];
	
	for (; low < high; low += rowsize, high -= rowsize) {
		memcpy(row, low, rowsize);
		memcpy(low, high, rowsize);
		memcpy(high, row, rowsize);
	}
	free(row);
}

void smsp_video_screenshot(const char* filename) {
	// Take a screenshot in .png format
	
	settings_t *settings = smsp_settings_ptr();
	
	int sswidth, ssheight;
	
	if (sms.console == CONSOLE_GG) {
		sswidth = VIDEO_WIDTH_GG * settings->video_scale;
		ssheight = VIDEO_HEIGHT_GG * settings->video_scale;
	}
	else {
		sswidth = renderwidth;
		ssheight = renderheight;
	}
	
	unsigned char *pixbuf;
	pixbuf = malloc(sizeof(uint32_t) * sswidth * ssheight);
	
	// Read the pixels and flip them vertically
	glReadPixels(0, 0, sswidth, ssheight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixbuf);
	smsp_video_screenshot_flip(pixbuf, sswidth, ssheight, 4);
	
	static int sshotnum = 1;
	
	if (filename == NULL) {
		// Set the filename
		char sshotpath[512];
		snprintf(sshotpath, sizeof(sshotpath), "screenshots/%d.png", sshotnum);
		sshotnum++;
		
		// Save the file
		stbi_write_png(sshotpath, sswidth, ssheight, 4, (const void *)pixbuf, 4 * sswidth);
		fprintf(stderr, "Screenshot: %s\n", sshotpath);
	}
	else {
		stbi_write_png(filename, sswidth, ssheight, 4, (const void *)pixbuf, 4 * sswidth);
	}
	
	free(pixbuf);
}
