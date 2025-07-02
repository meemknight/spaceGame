#include <gl2d/gl2d.h>
#include "fileChanged.h"


struct Shader
{

	gl2d::ShaderProgram shader;
	GLint iResolution = -1;
	GLint iTime = -1;
	GLint u_viewProjection = -1;
	GLint u_model = -1;

	void load(const char *path);

	void load3DShader(const char *path);

	void loadDefault3DShader();

	void loadUniforms();

	void tryReload();

	FileChanged fragmentChanged;
	int loadType = 0;

	enum
	{
		none = 0,
		LOAD = 1,
		LOAD3DSHADER,
		LOADDEFAULT3DSHADER,
	};

};


struct AssetManager
{

	gl2d::Texture emptyCard;
	gl2d::Texture earthCard;
	gl2d::Texture cardPacket;

	Shader backgroundShader;
	Shader default3DShader;
	Shader holographicShader;

	std::initializer_list<std::reference_wrapper<Shader>> getAllShaders()
	{
		return  {
			std::ref(backgroundShader), 
			std::ref(default3DShader), 
			std::ref(holographicShader)
		
		};
	}


	void loadAll();

	void tryReload();

};