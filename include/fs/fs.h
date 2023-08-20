#ifndef __FILE_SYSTEM_H
#define __FILE_SYSTEM_H

#include "../Logger/Logger.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>

namespace siem {


    namespace fs {

        typedef enum{
            None = 0,
            Read = 4,
            Write = 2,
            Execute = 1
        }PermissionType;

        struct Permission{
            explicit Permission(int permission);
            PermissionType readPermission;
            PermissionType writePermission;
            PermissionType execPermission;
        };

        struct PermissionGroup{
            explicit PermissionGroup(int fd);
            explicit PermissionGroup(const std::string& path);
            Permission owner;
            Permission group;
            Permission other;
        };

        class Path{
            static std::string join(const std::string& path, const std::string& file);
        };

        class FileUtil {
        public:
            /**
             * @brief confirm a file or directory is exist
             * @param path
             * @return
             */
            static bool exist(const std::string &path);

            static bool isDir(const std::string &path);
            static bool isFile(const std::string &filename);
            static bool isLink(const std::string &filename);
            static bool isCharDevice(const std::string &filename);
            static bool isBlockDevice(const std::string &filename);

            static std::vector<std::string> listDir(const std::string& path);

            static PermissionGroup getPermissionGroup(const std::string& path);
        };

        class File {

        };

    }


}

#endif //__FILE_SYSTEM_H
