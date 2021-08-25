#include "..\include\Spectre2D\FileSystem.h"
#include "..\include\Spectre2D\core.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__unix__)
#include <unistd.h>
#include <errno.h>
#endif

namespace sp
{
	FileSystem::FileSystem(bool appdata)
	{
		if (appdata)
		{

#if defined(_WIN32)
			char* appbuff;
			size_t len;
			_dupenv_s(&appbuff, &len, "APPDATA");
			current_path = appbuff;
#elif defined(__unix__)
			current_path = "~/.local/";
#endif

		}
		else
		{
			current_path = fs::current_path();
		}
	}

	bool FileSystem::createDir(const fs::path& dirname)
	{
		return fs::create_directory(current_path / dirname);
	}

	bool FileSystem::deleteDirRecursively(const fs::path& dirname)
	{
		return fs::remove_all(current_path / dirname) > 0;
	}

	bool FileSystem::enterDir(const fs::path& path)
	{
		if (exists(path))
		{
			current_path = getCorrectPath(path);
			return true;
		}
		return false;
	}

	std::string FileSystem::getDirectory() const
	{
		return current_path.filename().string();
	}

	fs::path FileSystem::getCurrentPath() const
	{
		return current_path;
	}

	void FileSystem::exit()
	{
		current_path = current_path.parent_path();
	}

	void FileSystem::exitTo(const fs::path& dirname)
	{
		while (
			current_path.filename() != dirname && fs::absolute(current_path) != fs::absolute(dirname)
			&& !current_path.empty()
			)
		{
			this->exit();
		}
	}

	bool FileSystem::createDirIfNecessary(const fs::path& dirname)
	{
		if (!exists(dirname))
			return createDir(dirname);

		return true;
	}

	std::ifstream FileSystem::openFile(const fs::path& path) const
	{
		return std::ifstream(getCorrectPath(path));
	}

	std::ofstream FileSystem::openOfile(const fs::path& path) const
	{
		return std::ofstream(getCorrectPath(path));
	}

	bool FileSystem::createFile(const fs::path& filename)
	{
		std::ofstream f(getCorrectPath(filename));

		bool worked = f.is_open();

		f.close();

		return worked;
	}

	bool FileSystem::createFileIfNecessary(const fs::path& filename)
	{
		if (!exists(filename))
			return createFile(filename);

		return true;
	}

	bool FileSystem::deleteFile(const fs::path& filename)
	{
		return fs::remove(filename);
	}

	bool FileSystem::deleteFileIfExists(const fs::path& filename)
	{
		if (exists(filename))
			return fs::remove(filename);

		return true;
	}

	bool FileSystem::exists(const fs::path& path) const
	{
		return fs::exists(getCorrectPath(path));
	}

	bool FileSystem::addPathTemplate(const std::string& temp, const fs::path& target)
	{
		fs::path correct = target;
		if (!correct.is_absolute()) correct = current_path / correct;

		size_t s = temp.size();
		if (s > 4 && temp[0] == temp[1] && temp[1] == temp[s - 1] && temp[s - 1] == temp[s - 2] && temp[s - 2] == '_')
		{
			path_templates.insert(std::make_pair(temp, correct));
			return true;
		}

		return false;
	}

	bool FileSystem::isTemplate(const std::string& name)
	{
		size_t s = name.size();
		return (s > 4 && name[0] == name[1] && name[1] == name[s - 1] && name[s - 1] == name[s - 2] && name[s - 2] == '_');
	}

	fs::path FileSystem::getCorrectPath(const fs::path& path) const
	{
		if (path.empty())
			return current_path;

		if (path.has_root_path())
			return path;

		std::string root = (*path.begin()).string();

		fs::path rest;
		auto it = path.begin();
		it++;
		for (; it != path.end(); it++)
			rest /= *it;

		if (isTemplate(root) && path_templates.find(root) != path_templates.end())
			return path_templates.at(root) / rest;

		if (!rest.empty())
			return current_path / root / rest;
		else
			return current_path / root;
	}

	fs::directory_iterator FileSystem::getFilesInDirectory(const fs::path& dirname)
	{
		return fs::directory_iterator(getCorrectPath(dirname));
	}

	bool FileSystem::isRegularFile(const fs::path& path) const
	{
		return fs::is_regular_file(getCorrectPath(path));
	}

	bool FileSystem::isDirectory(const fs::path& path) const
	{
		return fs::is_directory(getCorrectPath(path));
	}

	fs::path FileSystem::getExecutable() const
	{
		fs::path path;

#if defined(_WIN32)
		DWORD size = 1;
		LPWSTR buff = 0;

		do
		{
			size <<= 1;
			buff = new WCHAR[size];
		} while (GetModuleFileNameW(NULL, buff, size) == size);

		path = buff;
		delete[] buff;

#elif defined(__unix__)
		char* buff = 0;
		size_t size = 1;
		ssize_t result = 0;

		do
		{
			buff = new char[size]{ 0 };
			size << 1;
			result = readlink("/proc/self/exe", buff, size);
		} while (result == size || (result == -1 && errno == ENAMETOOLONG));

		path = buff;

		delete[] buff;
#endif
		return path;
	}
}