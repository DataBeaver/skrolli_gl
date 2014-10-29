SOURCES := animation.cpp \
	bloom.cpp \
	camera.cpp \
	engine.cpp \
	framebuffer.cpp \
	group.cpp \
	instance.cpp \
	main.cpp \
	material.cpp \
	mathutils.cpp \
	object.cpp \
	shader.cpp \
	resourcemanager.cpp \
	rotationanimation.cpp \
	texture.cpp \
	translationanimation.cpp

PACKAGES := sdl2 glew

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
CFLAGS := -Wall $(shell pkg-config --cflags $(PACKAGES)) -ggdb
LIBS := $(shell pkg-config --libs $(PACKAGES)) -lGL -lSDL2_image

skrolli_gl: $(OBJECTS)
	g++ -o $@ $^ $(LIBS)

$(OBJECTS): %.o: source/%.cpp
	g++ -c -o $@ $< $(CFLAGS)

clean:
	rm *.o
