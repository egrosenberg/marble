all: build/test

.PHONY: all

build:
	@echo "building directory $@"
	mkdir $@

build/test: build/index.o build/song.o | build
	@echo building $@
	g++ -I lib -msse2 -mavx2 -latomic -o $@ $^

build/index.o: src/index.cpp src/lib.h | build
	@echo building $@
	g++ -I lib -o $@ -c $<

build/song.o: src/song.cpp src/song.h src/lib.h | build
	@echo building $@
	g++ -I lib -o $@ -c $<

clean:
	@echo cleaning...
	rm -f build/*