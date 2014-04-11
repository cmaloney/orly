#!/bin/bash

set -e

mkdir -p third_party

cd third_party
VERSION=3.4
SRC_ROOT="$PWD/llvm"
OBJ_ROOT="$PWD/llvm_build_$VERSION"
INSTALL_DIR="llvm-$VERSION"
ORLY_BIN="/opt/$INSTALL_DIR"

BINUTILS_VERSION=2.23.2
BINUTILS_HEADERS="/usr/include"

#TODO:{lld, lldb} but it's buggy.
#TODO: Polly but lots of deps I don't want to deal with
llvm_projects=("llvm" "clang" "compiler-rt" "clang-tools-extra" "test-suite")

#Force compilation of clang with gcc
export CC=gcc
export CXX=g++
export MAKEFLAGS='-j8'

#NOTE: Not used anywhere (yet). For ubuntu 13.10 it looks like Arch for the time being
is_ubuntu=false
if [[ -f /etc/lsb-release ]]; then
    if grep -q "ubuntu" /etc/lsb-release; then
      is_ubuntu=true

      #Make sure plugin-api.h for ld.gold is installed.
      sudo apt-get install binutils-dev
    fi
fi

is_redhat=false
if [[ -f /etc/redhat-release ]]; then
    is_redhat=true
fi

if $is_redhat; then
    BINUTILS_HEADERS="$PWD/binutils-$BINUTILS_VERSION/include"
    echo "Downloading binutils-$BINUTILS_VERSION"
    if [ ! -e "binutils-$BINUTILS_VERSION.tar.gz" ]
    then
      wget "http://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz"
    fi
    tar -xzf "binutils-$BINUTILS_VERSION.tar.gz"
fi


echo "Downloading sources"
for project in "${llvm_projects[@]}"
do
  if [ ! -e "$project-$VERSION.src.tar.gz" ]
  then
    wget "http://llvm.org/releases/$VERSION/$project-$VERSION.src.tar.gz"
  fi
done

echo "Unpacking sources"

for project in "${llvm_projects[@]}"
do
  tar -xf "$project-$VERSION.src.tar.gz"
done


echo "Arranging sources"
#Remove the old build if any
rm -rf llvm

#Rearrange as needed
mv llvm-3.4 llvm
mv clang-3.4 llvm/tools/clang
mv compiler-rt-3.4 llvm/projects/compiler-rt
mv clang-tools-extra-3.4 llvm/tools/clang/tools/extra
mv test-suite-3.4 llvm/projects/test-suite
#TODO (broken) mv lld-3.4 llvm/tools/lld
#TODO (broken) mv lldb-3.4 llvm/tools/lldb

echo "Applying patches"
cd llvm/tools/clang

##Apply some patches

##Use Gold linker
#NOTE: This causes 5 clang driver tests to fail because the path isn't what's expected.
wget https://projects.archlinux.org/svntogit/packages.git/plain/trunk/clang-3.3-use-gold-linker.patch?h=packages/llvm -O clang-3.3-use-gold-linker.patch
patch -Np1 -i clang-3.3-use-gold-linker.patch

patch -p0 -i ../../../redhat_new_triple.patch

#TODO: Replace with http://llvm.org/viewvc/llvm-project?view=revision&revision=200954
patch -p0 -i ../../../fix_clang_segfault.patch

cd ../../
##Fix rpath https://bugs.archlinux.org/task/14017
sed -i 's:$(RPATH) -Wl,$(\(ToolDir\|LibDir\|ExmplDir\))::g' Makefile.rules

#Build and install LLVM
echo "Building"
rm -rf $OBJ_ROOT
mkdir $OBJ_ROOT
cd $OBJ_ROOT

COMMON_OPTS=("-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=/opt/llvm-3.4"
  "-DLLVM_ENABLE_CXX11=ON" "-DLLVM_ENABLE_ASSERTIONS=OFF" "-DCMAKE_CXX_FLAGS=-std=c++11"
  "-DPYTHON_EXECUTABLE=/usr/bin/python2" "-DLLVM_BINUTILS_INCDIR=$BINUTILS_HEADERS"
  "-GUnix Makefiles")

if $is_redhat ; then
    cmake "${COMMON_OPTS[@]}" -DGCC_INSTALL_PREFIX=/opt/gcc ../llvm
else
    cmake "${COMMON_OPTS[@]}" ../llvm
fi

make

echo "Running Tests"
echo "NOTE: 4 failures are anticipated in Clang :: Driver"
set +e
make check-all
set -e
echo "NOTE: 8 failures were anticipated"
echo "Clang :: Driver/B-opt.c"
echo "Clang :: Driver/coverage-ld.c"
echo "Clang :: Driver/cross-linux.c"
echo "Clang :: Driver/prefixed-tools.c"
echo "Clang :: Driver/sanitizer-ld.c"
echo "SanitizerCommon-Unit :: Sanitizer-x86_64-Test/SanitizerCommon.SizeClassAllocator64CompactGetBlockBegin"
echo "SanitizerCommon-Unit :: Sanitizer-x86_64-Test/SanitizerCommon.SizeClassAllocator64GetBlockBegin"
echo "SanitizerCommon-Unit :: Sanitizer-x86_64-Test/SanitizerCommon.SizeClassAllocator64Overflow"

#Install to /opt
echo "Installing to $ORLY_BIN"
sudo rm -rf $ORLY_BIN
sudo mkdir $ORLY_BIN
sudo make install
ln -s "$ORLY_BIN/bin/clang++" tools/clang++
ln -s "$ORLY_BIN/bin/clang" tools/clang

cd /opt
sudo tar czf "$INSTALL_DIR.tgz" $INSTALL_DIR
sudo ln -sf "$INSTALL_DIR" llvm

#Cleanup
#TODO: rm -rf $SRC_ROOT $OBJ_ROOT
