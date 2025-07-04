#include <gl2d/gl2d.h>
#include "fileChanged.h"


struct Shader
{

	gl2d::ShaderProgram shader;
	GLint iResolution = -1;
	GLint iTime = -1;
	GLint u_viewProjection = -1;
	GLint u_model = -1;
	GLint u_CursorPosition = -1;
	GLint u_background = -1;
	

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

	gl2d::FrameBuffer background;

	gl2d::Texture emptyCard;
	gl2d::Texture earthCard;
	gl2d::Texture cardPacket;
	gl2d::Texture star;
	gl2d::Texture cardGrid;
	gl2d::Texture white1pxSquareTexture;
	
	Shader backgroundShader;
	Shader default3DShader;
	Shader holographicShader;
	Shader dither;
	Shader glass;
	Shader paper;

	std::vector<Shader*> getAllShaders()
	{
		return  {
			(&backgroundShader), 
			(&default3DShader), 
			(&holographicShader),
			(&dither),
			(&glass),
			(&paper),
		
		};
	}


	void loadAll();

	void tryReload();

};