# Starting point:
ROOT="/home/krzysztof/packages/protostan"
cd $ROOT/..

## protobuf and stan are submodules, each has its own submodules...
## so to clone this mess.
git clone https://github.com/sakrejda/protostan.git
cd $ROOT/protostan
git submodule update --init --recursive

# To generate the protoc compiler, do a local in-package-dir install 
# so that protoc doesn't assume it can put its libraries into 
# system directories.
cd $ROOT/lib/protobuf
./autogen.sh
./configure --prefix=$ROOT
make && make install

# Use the protobuf compiler to produce message interface
# code:
cd $ROOT
bin/protoc --cpp_out=src/cpp ./proto/stanc.proto
bin/protoc --python_out=src/python ./proto/stanc.proto

# Compile google test library:
cd $ROOT/lib
g++ -I stan/lib/stan_math/lib/gtest_1.7.0/include \
    -I stan/lib/stan_math/lib/gtest_1.7.0/ -c \
    stan/lib/stan_math/lib/gtest_1.7.0/src/gtest-all.cc
ar -rv libgtest.a gtest-all.o

# Compile/link/run tests:
cd $ROOT
g++ -I $ROOT/lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
    -pthread -o $ROOT/test/unit/gtest-test \
    $ROOT/src/cpp/test/gtest-test.cpp \
    $ROOT/lib/libgtest.a
$ROOT/test/unit/gtest-test

