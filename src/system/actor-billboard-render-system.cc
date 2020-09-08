#include "system/actor-billboard-render-system.h"
#include "component/property-components.h"
#include "resource/text-library.h"
#include "resource/shader-library.h"
#include "resource/font-library.h"
#include "game/matrix.h"
#include "game/game.h"
#include "base/queue-macros.h"
#include <GL/glew.h>
#include <math.h>

namespace nyaa {

namespace sys {

ActorBillboardRenderSystem::ActorBillboardRenderSystem() {
    dummy_.next_ = &dummy_;
    dummy_.prev_ = &dummy_;
}

void ActorBillboardRenderSystem::Render(const Vector3f &position, const Vector3f &color, EntityId id,
                                        com::NaturePropertiesComponent *nature, Vector3f *view) {
    std::string name = nature->name();
    if (nature->name_id() != res::MAX_TEXT_ID) { name = Game::This()->text_lib()->Load(nature->name_id()); }

    res::BillboardShaderProgram *shader = Game::This()->shader_lib()->billboard_program();

    Matrix      model, mat;
    VboEntry *  entry        = EnsureBufferdVbo(name, id);
    const float scale_factor = 0.3 / entry->size.y;
    if (view) {
        Vector3f pos = position - *view;
        model.Translate(pos.x - entry->size.x / 2 * scale_factor + 0.5, pos.y, pos.z + 2);
    } else {
        model.Translate(0, 0, 1);
    }
    mat.Scale(scale_factor, scale_factor, scale_factor);
    model.Multiply(mat);
    mat.Rotate(-1, 0, 0, 80);
    model.Multiply(mat);
    shader->SetCenterPosition(entry->center);
    shader->SetModelMatrix(model);
    shader->SetSize(Vec2(0.02 / entry->size.y, 0.02 / entry->size.y));
    shader->SetPaintColor(color);

    glBindBuffer(GL_ARRAY_BUFFER, entry->vbo);
    shader->Enable();
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, Game::This()->font_lib()->system_face()->buffered_tex());
    glDrawArrays(GL_QUADS, 0, entry->count);
    glEnable(GL_DEPTH_TEST);
    shader->Disable();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ActorBillboardRenderSystem::VboEntry *  // :format
    ActorBillboardRenderSystem::EnsureBufferdVbo(const std::string &name, EntityId id) {
    auto iter = entries_.find(id);
    if (iter != entries_.end() && iter->second->text == name) {
        QUEUE_REMOVE(iter->second);
        QUEUE_INSERT_TAIL(dummy(), iter->second);
        return iter->second;
    }

    VboEntry *entry = iter != entries_.end() ? iter->second : nullptr;
    if (entry) {
        QUEUE_REMOVE(entry);
        glDeleteBuffers(1, &entry->vbo);
    } else {
        entry = new VboEntry;
        glGenBuffers(1, &entry->vbo);
    }
    if (entries_.size() + 1 >= max_entries_) {
        VboEntry *head = dummy()->next_;
        QUEUE_REMOVE(head);
        entries_.erase(head->id);
        delete head;
    }
    entry->id   = id;
    entry->text = name;

    glBindBuffer(GL_ARRAY_BUFFER, entry->vbo);

    std::vector<float> vertices;

    Boundf bound = Game::This()->font_lib()->system_face()->Render(Vec3(0,0,0), 1.0, name, &vertices);

    entry->count  = vertices.size() / 5;
    entry->size.x = bound.w;
    entry->size.y = bound.h;
    entry->center = Vec3(bound.x + entry->size.x / 2, bound.y + entry->size.y / 2, 0);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    QUEUE_INSERT_TAIL(dummy(), entry);
    entries_[id] = entry;
    return entry;
}

}  // namespace sys

}  // namespace nyaa
