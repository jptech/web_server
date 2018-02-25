#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <cstring>
#include <dirent.h>

#include "path.hpp"
#include "exceptions.hpp"

namespace wwwserver
{
    Path::Path()
    {
        // Get current working directory
        std::string path = getcwd(nullptr, 0);
        updatePath(path);
    }

    Path::Path(const std::string &const_path)
    {
        std::string path = const_path;
        // Update the internal path to match
        updatePath(path);
    }

    Path::Path(std::vector<std::string> &path, bool is_relative)
    {
        // Update the internal path to match
        updatePath(path, is_relative);
    }

    // Copy Constructor
    Path::Path(const Path &path)
    {
        // get the path string and path vector
        m_path_str = path.str();
        m_path_components = path.vec();

        // don't try to copy the stat data
        updateStat();
    }

    void Path::setPath(const std::string &path)
    {
        updatePath(path);
    }

    bool Path::isDir() const
    {
        return m_path_type == PathType::DIRECTORY || m_path_type == PathType::ROOT;
    }

    bool Path::isFile() const
    {
        return m_path_type == PathType::FILE;
    }

    bool Path::exists() const
    {
        return m_path_type != PathType::NOT_FOUND;
    }

    std::string Path::filename() const
    {
        return m_path_components[m_path_components.size()-1];
    }

    std::string Path::extension() const
    {
        std::string ext;

        // The path must be a file for it to have a file extension
        if(!isFile()) return "";

        // get the filename
        std::string fname = filename();

        // locate the extension by doing a reverse find for '.'
        return fname.substr(fname.rfind('.'));
    }

    Path Path::getParent() const
    {
        // Get the current path as a vector while leaving off the last element
        std::vector<std::string> new_path(
                &this->m_path_components[0],
                &this->m_path_components[this->m_path_components.size()-2]
        );

        return Path(new_path, false);
    }

    std::vector<Path> Path::getChildren()
    {
        std::vector<PathType> types {PathType::DIRECTORY, PathType::FILE, PathType::SYMLINK};
        return getChildrenType(types);
    }

    std::vector<Path> Path::getChildrenDirs()
    {
        std::vector<PathType> types {PathType::DIRECTORY, PathType::FILE};
        return getChildrenType(types);
    }

    std::vector<Path> Path::getChildrenFiles()
    {
        std::vector<PathType> types {PathType::FILE};
        return getChildrenType(types);
    }

    std::vector<Path> Path::getChildrenSymlinks()
    {
        std::vector<PathType> types {PathType::SYMLINK};
        return getChildrenType(types);
    }

    std::vector<Path> Path::getChildrenType(const std::vector<PathType> &types)
    {
        std::vector<Path> children;

        // *** ENTER C LAND *** //
        DIR *directory;
        struct dirent *entry;

        directory = opendir(this->str().c_str());

        if(directory == NULL)
        {
            // handle error
        }

        while( (entry = readdir(directory)) )
        {
            int ssize = strlen(entry->d_name);

            if(strncmp(entry->d_name, ".",  ssize) != 0 &&
               strncmp(entry->d_name, "..", ssize) != 0   )
            {
                Path p(entry->d_name);

                if(std::find(types.begin(), types.end(), p.getType()) != types.end())
                {
                    children.push_back(p);
                }
            }
        }

        closedir(directory);
        // *** EXIT C LAND *** //

        return children;
    }

    std::unique_ptr<Path> Path::find(const std::string &filename, int max_recursion)
    {
        // base case
        if(max_recursion < 0) return nullptr;

        // get the children from the current directory
        std::vector<Path> paths = getChildren();

        // loop through each child from the current directory
        for(Path &path : paths)
        {
            if(path.isFile() && path.filename() == filename)
            {
                // if the path is a file that matches the desired name, return a pointer to it
                return std::unique_ptr<Path>(new Path(path));
            }
            else if(path.isDir())
            {
                // if the path is a directory, recursively search it
                std::unique_ptr<Path> p = path.find(filename, max_recursion-1);

                // return up the chain if we find what we are looking for
                if(p != nullptr) return p;
            }
        }

        // if we make it through the whole search and can't find it, return a nullptr
        return nullptr;
    }

