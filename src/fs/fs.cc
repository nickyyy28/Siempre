#include "fs/fs.h"
#include "Logger/Logger.h"
#include <cstddef>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>

namespace siem{


namespace fs{

    bool FileUtil::exist(const std::string &path) {
        if (access(path.c_str(), 0) == 0) {
            return true;
        }        
        return false;
    }

    bool FileUtil::isDir(const std::string &path) {
        if (!exist(path)) {
            return false;
        }
        struct stat s_buff;
        stat(path.c_str(), &s_buff);
        if (S_ISDIR(s_buff.st_mode)) {
            return true;
        }
        return false;
    }

    bool FileUtil::isFile(const std::string &file) {
        if (!exist(file)) {
            return false;
        }
        struct stat s_buff;
        stat(file.c_str(), &s_buff);
        if (S_ISREG(s_buff.st_mode)) {
            return true;
        }
        return false;
    }

    bool FileUtil::isLink(const std::string &file) {
        if (!exist(file)) {
            return false;
        }
        struct stat s_buff;
        stat(file.c_str(), &s_buff);
        if (S_ISLNK(s_buff.st_mode)) {
            return true;
        }
        return false;
    }

    bool FileUtil::isCharDevice(const std::string &file) {
        if (!exist(file)) {
            return false;
        }
        struct stat s_buff;
        stat(file.c_str(), &s_buff);
        if (S_ISCHR(s_buff.st_mode)) {
            return true;
        }
        return false;
    }

    bool FileUtil::isBlockDevice(const std::string &file) {
        if (!exist(file)) {
            return false;
        }
        struct stat s_buff;
        stat(file.c_str(), &s_buff);
        if (S_ISBLK(s_buff.st_mode)) {
            return true;
        }
        return false;
    }

    std::vector<std::string> FileUtil::listDir(const std::string& path)
    {
        std::vector<std::string> ret;
        if (!exist(path)) {
            WARN() << path << " DONT EXIST";
            return ret;
        }

        if (!isDir(path)) {
            WARN() << path << "IS NOT A DIRECTORY";
            return ret;
        }

        struct dirent *entry;
        DIR *dir = opendir(path.c_str());
        while ((entry = readdir(dir)) != nullptr) {
            if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            ret.push_back(std::string(path) + "/" + std::string(entry->d_name));
        }
        return ret;
    }

    PermissionGroup FileUtil::getPermissionGroup(const std::string &path) {
        return PermissionGroup(path);
    }

    Permission::Permission(int permission) {
        if (permission & (1 << 2) ) {
            this->readPermission = PermissionType::Read;
        }

        if (permission & (1 << 1) ) {
            this->readPermission = PermissionType::Write;
        }

        if (permission & (1 << 0) ) {
            this->readPermission = PermissionType::Execute;
        }
    }

    PermissionGroup::PermissionGroup(int fd)
        : owner(0),
        group(0),
        other(0) {
        
    }

    PermissionGroup::PermissionGroup(const std::string& path) 
        : owner(0), group(0), other(0) {

        struct stat fileStatus;
        stat(path.c_str(), &fileStatus);
        //fileStatus.st_mode & 0x0700 = 
    }

    std::string Path::join(const std::string& path, const std::string& file)
    {
        std::stringstream ss1;
        std::stringstream ss2;

        ss1 << path;
        ss2 << file;

        

        return ss1.str() + ss2.str();
    }

}


}
