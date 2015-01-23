# Library name
LIBNAME := telematics

# The library file
LIBFILE := lib/lib$(LIBNAME).dylib

# Source files for library
SOURCES := $(wildcard src/library/src/*.cpp)

# Object files
OBJECTS := $(patsubst src/library/src/%.cpp,obj/library/%.o,$(SOURCES))

# Application files
APPSOURCES := $(wildcard src/applications/*.cpp)
APPBINS := $(patsubst src/applications/%.cpp,bin/%,$(APPSOURCES))

# Header files
INCLUDE := -I./src/library/include

# General rules
.PHONY : all
all : lib apps

.PHONY : lib
lib : $(LIBFILE)

.PHONY : apps
apps : $(APPBINS)

.PHONY : clean
clean :
	@rm -f $(OBJECTS)
	@rm -f $(LIBFILE)
	@rm -f $(APPBINS)
	@rm -f *~ src/applications/*~ src/library/*/*~


# Rules for building individual files
obj/library/%.o : src/library/src/%.cpp
	@mkdir -p obj/library
	clang++ -std=c++11 -stdlib=libc++ -c -O3 -pedantic-errors -Werror $(INCLUDE) -o $@ $<

$(LIBFILE): $(OBJECTS)
	@mkdir -p lib
	clang++ -dynamiclib -o $@ -fPIC -lc++ $(OBJECTS)

bin/% : src/applications/%.cpp $(LIBFILE)
	@mkdir -p bin
	clang++ -std=c++11 -stdlib=libc++ $(INCLUDE) -o $@ -L./lib -l$(LIBNAME) $<
