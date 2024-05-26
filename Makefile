define HELPMESSAGE
make              -> generates normal binary
make debug        -> generates debug binary
make [debug] v=n  -> uses C++ with version n, with n = 11 (default), 14, 17, ecc.
endef

v ?= 11

s_files = trainitez.cpp big_lib.cpp neural_web.cpp read_mnist.cpp readbindataset.cpp settings.cpp

h_files = readwritebin.h big_lib.h neural_web.h read_mnist.h readbindataset.h settings.h

libs =

trainitez: $(s_files) $(h_files)
	$(CXX) -std=gnu++$(v) -Wall -Wextra -O2 -o trainitez $(s_files) $(libs)

debug: trainitez_debug

trainitez_debug: $(s_files) $(h_files)
	$(CXX) -std=gnu++$(v) -Wall -Wextra -g -o trainitez_debug $(s_files) $(libs)

export HELPMESSAGE
help:
	@echo "$$HELPMESSAGE"
