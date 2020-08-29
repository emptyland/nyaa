#include "resource/ai-def.h"
#include "base/lazy-instance.h"
#include "glog/logging.h"
#include <unordered_map>

namespace nyaa {

namespace res {

const char *kAINames[AI::MAX_AI_KIND] = {
#define DEFINE_AI_NAME(name) #name,
    DECLARE_AI_KINDS(DEFINE_AI_NAME)
#undef DEFINE_AI_NAME
};

using AINameTable = std::unordered_map<std::string_view, AI::Kind>;

struct AINameInitializer {
    static AINameTable *New(void *chunk) {
        AINameTable *table = new (chunk) AINameTable();
        for (int i = 0; i < AI::MAX_AI_KIND; i++) {
            (*table)[std::string_view(kAINames[i])] = static_cast<AI::Kind>(i);
        }
        return table;
    }

    static void Delete(void *obj) { static_cast<AINameTable *>(obj)->~AINameTable(); }
};

/*static*/ AI::Kind AI::OfName(std::string_view name) {
    static base::LazyInstance<AINameTable, AINameInitializer> name_table;
    auto iter = name_table->find(name);
    DCHECK(iter != name_table->end());
    return iter->second;
}

/*static*/ const char *AI::Name(Kind kind) {
    int i = static_cast<int>(kind);
    DCHECK_GE(i, 0);
    DCHECK_LT(i, MAX_AI_KIND);
    return kAINames[i];
}

}  // namespace res

}  // namespace nyaa