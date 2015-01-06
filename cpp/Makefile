# Library name
LIBNAME := telematics

# The library file
LIBFILE := lib/lib$(LIBNAME).dylib

# Source files for library
SOURCES := $(wildcard src/library/src/*.cpp)

# Object files
OBJECTS := $(patsubst src/library/src/%.cpp,obj/library/%.o,$(SOURCES))

# Header files
INCLUDE := -I./src/library/include

obj/library/%.o : src/library/src/%.cpp
	@mkdir -p obj/library
	clang++ -std=c++11 -stdlib=libc++ -c -O3 -pedantic-errors -Werror $(INCLUDE) -o$@ $<

.PHONY : lib
lib : $(LIBFILE)

$(LIBFILE): $(OBJECTS)
	@mkdir -p lib
	clang++ -dynamiclib -o $@ -fPIC -lc++ $(OBJECTS)


# Rule for builing applications from the files and linking to the libraries
bin/% : src/applications/%.cpp
	@mkdir -p bin
	clang++ -std=c++11 -stdlib=libc++ $(INCLUDE) -o$@ -L./lib -l$(LIBNAME) $<


.PHONY : readFromCSV
readFromCSV : bin/$(APP_READFROMCSV)

.PHONY : clean
clean :
	@rm -f $(OBJECTS)
	@rm -f $(LIBFILE)
	@rm -f *~
	@rm -f bin/*

.PHONY : run
run : bin/readFromCSV
	@$<
