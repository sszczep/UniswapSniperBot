CXXFLAGS=-std=c++17 -O3
SOURCES=$(wildcard sources/*.cc)
INCLUDES_PATHS=$(wildcard libs.build/*/includes) /usr/local/opt/openssl/include includes
LIBRARIES_PATHS=$(wildcard libs.build/*) /usr/local/opt/openssl/lib
LIBRARIES=pthread ssl crypto secp256k1 XKCP

TEST_CXXFLAGS=-std=c++17
TEST_SOURCES=$(wildcard tests/*.cc) $(SOURCES)
TEST_LIBRARIES =$(LIBRARIES) gtest gmock gtest_main gmock_main

all: main

build-websocketpp:
ifeq ($(wildcard libs.build/websocketpp),)
	mkdir -p libs.build/websocketpp/includes

	cp -R libs/websocketpp/websocketpp libs.build/websocketpp/includes
else
	@echo "websocketpp already built. To rebuild, type make clean-websocketpp and build again"
endif

clean-websocketpp:
	rm -Rf libs.build/websocketpp

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

build-gtest:
ifeq ($(wildcard libs.build/gtest),)
	mkdir -p libs/googletest/build
	cd libs/googletest/build && cmake ..
	cd libs/googletest/build && make

	mkdir -p libs.build/gtest/includes/gtest
	mkdir -p libs.build/gtest/includes/gmock

	cp -R libs/googletest/build/lib/. libs.build/gtest/
	cp -R libs/googletest/googletest/include/gtest/. libs.build/gtest/includes/gtest/
	cp -R libs/googletest/googlemock/include/gmock/. libs.build/gtest/includes/gmock/
else
	@echo "gtest already built. To rebuild, type make clean-gtest and build again"
endif

clean-gtest:
	rm -Rf libs.build/gtest
	rm -Rf libs/googletest/build

build-doxygen:
ifeq ($(wildcard libs.build/doxygen),)
	mkdir -p libs/doxygen/build
	cd libs/doxygen/build && cmake -G "Unix Makefiles" ..
	cd libs/doxygen/build && make

	mkdir -p libs.build/doxygen/

	cp libs/doxygen/build/bin/doxygen libs.build/doxygen/doxygen
else
	@echo "doxygen already built. To rebuild, type make clean-doxygen and build again"
endif

clean-doxygen:
	rm -Rf libs.build/doxygen
	rm -Rf libs/doxygen/build

build-libs: build-websocketpp build-secp256k1 build-xkcp

clean-libs: clean-websocketpp clean-secp256k1 clean-xkcp

main: build-libs $(SOURCES)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES_PATHS:%=-I%) $(LIBRARIES_PATHS:%=-L%) $(SOURCES) main.cc $(LIBRARIES:%=-l%) -o build/$@

test: build-gtest $(TEST_SOURCES) $(SOURCES)
	mkdir -p build
	$(CXX) $(TEST_CXXFLAGS) $(INCLUDES_PATHS:%=-I%) $(LIBRARIES_PATHS:%=-L%) $(TEST_SOURCES) $(TEST_LIBRARIES:%=-l%) -o build/$@

docs: build-doxygen
	libs.build/doxygen/doxygen doxygen/Doxyfile

clean: clean-libs
	rm -Rf main