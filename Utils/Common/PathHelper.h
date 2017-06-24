#ifndef PATHHELPER_H
#define PATHHELPER_H

#include <string>
#include "CommonHdr.h"

class UTILS_EXPORTS_API PathHelper
{
public:
	static const std::string& AppDeployPath();

	static const std::string& AppExecutablePath();

	//传入的可变参数后必须有一个空指针（0）表示结束
	static void Combine(std::string *result,...);
};

#endif /* PATHHELPER_H */