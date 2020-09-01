export DYLD_LIBRARY_PATH=../nanogui/build:$DYLD_LIBRARY_PATH
if g++ -o main.out main.cpp -I../nanogui/include -I../nanogui/ext/eigen -I../nanogui/ext/nanovg/src -I.. -std=c++11 -framework OpenGL -L../nanogui/build -lGLEW -lnanogui
then
./main.out
fi
