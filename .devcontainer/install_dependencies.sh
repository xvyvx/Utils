#! /bin/sh

ComponentRootPath=/root/component

InstallBoost()
{
    cd $ComponentRootPath
    if wget --no-check-certificate --quiet https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.bz2; then
        tar xjf ./boost_1_78_0.tar.bz2 && rm -f ./boost_1_78_0.tar.bz2 && cd ./boost_1_78_0/ && ./bootstrap.sh && \
        ./b2 stage --toolset=gcc cflags="-fPIC" cxxflags="-std=c++14 -fPIC" linkflags="-fPIC" --without-log --without-python \
            --stagedir="./stage" link=static runtime-link=shared threading=multi --layout=versioned debug release && \
        rm -rf ./bin.v2 && return 0
        return 1
    else
        return 1
    fi
}

InstallLog4cplus()
{
    cd $ComponentRootPath
    if wget --no-check-certificate --quiet https://github.com/log4cplus/log4cplus/archive/REL_1_2_0.zip; then
        unzip -q REL_1_2_0.zip && rm -f REL_1_2_0.zip && cd log4cplus-REL_1_2_0 && mkdir -p build/debug && cd build/debug && \
        cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-std=c++14" -DLOG4CPLUS_BUILD_TESTING=OFF -DLOG4CPLUS_BUILD_LOGGINGSERVER=OFF \
            -DCMAKE_INSTALL_PREFIX="../../install/debug" ../.. && \
        cmake --build . --target install && \
        mkdir ../release && cd ../release && \
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++14" -DLOG4CPLUS_BUILD_TESTING=OFF -DLOG4CPLUS_BUILD_LOGGINGSERVER=OFF \
            -DCMAKE_INSTALL_PREFIX="../../install/release" ../.. && \
        cmake --build . --target install && cd ../.. && rm -rf ./build && return 0
        return 1
    else
        return 1
    fi
}

InstallOTL()
{
    cd $ComponentRootPath && mkdir otl && cd otl && wget -q http://otl.sourceforge.net/otlv4_h2.zip && unzip -q otlv4_h2.zip && rm -f ./otlv4_h2.zip && return 0
    return 1
}

apt-get update -qq && apt-get -qq install -y libbsd-dev socat
mkdir -p $ComponentRootPath && InstallBoost && InstallLog4cplus && InstallOTL
apt-get clean