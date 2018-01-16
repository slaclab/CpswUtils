-include ../config.mak

ifdef BR_VERSION
	CXX	     = $(XCROSS_HOME)g++
	TARCH    = $(BR_VERSION)-x86_64
else
	CXX    = g++
	TARCH  = $(H_VERSION)-x86_64
endif

OBJDIR   = O.$(TARCH)
SRCDIR   = .
BINDIR   = O.$(TARCH)
INSTALL_DIR = ..

DEPS     =
SRCS     = $(wildcard $(SRCDIR)/*.cpp)
OBJS     = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
HEADERS  = $(wildcard $(SRCDIR)/*.h)
TARGET   = $(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%, $(SRCS))

CXXFLAGS  = -g -std=c++0x -fPIC
CXXFLAGS += -I. 
CXXFLAGS += -I$(CPSW_DIR)/include
CXXFLAGS += -I$(BOOST_DIR)/include
CXXFLAGS += -I$(YAML_CPP_DIR)/include

LIBS_EXT = -lcpsw -lyaml-cpp -lboost_system -lpthread -lrt -ldl

LDIR  = -L. 
LDIR += -L$(CPSW_DIR)/lib
LDIR += -L$(BOOST_DIR)/lib
LDIR += -L$(YAML_CPP_DIR)/lib

.PHONY: all clean insatll uninstall

all: $(TARGET)

$(BINDIR):
	@mkdir -p $@

$(BINDIR)/%: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) -o $@ $^ -static $(CXXFLAGS) $(LDIR) $(LIBS_EXT)

clean:
	$(RM) -rf $(OBJDIR)
	$(RM) -rf $(BINDIR)

install: $(TARGET)
	@mkdir -p $(INSTALL_DIR)/$(TARCH)/bin
	install -C $(TARGET) $(INSTALL_DIR)/$(TARCH)/bin/

uninstall:
	$(RM) -rf $(INSTALL_DIR)/$(TARCH)
