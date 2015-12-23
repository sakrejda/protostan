# Starting point:
ROOT="/home/krzysztof/packages/protostan"
cd $ROOT/..

## protobuf and stan are submodules, each has its own submodules...
## so to clone this mess.
git clone https://github.com/sakrejda/protostan.git
cd $ROOT/protostan
git submodule update --init --recursive

# To generate the protoc compiler:
cd $ROOT/lib/protobuf
./autogen.sh
./configure --prefix=$ROOT
make && make install

# Use the protobuf compiler to produce message interface
# code:
cd $ROOT
bin/protoc --cpp_out=src/cpp ./proto/stanc.proto
bin/protoc --python_out=src/python ./proto/stanc.proto




