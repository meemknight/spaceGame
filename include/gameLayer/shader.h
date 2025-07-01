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


struct AssetManager
{

	gl2d::Texture emptyCard;
	gl2d::Texture earthCard;
	gl2d::Texture cardPacket;

	Shader backgroundShader;
	Shader default3DShader;
	Shader holographicShader;

	void loadAll();

};