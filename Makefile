CXX = gcc

EXE = BTSearch
SOURCES = main.c
SOURCES += sqlite3.c sql.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CXXFLAGS = 
CXXFLAGS += -g -Wall -Wformat
LIBS =
LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf


%.o:%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	del $(EXE) $(OBJS)
