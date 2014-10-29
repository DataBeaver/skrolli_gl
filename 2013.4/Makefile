PACKAGES := sdl2 glew
CFLAGS := -Wall $(shell pkg-config --cflags $(PACKAGES))
LIBS := $(shell pkg-config --libs $(PACKAGES)) -lGL -lSDL2_image

skrolli_gl: main.o object.o shader.o texture.o mathutils.o
	g++ -o $@ $^ $(LIBS)

%.o: %.cpp
	g++ -c -o $@ $< $(CFLAGS)

clean:
	rm *.o
