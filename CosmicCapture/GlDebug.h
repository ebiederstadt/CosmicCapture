#pragma once
#include "GL/glew.h"

//------------------------------------------------------------------------------
// OpenGL has a mechanism where you can turn on debug mode and it will tell you
// all sorts of fun stuff about what you're doing.
//
// We are going to use it (best we can) to give you advanced warning of when you
// are doing something incorrectly.
//
// Note: This class was taken directly from the CPSC 453 fall 2020 assignment boilerplate,
// which can be found here: https://github.com/Fall-2020-CPSC-453/453-skeleton
//------------------------------------------------------------------------------


namespace GLDebug {

	void debugOutputHandler(
		GLenum,
		GLenum,
		GLuint,
		GLenum,
		GLsizei,
		const GLchar*,
		const void*
	);

	void enable();
}