#include "game/game.h"
#include "entity/entity.h"
#include "glog/logging.h"

class SimpleEntity : public nyaa::entity::Entity<SimpleEntity> {
public:
    SimpleEntity(int value): value_(value) {}

    DEF_VAL_GETTER(int, value);
private:
    int value_;
};

int main(int argc, char* argv[]) {
#if defined(DEBUG)
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
#endif
    
    nyaa::AtExit at_exit(nyaa::AtExit::INITIALIZER);
    if (!nyaa::ThisGame->Prepare()) {
        return -1;
    }

    nyaa::ThisGame->Run();
    return 0;
}