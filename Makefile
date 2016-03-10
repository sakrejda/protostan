CXX ?= g++
PROTOC ?= protoc
PROTO_DIR = src/stan/proto
PROTO_SRCS = $(shell find $(PROTO_DIR) -name "*.proto")
PROTO_GENERATED = $(subst .proto,.pb.cc,$(PROTO_SRCS))
TEST_DIR = src/test/
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.cpp")
TEST_BINS = $(subst .cpp,,$(TEST_SRCS))
TEST_LIBS = \
  lib/libstanc.a \
	lib/libprotobuf.a \
  lib/libgtest.a \

default: test

libraries: $(TEST_LIBS)

%-test: %-test.cpp $(TEST_LIBS) $(PROTO_GENERATED)
	$(CXX) -std=c++11 -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
			-I src \
			-I lib/stan/src \
			-I lib/protobuf/src \
			-isystem lib/stan/lib/stan_math/lib/boost_1.58.0 \
			-pthread \
			-o $@ \
			$< \
			$(PROTO_GENERATED) \
			$(TEST_LIBS)

%.pb.cc: %.proto
	$(PROTOC) --cpp_out=$(PROTO_DIR) --proto_path=$(PROTO_DIR) $<

test: $(TEST_BINS)
	@echo running tests
	$(foreach bin,$(TEST_BINS), $(bin);)

test-full: test cpplint

lib/libprotobuf.a: lib/protobuf
	cd lib/protobuf && ./autogen.sh && ./configure && make
	cp lib/protobuf/src/.libs/libprotobuf.a lib

lib/libgtest.a: lib/stan
	$(CXX) -I lib/stan/lib/stan_math/lib/gtest_1.7.0/include \
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

clean:
	rm -f $(TEST_BINS) $(PROTO_GENERATED)

.PHONY: default libraries test cpplint clean
