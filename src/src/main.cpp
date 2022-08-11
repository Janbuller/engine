#include "ecstest/GameApp.h"

int main(int argc, char *argv[]) {
    ecstest::GameApp app{640, 480, "Hello, World!"};
    app.Run();

    return 0;
}
