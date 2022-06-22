#include "fs/fs.h"

namespace siem{


namespace fs{

    bool FileUtil::exist(const std::string &path) {
        if (access(path.c_str(), 0) == 0) {
            return true;
        }        
        return false;
    }

    bool FileUtil::isDir(const std::string &path) {
        /*if (path && opendir(path.c_str()) != nullptr) {
            return true;
        }
        return false;*/

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

    __attribute__((unused)) bool FileUtil::isFile(const std::string &file) {
        return false;
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

}


}
