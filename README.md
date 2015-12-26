# protostan

The goal of <b>protostan</b> is to provide an API for 
[Stan](github.com/stan-dev/stan) based solely on passing 
(and returning) [protobuf](https://developers.google.com/protocol-buffers/docs/overview)
messages.  The advantages of this approach are:

- As long as a language has protobuf (version 3) support, a
  Stan interface can be written using solely the language and
  its protobuf package without the need to interface with C++.
- 



