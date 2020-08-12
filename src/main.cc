#include "game/game.h"
#include "entity/entity.h"
#include "glog/logging.h"

int main(int argc, char* argv[]) {
#if defined(DEBUG)
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
#endif

    nyaa::AtExit at_exit(nyaa::AtExit::INITIALIZER);
    if (!nyaa::ThisGame->Prepare("assets/properties.txt")) { return -1; }

    nyaa::ThisGame->Run();
    return 0;
}