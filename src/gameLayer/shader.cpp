#include "shader.h"
#include <gameLayer.h>


void Shader::load(const char *path)
{
	*this = {};
	shader = gl2d::createShaderFromFile(path);
	loadUniforms();

	loadType = LOAD;
	fragmentChanged.setFile(path);
}


void Shader::loadUniforms()
{
	iResolution = glGetUniformLocation(shader.id, "iResolution");
	iTime = glGetUniformLocation(shader.id, "iTime");
	u_viewProjection = glGetUniformLocation(shader.id, "u_viewProjection");
	u_model = glGetUniformLocation(shader.id, "u_model");
	
}

void Shader::tryReload()
{
	if (loadType != 0 && loadType != LOADDEFAULT3DSHADER)
	{

		if (fragmentChanged.changed())
		{
			switch (loadType)
			{
				case LOAD:
				{
					load(fragmentChanged.path.string().c_str());
					break;
				}

				case LOAD3DSHADER:
				{
					load3DShader(fragmentChanged.path.string().c_str());
					break;
				}

			}


		}

	}


}


static const char *default3DVertexShader =
GL2D_OPNEGL_SHADER_VERSION "\n"
GL2D_OPNEGL_SHADER_PRECISION "\n"
"in vec3 quad_positions;\n"
"in vec4 quad_colors;\n"
"in vec2 texturePositions;\n"
"out vec4 v_color;\n"
"out vec2 v_texture;\n"
"out vec2 v_positions;\n"
"out vec3 v_normal;"
"uniform mat4 u_viewProjection = mat4(1.0);\n"
"uniform mat4 u_model = mat4(1.0);\n"
"void main()\n"
"{\n"
"   mat3 rotationMatrix = mat3(u_model);\n"
"   vec3 transformedNormal = normalize(rotationMatrix * vec3(0,0,1));\n"
"   v_normal = transformedNormal;\n"
"	gl_Position = u_viewProjection * vec4(quad_positions, 1);\n"
"	//gl_Position.z = 0.5;\n"
"	v_color = quad_colors;\n"
"	v_texture = texturePositions;\n"
"	v_positions = gl_Position.xy;\n"
"}\n";

static const char *defaultFragmentShader =
GL2D_OPNEGL_SHADER_VERSION "\n"
GL2D_OPNEGL_SHADER_PRECISION "\n"
"out vec4 color;\n"
"in vec4 v_color;\n"
"in vec2 v_texture;\n"
"uniform sampler2D u_sampler;\n"
"void main()\n"
"{\n"
"    color = v_color * texture(u_sampler, v_texture);\n"
"}\n";


void Shader::load3DShader(const char *path)
{
	*this = {};
	std::string rez = platform::readEntireFile(path);

	if (rez.size())
	{
		shader = gl2d::createShaderProgram(default3DVertexShader, rez.c_str());
		loadUniforms();

		loadType = LOAD3DSHADER;
		fragmentChanged.setFile(path);
	}
}

void Shader::loadDefault3DShader()
{
	shader = gl2d::createShaderProgram(default3DVertexShader, defaultFragmentShader);
	loadUniforms();

	loadType = LOADDEFAULT3DSHADER;
	fragmentChanged = {};
}


void AssetManager::loadAll()
{

	backgroundShader.load(RESOURCES_PATH "space.frag");
	holographicShader.load3DShader(RESOURCES_PATH "balatro.frag");
	default3DShader.loadDefault3DShader();

	auto load = [&](const char *p)
	{
		gl2d::Texture t;
		t.loadFromFile(p, false, true);
		return t;
	};

	emptyCard = load(RESOURCES_PATH "cards/empty.png");
	earthCard = load(RESOURCES_PATH "cards/earth.png");
	cardPacket = load(RESOURCES_PATH "cards/cardpack.png");

}

void AssetManager::tryReload()
{
	for (auto &s : getAllShaders())
	{
		s.get().tryReload();

	}


}
