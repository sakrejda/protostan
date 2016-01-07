# protostan

The goal of <b>protostan</b> is to provide an API for 
[Stan](github.com/stan-dev/stan) based solely on passing 
(and returning) [protobuf](https://developers.google.com/protocol-buffers/docs/overview)
messages.  The advantages of this approach are:

- As long as a language has protobuf (version 3) support, a
  Stan interface can be written using solely the language and
  its protobuf package without the need to interface with C++.
- Protobuf has [impressive](https://developers.google.com/protocol-buffers/docs/reference/other?hl=en) 
  multi-language support, with apparent priority given to the 
  C++ interface among others.  Other languages important to Stan
  (e.g.-R, Python) also have solid support.
- Protobuf deals with [strongly-typed
  data](https://developers.google.com/protocol-buffers/docs/proto3)
  and offers well-defined mappings from those types to different
  language-specific types.  

We would like to wrap enough of the API so that a first interface
for a language could be written using protobuf without touching
the C++ API.  For the time being this repository should contain:

- Stan-related .proto files for other projects.
- Versions of Stan external API functions with only protobuf 
  messages as input and output types.

Location of Protocol Buffer Message Definitions
===============================================

Message definitions are found in ``src/stan/proto``.


Quickstart
==========

1. Clone repository and submodules

```
git clone https://github.com/sakrejda/protostan.git
cd protostan
git submodule update --init --recursive
```

2. Ensure static libraries exist

```
make libraries
```

3. Run tests

```
make test
```
