#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <string>
#include "CommonHdr.h"

/**
 * File system path related utilities.
 */
class UTILS_EXPORTS_API PathHelper
{
public:

    /**
     * Get application executable path(not include executable file name).
     *
     * @return Application executable path.
     */
    static const std::string& AppDeployPath();

    /**
     * Get application executable path(include executable file name).
     *
     * @return Application executable path.
     */
    static const std::string& AppExecutablePath();

    /**
     * Query if 'path' is absolute path
     *
     * @param path The path.
     * @param size Size of path.
     *
     * @return True if absolute path, false if not.
     */
    static bool IsAbsolutePath(const char *path, size_t size);

    /**
     * Combine file/dir names.
     * 
     * Variable arguments must be end with a null pointer.
     *
     * @param [in,out] result Combined path result.
     * @param 		   ...    Variable arguments providing additional file or directory names.
     */
    static void Combine(std::string *result,...);
};

#endif /* PATHHELPER_H */