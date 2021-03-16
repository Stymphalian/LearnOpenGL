IMGUI_DIR=third_party/imgui

CXX=clang++
CFLAGS=-g -Wall -Wformat -std=c++17 -O3
INFLAGS=-I./ -I./include -I./$(IMGUI_DIR) -I./$(IMGUI_DIR)/backends
LDFLAGS= -lGLEW -lGL -lglfw -lrt -lm -ldl -lassimp -lstdc++fs

EXECUTABLE=noin
OBJDIR=build
SRCDIR=src

# proejct sources
RAW_SOURCES=main.cc hand_mesh.cc object.cc misc.cc camera.cc mesh.cc \
						 point.cc cloth.cc stb_image.cc time.cc light.cc model.cc
SOURCES = $(addprefix $(SRCDIR)/, $(RAW_SOURCES) )
OBJECTS = $(addprefix $(OBJDIR)/, $(RAW_SOURCES:.cc=.o) )

# 3P sources
IMGUI_SOURCES=imgui.cpp \
							imgui_draw.cpp \
							imgui_tables.cpp \
							imgui_widgets.cpp \
							backends/imgui_impl_glfw.cpp \
							backends/imgui_impl_opengl3.cpp
SOURCES += $(addprefix $(IMGUI_DIR)/, $(IMGUI_SOURCES) )
OBJECTS += $(addprefix $(OBJDIR)/$(IMGUI_DIR)/, $(IMGUI_SOURCES:.cpp=.o) )

#-g allow for debugging
#-E macro expanded
# -pg

#all: $(SOURCES) $(EXECUTABLE)
all: $(EXECUTABLE)
	ctags -R .

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(INFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/$(IMGUI_DIR)/%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(INFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/$(IMGUI_DIR)/backends/%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(INFLAGS) $(CFLAGS) -c -o $@ $<

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(INFLAGS) $(CFLAGS) $(LDFLAGS)

$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/third_party/imgui/backends

rm_noin:
	rm -f $(EXECUTABLE)

clean: rm_noin
	rm -rf $(OBJDIR) *.o
