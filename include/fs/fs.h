#ifndef __FILE_SYSTEM_H
#define __FILE_SYSTEM_H

#include "../Logger/Logger.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

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

        class FileUtil {
        public:
            /**
             * @brief confirm a file or directory is exist
             * @param path
             * @return
             */
            static bool exist(const std::string &path);

            static bool isDir(const std::string &path);

            __attribute__((unused)) static bool isFile(const std::string &filename);

            static PermissionGroup getPermissionGroup(const std::string& path);
        };

        class File {

        };

    }


}

#endif //__FILE_SYSTEM_H
