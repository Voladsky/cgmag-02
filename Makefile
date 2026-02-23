CFLAGS = -std=c++17 -O2 -I./include
LDFLAGS = -lglfw -lGL -ldl

OpenGLTest: refactor.cpp glad.c
	g++ $(CFLAGS) -o OpenGLTest refactor.cpp glad.c $(LDFLAGS)

.PHONY: test clean

test: OpenGLTest
	./OpenGLTest

clean:
	rm -f OpenGLTest