CC = g++

CFLAGS = -g -ansi -std=gnu++2a -Wc++11-compat -Wc++14-compat -Wc++17-compat -Wall -Wextra -Weffc++ \
         -Walloca -Warray-bounds -Wcast-align -Wcast-qual -Wchar-subscripts -Wctor-dtor-privacy     \
		 -Wdangling-else -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 \
		 -Winline -Wlarger-than=8192 -Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual   \
		 -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-overflow=2          \
		 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused          \
		 -Wvariadic-macros -Wno-missing-field-initializers -Wnarrowing -Wno-old-style-cast -Wvarargs     \
		 -Walloca -Wdangling-else -fcheck-new -fsized-deallocation -fstack-check -fstrict-overflow        \
		 -fno-omit-frame-pointer


IFLAGS = -I

FRT_PROJ_NAME = frontend_
BCK_PROJ_NAME = backend_

FRT_SRC_DIR  = frontend
BCK_SRC_DIR  = backend
LIB_SRC_DIR  = lang_lib
STK_SRC_DIR  = $(LIB_SRC_DIR)/stack
STR_SRC_DIR  = $(LIB_SRC_DIR)/str_lib
TREE_SRC_DIR = $(LIB_SRC_DIR)/tree

LIB_OBJ_DIR = lang_lib/obj
FRT_OBJ_DIR = frontend/obj
BCK_OBJ_DIR = backend/obj

FRT_SRC  = $(wildcard $(FRT_SRC_DIR)/*.cpp)
BCK_SRC  = $(wildcard $(BCK_SRC_DIR)/*.cpp)
LIB_SRC  = $(wildcard $(LIB_SRC_DIR)/*.cpp)
STK_SRC  = $(wildcard $(STK_SRC_DIR)/*.cpp)
STR_SRC  = $(wildcard $(STR_SRC_DIR)/*.cpp)
TREE_SRC = $(wildcard $(TREE_SRC_DIR)/*.cpp)

FRT_OBJ  = $(patsubst $(FRT_SRC_DIR)/%.cpp, $(FRT_OBJ_DIR)/%.o, $(FRT_SRC))
BCK_OBJ  = $(patsubst $(BCK_SRC_DIR)/%.cpp, $(BCK_OBJ_DIR)/%.o, $(BCK_SRC))
LIB_OBJ  = $(patsubst $(LIB_SRC_DIR)/%.cpp, $(LIB_OBJ_DIR)/%.o, $(LIB_SRC))
STK_OBJ  = $(patsubst $(STK_SRC_DIR)/%.cpp, $(LIB_OBJ_DIR)/%.o, $(STK_SRC))
STR_OBJ  = $(patsubst $(FRT_SRC_DIR)/%.cpp, $(LIB_OBJ_DIR)/%.o, $(STR_SRC))
TREE_OBJ = $(patsubst $(FRT_SRC_DIR)/%.cpp, $(LIB_OBJ_DIR)/%.o, $(TREE_SRC))

all:
	make backend
	make frontend

clean: clean_frt clean_bck

frontend: $(STK_OBJ) $(STR_OBJ) $(TREE_OBJ) $(FRT_OBJ) $(LIB_OBJ)
	$(CC) $^ -o $(FRT_PROJ_NAME)

backend: $(STK_OBJ) $(STR_OBJ) $(TREE_OBJ) $(BCK_OBJ) $(LIB_OBJ)
	$(CC) $^ -o $(BCK_PROJ_NAME)

$(FRT_OBJ_DIR)/%.o : $(FRT_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

$(BCK_OBJ_DIR)/%.o : $(BCK_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

$(LIB_OBJ_DIR)/%.o : $(LIB_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@
	
$(LIB_OBJ_DIR)/%.o : $(STK_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

$(LIB_OBJ_DIR)/%.o : $(STR_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

$(LIB_OBJ_DIR)/%.o : $(TREE_SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(IFLAGS) $(CFLAGS) -c $^ -o $@

clean_frt:
	rm -rf $(FRT_OBJ_DIR) $(LIB_OBJ_DIR)
	rm $(FRT_PROJ_NAME)

clean_bck:
	rm -rf $(BCK_OBJ_DIR) $(LIB_OBJ_DIR)
	rm $(BCK_PROJ_NAME)
