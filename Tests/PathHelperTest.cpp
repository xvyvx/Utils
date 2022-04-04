#include <boost/test/unit_test.hpp>
#include <string.h>
#include "Common/PathHelper.h"

BOOST_AUTO_TEST_SUITE(PathHelperTest)

BOOST_AUTO_TEST_CASE(DeployPathTest)
{
    std::string deployPath = PathHelper::AppDeployPath();
    BOOST_TEST(!deployPath.empty());
    BOOST_TEST(PathHelper::IsAbsolutePath(deployPath.c_str(), deployPath.size()));
}

BOOST_AUTO_TEST_CASE(ExecutablePathTest)
{
    std::string appPath = PathHelper::AppExecutablePath();
    std::string deployPath = PathHelper::AppDeployPath();
    BOOST_TEST(!deployPath.empty());
    BOOST_TEST(!appPath.empty());
    BOOST_TEST(strncmp(deployPath.c_str(), appPath.c_str(), deployPath.size()) == 0);
    deployPath.pop_back();
#ifdef IS_WINDOWS
    PathHelper::Combine(&deployPath, "UtilsTest.exe", nullptr);
#else
    PathHelper::Combine(&deployPath, "UtilsTest", nullptr);
#endif
    BOOST_TEST(deployPath == appPath);
}

BOOST_AUTO_TEST_SUITE_END()