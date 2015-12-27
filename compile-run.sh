
## protobuf and stan are submodules, each has its own submodules...
## so to clone this mess.
git clone https://github.com/sakrejda/protostan.git
# At this point, define a $ROOT directory 
# that's /whathever/something/protostan
cd $ROOT/protostan
git submodule update --init --recursive

## Generate stan library:
cd $ROOT/lib/stan
make bin/libstanc.a
mv $ROOT/lib/stan/bin/libstanc.a $ROOT/lib/

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
g++ -I $ROOT/lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
    -I $ROOT/src/cpp \
    -I $ROOT/lib/stan/src \
    -I $ROOT/lib/protobuf/src \
    -isystem $ROOT/lib/stan/lib/stan_math/lib/boost_1.58.0 \
    -pthread \
    -o $ROOT/test/unit/stanc-test \
    $ROOT/src/cpp/test/stanc-test.cpp \
    $ROOT/src/cpp/proto/stanc.pb.cc \
    $ROOT/lib/libgtest.a \
    $ROOT/lib/libstanc.a \
    $ROOT/lib/libprotobuf.a
$ROOT/test/unit/gtest-test
$ROOT/test/unit/stanc-test

