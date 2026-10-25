// [0.0.2 T21] specs/features/0.0.2-theme.md
#include <kgc/ui/SlotModel.h>
#include <kgc/ui/Slots.h>
#include <type_traits>
// T21: 네 기여 목록이 같은 바탕을 써서 동작이 갈라지지 않게 확인해요.
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::TopbarSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::StatusChipSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::RightTabSlot>);
static_assert(std::is_base_of_v<kgc::ui::SlotModel, kgc::ui::MapLayerSlot>);
int main() { return 0; }
