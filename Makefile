EXENAME:=integral
SRCs:=$(wildcard *.cpp)
OBJs:=$(SRCs:.cpp=.o)
CXXFLAGS="c++0x"
$(EXENAME):$(OBJs)
%.o:%.cpp
	@mpic++ -std=$(CXXFLAGS) -o $@ -c $<$(CFLAGS)
	@echo "\033[92mCompiling $^\033[0m"
