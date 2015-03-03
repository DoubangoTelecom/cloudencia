# Ragel generator
# For more information about Ragel: http://www.complang.org/ragel/

export OPTIONS="-C -L -T0"

ragel.exe $OPTIONS -o ./src/ca_parser_url.cc ./ragel/ca_parser_url.rl