    void Path::upDir()
    {
        // Can go up unless already at the root directory
        if(m_path_type != PathType::ROOT && m_path_type != PathType::UNKNOWN)
        {
            // Invalidate the cached data
            invalidateCache();

            // Remove last element
            m_path_components.pop_back();

            // Update the path
            updatePath(m_path_components);
        }
        else
        {
            // Throw exception?
            throw "Can't go up a directory";
        }
    }

    void Path::join(const std::string &path)
    {
        updatePath(this->str() + path);
    }

    Path& Path::operator+=(const std::string &rhs)
    {
        this->join(rhs);
        return *this;
    }

    Path Path::operator+(const std::string &rhs)
    {
        return Path(this->str() + rhs);
    }

    std::string Path::str(bool relative) const
    {
        std::stringstream ss;

        if(relative)
        {
            // TODO
        }
        else
        {
            for(auto &segment : m_path_components)
            {
                ss << "/" << segment;
            }
        }

        return ss.str();
    }

    std::vector<std::string> Path::vec() const
    {
        return m_path_components;
    }

    PathType Path::getType() const
    {
        return m_path_type;
    }

    void Path::updateStat()
    {
        int stat_rv;

        // check if a stat struct should be allocated
        if(m_stat == nullptr)
        {
            m_stat.reset(new struct stat);
        }

        // get the stat for the path
        stat_rv = stat(m_path_str.c_str(), m_stat.get());

        // if there was an error, determine why
        if( stat_rv != 0 )
        {
            switch(errno)
            {
                // Permissions issue
                case EACCES:
                {
                    // TODO: throw exception
                    throw "Permissions Error";
                    break;
                }

                // File does not exist
                case ENOTDIR:
                case ENOENT:
                {
                    m_path_type = PathType::NOT_FOUND;
                    break;
                }

                // Weird things happened
                case EBADF:
                case EFAULT:
                case ELOOP:
                case ENOMEM:
                case ENAMETOOLONG:
                case EOVERFLOW:
                default:
                {
                    // TODO: throw an exception
                    throw "Bad things happened";
                    break;
                }
            }
        }

        // Determine the type of the path (file, dir, symlink, other)
        if(m_path_type != PathType::NOT_FOUND)
        {
            switch (m_stat.get()->st_mode & S_IFMT) {
                case S_IFDIR:
                {
                    m_path_type = PathType::DIRECTORY;
                    break;
                }

                case S_IFREG:
                {
                    m_path_type = PathType::FILE;
                    break;
                }

                case S_IFLNK:
                {
                    m_path_type = PathType::SYMLINK;
                    break;
                }

                case S_IFBLK:  // block device
                case S_IFCHR:  // char device
                case S_IFIFO:  // fifo/pipe
                case S_IFSOCK: // socket
                default:
                {
                    m_path_type = PathType::UNKNOWN;
                    break;
                }
            }
        }
    }

    void Path::updatePath(std::string path)
    {
        std::vector<std::string> path_vec;
        std::istringstream iss;
        std::string token;
        char path_cstr[PATH_MAX];

        // Clean the path string
        std::replace(path.begin(), path.end(), '\\', '/');
        realpath(path.c_str(), path_cstr);

        if (path_cstr == nullptr)
        {
            // throw exception
            throw "realpath failed";
        }

        // save the path string
        m_path_str = path_cstr;

        // clear out the old path vector
        m_path_components.clear();

        // Convert the string into a path vector
        iss.str(path);
        while(std::getline(iss, token, '/'))
        {
            if(!token.empty())
            {
                m_path_components.push_back(std::move(token));
            }
        }

        updateStat();
    }

    void Path::updatePath(std::vector<std::string> &path, bool is_relative)
    {
        std::stringstream ss;
        std::string path_str;
        std::string token;

        // safety check
        if(path.empty())
        {
            // throw exception - because that is silly
            throw "Empty Path variable";
        }

        // make the relative path absolute
        std::copy(path.begin(), path.end(), std::ostream_iterator<std::string>(ss, "/"));
        ss.str(realpath(ss.str().c_str(), nullptr));

        // save the string
        m_path_str = ss.str();

        // Update the internal path vector
        while(std::getline(ss, token, '/'))
        {
            if(!token.empty())
            {
                m_path_components.push_back(std::move(token));
            }
        }

        updateStat();
    }

    void Path::invalidateCache()
    {
        m_stat.reset(nullptr);
        m_path_type = PathType::NOT_SET;
    }
}
