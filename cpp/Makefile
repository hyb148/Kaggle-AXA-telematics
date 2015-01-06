# Source files
SOURCES := $(wildcard src/*.cpp)

# Object files
OBJECTS := $(patsubst src/%.cpp,obj/%.o,$(SOURCES))

INCLUDE := -I./src

obj/%.o : src/%.cpp
	clang -c -O3 -pedantic-errors -Werror $(INCLUDE) -o$@ $<

bin/explore : $(OBJECTS)
	clang -lstdc++ $(OBJECTS) -o $@

.PHONY : clean
clean :
	@rm -f $(OBJECTS)
	@rm -f *~

.PHONY : run
run : bin/explore
	$<
