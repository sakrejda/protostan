default: test

libraries: lib/libstan.a lib/libgtest.a lib/libprotobuf.a

test-binaries: test/unit/gtest-test test/unit/stanc-test

generated: src/proto/stanc.pb.cc

test: libraries generated test-binaries
	@echo running tests
	test/unit/gtest-test
	test/unit/stanc-test

test/unit/gtest-test:
	mkdir -p test/unit
	g++ -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-pthread -o test/unit/gtest-test \
			src/test/gtest-test.cpp \
			lib/libgtest.a

test/unit/stanc-test:
	mkdir -p test/unit
	g++ -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o test/unit/stanc-test \
			src/test/stanc-test.cpp \
			src/proto/stanc.pb.cc \
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

lib/libstan.a: lib/stan
	$(MAKE) -C lib/stan bin/libstanc.a
	cp lib/stan/bin/libstanc.a lib

src/proto/stanc.pb.cc: proto/stanc.proto
	lib/protobuf/src/protoc --cpp_out=src ./proto/stanc.proto

.PHONY: test
