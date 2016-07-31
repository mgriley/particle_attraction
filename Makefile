CXX = clang++
SDL = -framework SDL2

CXXFLAGS = -Wall -c -std=c++11 -g
LDFLAGS = $(SDL)
EXE = sim

all: $(EXE)

# NB: $< is name of prerequisite of the rule, $@ is name of target of rule

#link
$(EXE): main.o v2.o Simulation.o; $(CXX) $(LDFLAGS) $< -o $@
#compile
main.o: main.cpp ; $(CXX) $(CXXFLAGS) $< -o $@
Simulation.o: Simulation.cpp ; $(CXX) $(CXXFLAGS) $< -o $@
v2.o: v2.cpp ; $(CXX) $(CXXFLAGS) $< -o $@

clean: ; rm *.o $(EXE)
