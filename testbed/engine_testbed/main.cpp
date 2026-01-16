#include <freya.h>

int main(int argc, char** argv) {
  freya::AssetGroupID group_id = freya::asset_group_create("dungeon_alpha");
  freya::asset_group_build(group_id, "../../assets/asset_list.frlist", "dungeon_alpha.frpkg");
}
