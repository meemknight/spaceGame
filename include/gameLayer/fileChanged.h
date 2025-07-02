#pragma once
#include <filesystem>


#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif


#ifdef WIN32

	struct FileChanged
	{

		std::filesystem::path path;

		void setFile(const char *path);
		bool changed();

	private:
		FILETIME time = {};

	};

#else


	struct FileChanged
	{


		std::filesystem::path path;

		void setFile(const char *path) {this->path = path;}
		bool changed()
		{
			return 0;
		}


	};

#endif

