
all:
	g++ -dynamiclib -std=c++11 -O2 example-extension.cpp -o shadron-example.dylib

install:
	mkdir -p ~/.config/Shadron/extensions
	cp -f shadron-example.dylib ~/.config/Shadron/extensions/shadron-example.dylib

clean:
	rm -f shadron-example.dylib
