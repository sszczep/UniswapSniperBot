CXXFLAGS=-std=c++17 -O3 -Wall -Wextra -Werror -pthread
SOURCES=$(wildcard sources/*.cc)
INCLUDES_PATHS=$(wildcard libs.build/*/includes) includes
LIBRARIES_PATHS=$(wildcard libs.build/*)
LIBRARIES=secp256k1 XKCP

all: main

build-secp256k1:
ifeq ($(wildcard libs.build/secp256k1),)
	cd libs/secp256k1 && ./autogen.sh
	cd libs/secp256k1 && ./configure --disable-benchmark --disable-tests --disable-openssl_tests --disable-exhaustive_tests --enable-module_recovery
	cd libs/secp256k1 && make

	mkdir -p libs.build/secp256k1/includes

	cp libs/secp256k1/.libs/libsecp256k1.a libs.build/secp256k1/libsecp256k1.a
	cp -R libs/secp256k1/include/. libs.build/secp256k1/includes/
else
	@echo "secp256k1 already built. To rebuild, type make clean-secp256k1 and build again"
endif

clean-secp256k1:
	rm -Rf libs.build/secp256k1
	cd libs/secp256k1 && make clean

build-xkcp:
ifeq ($(wildcard libs.build/XKCP),)
	cd libs/XKCP && make generic64/libXKCP.a

	mkdir -p libs.build/XKCP/includes

	cp libs/XKCP/bin/generic64/libXKCP.a libs.build/XKCP/libXKCP.a
	cp -R libs/XKCP/bin/generic64/libXKCP.a.headers/. libs.build/XKCP/includes/
else
	@echo "XKCP already built. To rebuild, type make clean-xkcp and build again"
endif

clean-xkcp:
	rm -Rf libs.build/XKCP
	cd libs/XKCP && make clean

build-libs: build-secp256k1 build-xkcp

clean-libs: clean-secp256k1 clean-xkcp

main: build-libs $(SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES_PATHS:%=-I%) $(LIBRARIES_PATHS:%=-L%) $(SOURCES) main.cc $(LIBRARIES:%=-l%) -o build/$@

clean: clean-libs
	rm -Rf build