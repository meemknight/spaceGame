#include <gl2d/gl2d.h>



struct Shader
{

	gl2d::ShaderProgram shader;
	GLuint iResolution = 0;
	GLuint iTime = 0;
	GLuint u_viewProjection = 0;

	void load(const char *path);

	void load3DShader(const char *path);

	void loadDefault3DShader();

	void loadUniforms();

};