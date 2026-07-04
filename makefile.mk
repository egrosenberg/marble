all: bin/test

.PHONY: all

flags=-Ilib -Wall

build:
	@echo "building directory $@"
	mkdir $@

bin/test: build/index.o build/song.o | build
	@echo building $@
	g++ $(flags) -o $@ $^

build/index.o: src/index.cpp src/lib.h | build
	@echo building $@
	g++ $(flags) -o $@ -c $<

build/song.o: src/song.cpp src/song.h src/lib.h | build
	@echo building $@
	g++ $(flags) -o $@ -c $<

clean:
	@echo cleaning...
	rm -f build/*