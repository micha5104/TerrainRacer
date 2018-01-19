GOOGLE_TEST_PATH=../../../googletest
GLM_PATH=../../../glm-0.9.8.5

SRC=$(wildcard *.cpp)
SRC+=$(wildcard utils/*.cpp)
HDR=$(wildcard *.h)
HDR+=$(wildcard utils/*.h)

TST_SRC=$(wildcard test/*.cpp)
TST_HDR=$(GOOGLE_TEST_PATH)/googletest/include/gtest/*.h

CXX = /opt/local/bin/ccache g++

OBJ_DIR:=.o

VPATH := utils

#OBJS := $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))
OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

TST_OBJS := $(addprefix $(OBJ_DIR)/,$(subst test/,,$(TST_SRC:.cpp=.o)))
TST_OBJS += $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

BINARY:=experiment
TST_BINARY:=unittests

INCLUDES := -Wno-deprecated \
	-std=c++11 \
	-g \
	-I/System/Library/Frameworks/OpenGL.framework/Headers \
	-I/opt/X11/include \
	-I$(GOOGLE_TEST_PATH)/googletest/include \
	-I$(GOOGLE_TEST_PATH)/googletest \
	-I$(GLM_PATH) \
	-I/opt/local/include

LIBS := -L/opt/X11/lib -framework OpenGL -framework GLUT -L/opt/local/lib -ljpeg

all: $(BINARY) $(TST_BINARY)
	@./$(TST_BINARY)

$(OBJS): $(HDR) | $(OBJ_DIR)

$(TST_OBJS): $(HDR) $(TST_HDR)

$(OBJ_DIR)/%.o: %.cpp
	@echo " CXX "$<
	$(CXX) -c $< -o $@ $(INCLUDES)

$(OBJ_DIR)/%.o: test/%.cpp
	@echo " CXX "$<
	$(CXX) -c $< -o $@ $(INCLUDES)

$(OBJ_DIR)/gtest-all.o: $(GOOGLE_TEST_PATH)/googletest/src/gtest-all.cpp
	@echo " CXX "$<
	$(CXX) -c $< -o $@ $(INCLUDES)

$(BINARY): $(OBJS)
	@echo "  LD "$<
	$(CXX) -o $(BINARY) $(OBJS) $(LIBS)

$(TST_BINARY): $(TST_OBJS)
	@echo "  LD "$<
	$(CXX) -o $(TST_BINARY) $(TST_OBJS) $(LIBS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	@rm -f $(OBJS)

realclean:
	@rm -f $(BINARY)
	@rm -f $(TST_BINARY)
	@rm -rf $(OBJ_DIR)

printvar:
	@echo "SRC: "$(SRC)
	@echo "OBJS: "$(OBJS)
	@echo "TST_SRC: "$(TST_SRC)
	@echo "TST_OBJS: "$(TST_OBJS)
