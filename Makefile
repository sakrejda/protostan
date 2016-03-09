
default: test

libraries: lib/libstanc.a lib/libgtest.a lib/libprotobuf.a

test-binaries: test/unit/compile-test test/unit/rw_delimited_pb-test test/unit/binary_proto_filestream_writer-test test/unit/binary_proto_ostream_writer-test

generated:
	./lib/protobuf/src/protoc --cpp_out=src/stan/proto --proto_path=src/stan/proto src/stan/proto/*.proto

test: libraries generated test-binaries 
	@echo running tests
	test/unit/compile-test
	test/unit/rw_delimited_pb-test
	test/unit/binary_proto_filestream_writer-test
	test/unit/binary_proto_ostream_writer-test

test-full: test cpplint

test/unit/compile-test: src/test/compile-test.cpp
	mkdir -p test/unit
	g++ -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o test/unit/compile-test \
			src/test/compile-test.cpp \
			src/stan/proto/compile.pb.cc \
			lib/libgtest.a \
			lib/libstanc.a \
			lib/libprotobuf.a

test/unit/rw_delimited_pb-test: src/test/rw_delimited_pb-test.cpp
	mkdir -p test/unit
	g++ -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o test/unit/rw_delimited_pb-test \
			src/test/rw_delimited_pb-test.cpp \
			src/stan/proto/stan-core.pb.cc \
			src/stan/proto/compile.pb.cc \
			src/stan/proto/sample.pb.cc \
			lib/libgtest.a \
			lib/libstanc.a \
			lib/libprotobuf.a

test/unit/binary_proto_filestream_writer-test: src/test/binary_proto_filestream_writer-test.cpp
	mkdir -p test/unit
	g++ -std=c++0x -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o test/unit/binary_proto_filestream_writer-test \
			src/test/binary_proto_filestream_writer-test.cpp \
			src/stan/proto/stan-core.pb.cc \
			src/stan/proto/compile.pb.cc \
			src/stan/proto/sample.pb.cc \
			lib/libgtest.a \
			lib/libstanc.a \
			lib/libprotobuf.a

test/unit/binary_proto_ostream_writer-test: src/test/binary_proto_ostream_writer-test.cpp
	mkdir -p test/unit
	g++ -std=c++0x -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o test/unit/binary_proto_ostream_writer-test \
			src/test/binary_proto_ostream_writer-test.cpp \
			src/stan/proto/stan-core.pb.cc \
			src/stan/proto/compile.pb.cc \
			src/stan/proto/sample.pb.cc \
			lib/libgtest.a \
			lib/libstanc.a \
			lib/libprotobuf.a

lib/libprotobuf.a: lib/protobuf
	cd lib/protobuf && ./autogen.sh && ./configure && make
	cp lib/protobuf/src/.libs/libprotobuf.a lib

lib/libgtest.a: lib/stan
	g++ -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I lib/stan/lib/stan_math/lib/gtest_1.7.0/ \
			-c lib/stan/lib/stan_math/lib/gtest_1.7.0/src/gtest-all.cc \
			-o lib/gtest-all.o
	ar -rv lib/libgtest.a lib/gtest-all.o

lib/libstanc.a: lib/stan
	$(MAKE) -C lib/stan bin/libstanc.a
	cp lib/stan/bin/libstanc.a lib

cpplint:
	python2 lib/stan/lib/stan_math/lib/cpplint_4.45/cpplint.py \
		--output=vs7 \
		--counting=detailed \
		--root=src \
		--extension=hpp,cpp \
		--filter=-runtime/indentation_namespace,-readability/namespace,-legal/copyright,-whitespace/indent,-whitespace/line_length,-runtime/reference \
		$(shell find src/protostan -name '*.hpp' -o -name '*.cpp')

.PHONY: cpplint test
