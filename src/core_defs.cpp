#include "core_defs.hpp"

#include "items/ItemDef.hpp"
#include "content/Content.hpp"
#include "content/ContentBuilder.hpp"
#include "files/files.hpp"
#include "files/engine_paths.hpp"
#include "window/Window.hpp"
#include "window/Events.hpp"
#include "window/input.hpp"
#include "voxels/Block.hpp"

// All in-game definitions (blocks, items, etc..)
void corecontent::setup(EnginePaths* paths, ContentBuilder* builder) {
    {
        Block& block = builder->blocks.create(CORE_AIR);
        block.replaceable = true;
        block.drawGroup = 1;
        block.lightPassing = true;
        block.skyLightPassing = true;
        block.obstacle = false;
        block.selectable = false;
        block.model = BlockModel::none;
        block.pickingItem = CORE_EMPTY;
    }
    {
        ItemDef& item = builder->items.create(CORE_EMPTY);
        item.iconType = ItemIconType::NONE;
    }

    auto bindsFile = paths->getResourcesFolder()/fs::path("bindings.toml");
    if (fs::is_regular_file(bindsFile)) {
        Events::loadBindings(
            bindsFile.u8string(), files::read_string(bindsFile), BindType::BIND
        );
    }

    {
        Block& block = builder->blocks.create(CORE_OBSTACLE);
        for (uint i = 0; i < 6; i++) {
            block.textureFaces[i] = "obstacle";
        }
        block.hitboxes = {AABB()};
        block.breakable = false;
        ItemDef& item = builder->items.create(CORE_OBSTACLE+".item");
        item.iconType = ItemIconType::BLOCK;
        item.icon = CORE_OBSTACLE;
        item.placingBlock = CORE_OBSTACLE;
        item.caption = block.caption;
    }
    {
        Block& block = builder->blocks.create(CORE_STRUCT_AIR);
        for (uint i = 0; i < 6; i++) {
            block.textureFaces[i] = "struct_air";
        }
        block.drawGroup = -1;
        block.skyLightPassing = true;
        block.lightPassing = true;
        block.hitboxes = {AABB()};
        block.obstacle = false;
        ItemDef& item = builder->items.create(CORE_STRUCT_AIR+".item");
        item.iconType = ItemIconType::BLOCK;
        item.icon = CORE_STRUCT_AIR;
        item.placingBlock = CORE_STRUCT_AIR;
        item.caption = block.caption;
    }
}