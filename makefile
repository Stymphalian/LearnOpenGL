CXX=clang++
CTYPE=cc
CFLAGS=-c -ansi -Wall -std=c++17 -O3
INFLAGS=-Iinclude
LDFLAGS= -lglfw -lm -lGL -lGLEW -lassimp -lstdc++fs

OBJDIR=build
SRCDIR=src

PURE_SOURCES=main.cc hand_mesh.cc object.cc misc.cc camera.cc mesh.cc \
						 point.cc cloth.cc stb_image.cc time.cc
SOURCES=$(addprefix $(SRCDIR)/, $(PURE_SOURCES) )
OBJECTS=$(addprefix $(OBJDIR)/, $(PURE_SOURCES:.cc=.o) )

EXECUTABLE=noin

#-g allow for debugging
#-E macro expanded
# -pg

all: $(SOURCES) $(EXECUTABLE)
	ctags -R .

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CFLAGS) $(INFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

rm_noin:
	rm -f noin

clean: rm_noin
	rm -rf $(OBJDIR) *.o
