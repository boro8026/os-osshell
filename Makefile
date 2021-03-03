CXX= g++
CXXFLAGS= -std=c++17

INCLUDE= 
LIB= 

OBJDIR= obj
SRCDIR= src
BINDIR= bin

OBJS= $(addprefix $(OBJDIR)/, osshell.o)
EXEC= $(addprefix $(BINDIR)/, osshell)

mkdirs:= $(shell mkdir -p $(OBJDIR) $(BINDIR))

# BUILD EVERYTHING
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)


# REMOVE OLD FILES
clean:
	rm -f $(OBJS) $(EXEC)
