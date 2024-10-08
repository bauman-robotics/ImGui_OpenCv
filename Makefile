# Укажите компилятор
CXX = g++

# Укажите флаги компиляции
CXXFLAGS = -std=c++17 -I/usr/include/opencv4 -Iimgui -Ibackends -Icore/inc $(DEBUG_FLAGS)

# Укажите флаги для отладки
DEBUG_FLAGS = -g

# Укажите файлы исходного кода
SRCS = $(shell find core/src -name '*.cpp') \
       imgui/imgui.cpp \
       imgui/imgui_draw.cpp \
       imgui/imgui_widgets.cpp \
       imgui/imgui_demo.cpp \
       imgui/imgui_tables.cpp \
       backends/imgui_impl_glfw.cpp \
       backends/imgui_impl_opengl3.cpp

# Укажите библиотеки для линковки
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lglfw -lGL -lGLEW -ldl -lX11 -pthread

# Укажите директорию для объектных файлов
OBJDIR = build

# Укажите директорию для исполняемого файла
BINDIR = bin

# Создайте список объектных файлов
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Укажите выходной файл
TARGET = $(BINDIR)/imGui_test

# Правило по умолчанию для сборки
all: $(OBJDIR) $(BINDIR) $(TARGET)

# Правило для создания директории объектных файлов
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Правило для создания директории исполняемого файла
$(BINDIR):
	mkdir -p $(BINDIR)

# Правило для компиляции и линковки
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Правило для компиляции объектных файлов
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Генерация файлов зависимостей
DEPS = $(OBJS:.o=.d)

-include $(DEPS)

$(OBJDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MM -MT '$(@:.d=.o)' $< > $@

# Правило для очистки
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean