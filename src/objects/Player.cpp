#include "Player.hpp"

#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "content/ContentReport.hpp"
#include "items/Inventory.hpp"
#include "Entities.hpp"
#include "rigging.hpp"
#include "physics/Hitbox.hpp"
#include "physics/PhysicsSolver.hpp"
#include "voxels/Chunks.hpp"
#include "window/Camera.hpp"
#include "window/Events.hpp"
#include "world/Level.hpp"
#include "data/dv_util.hpp"

constexpr float CROUCH_SPEED_MUL = 0.35f;
constexpr float RUN_SPEED_MUL = 1.5f;
constexpr float PLAYER_GROUND_DAMPING = 10.0f;
constexpr float PLAYER_AIR_DAMPING = 7.0f;
constexpr float FLIGHT_SPEED_MUL = 4.0f;
constexpr float CHEAT_SPEED_MUL = 5.0f;
constexpr float JUMP_FORCE = 8.0f;
constexpr int SPAWN_ATTEMPTS_PER_UPDATE = 64;

Player::Player(
    Level* level,
    glm::vec3 position,
    float speed,
    std::shared_ptr<Inventory> inv,
    entityid_t eid
)
    : level(level),
      speed(speed),
      chosenSlot(0),
      position(position),
      inventory(std::move(inv)),
      eid(eid),
      fpCamera(level->getCamera("core:first-person")),
      spCamera(level->getCamera("core:third-person-front")),
      tpCamera(level->getCamera("core:third-person-back")),
      currentCamera(fpCamera) {
    fpCamera->setFov(glm::radians(90.0f));
    spCamera->setFov(glm::radians(90.0f));
    tpCamera->setFov(glm::radians(90.0f));
}

Player::~Player() {
}

void Player::updateEntity() {
    if (eid == 0) {
        auto& def = level->content->entities.require("base:player");
        eid = level->entities->spawn(def, getPosition());
    } else if (auto entity = level->entities->get(eid)) {
        position = entity->getTransform().pos;
    } else {
        // TODO: check if chunk loaded
    }
}

Hitbox* Player::getHitbox() {
    if (auto entity = level->entities->get(eid)) {
        return &entity->getRigidbody().hitbox;
    }
    return nullptr;
}

void Player::updateInput(PlayerInput& input, float delta) {
    auto hitbox = getHitbox();
    if (hitbox == nullptr) {
        return;
    }
    bool crouch = input.shift && hitbox->grounded && !input.sprint;
    float speed = this->speed;
    if (flight) {
        speed *= FLIGHT_SPEED_MUL;
    }
    if (input.cheat) {
        speed *= CHEAT_SPEED_MUL;
    }

    hitbox->crouching = crouch;
    if (crouch) {
        speed *= CROUCH_SPEED_MUL;
    } else if (input.sprint) {
        speed *= RUN_SPEED_MUL;
    }

    glm::vec3 dir(0, 0, 0);
    if (input.moveForward) {
        dir += fpCamera->dir;
    }
    if (input.moveBack) {
        dir -= fpCamera->dir;
    }
    if (input.moveRight) {
        dir += fpCamera->right;
    }
    if (input.moveLeft) {
        dir -= fpCamera->right;
    }
    if (glm::length(dir) > 0.0f) {
        dir = glm::normalize(dir);
        hitbox->velocity += dir * speed * delta * 9.0f;
    }

    hitbox->linearDamping = PLAYER_GROUND_DAMPING;
    hitbox->verticalDamping = flight;
    hitbox->gravityScale = flight ? 0.0f : 1.0f;
    if (flight) {
        hitbox->linearDamping = PLAYER_AIR_DAMPING;
        if (input.jump) {
            hitbox->velocity.y += speed * delta * 9;
        }
        if (input.shift) {
            hitbox->velocity.y -= speed * delta * 9;
        }
    }
    if (!hitbox->grounded) {
        hitbox->linearDamping = PLAYER_AIR_DAMPING;
    }

    if (input.jump && hitbox->grounded) {
        hitbox->velocity.y = JUMP_FORCE;
    }

    if ((input.flight && !noclip) || (input.noclip && flight == noclip)) {
        flight = !flight;
        if (flight) {
            hitbox->velocity.y += 1.0f;
        }
    }
    hitbox->type = noclip ? BodyType::KINEMATIC : BodyType::DYNAMIC;
    if (input.noclip) {
        noclip = !noclip;
    }
    input.noclip = false;
    input.flight = false;
}

void Player::updateSelectedEntity() {
    selectedEid = selection.entity;
}

void Player::postUpdate() {
    auto entity = level->entities->get(eid);
    if (!entity.has_value()) {
        return;
    }
    auto& hitbox = entity->getRigidbody().hitbox;
    position = hitbox.position;

    if (flight && hitbox.grounded && !noclip) {
        flight = false;
    }
    if (spawnpoint.y <= 0.1) {
        for (int i = 0; i < SPAWN_ATTEMPTS_PER_UPDATE; i++) {
            attemptToFindSpawnpoint();
        }
    }

    auto& skeleton = entity->getSkeleton();

    skeleton.visible = currentCamera != fpCamera;

    auto body = skeleton.config->find("body");
    auto head = skeleton.config->find("head");

    if (body) {
        skeleton.pose.matrices[body->getIndex()] = glm::rotate(
            glm::mat4(1.0f), glm::radians(cam.x), glm::vec3(0, 1, 0)
        );
    }
    if (head) {
        skeleton.pose.matrices[head->getIndex()] = glm::rotate(
            glm::mat4(1.0f), glm::radians(cam.y), glm::vec3(1, 0, 0)
        );
    }
}

void Player::teleport(glm::vec3 position) {
    this->position = position;

    if (auto entity = level->entities->get(eid)) {
        entity->getRigidbody().hitbox.position = position;
        entity->getTransform().setPos(position);
    }
}

void Player::attemptToFindSpawnpoint() {
    glm::vec3 newpos(
        position.x + (rand() % 200 - 100),
        rand() % 80 + 100,
        position.z + (rand() % 200 - 100)
    );
    while (newpos.y > 0 &&
           !level->chunks->isObstacleBlock(newpos.x, newpos.y - 2, newpos.z)) {
        newpos.y--;
    }

    voxel* headvox = level->chunks->get(newpos.x, newpos.y + 1, newpos.z);
    if (level->chunks->isObstacleBlock(newpos.x, newpos.y, newpos.z) ||
        headvox == nullptr || headvox->id != 0) {
        return;
    }
    spawnpoint = newpos + glm::vec3(0.5f, 0.0f, 0.5f);
    teleport(spawnpoint);
}

void Player::setChosenSlot(int index) {
    chosenSlot = index;
}

int Player::getChosenSlot() const {
    return chosenSlot;
}

float Player::getSpeed() const {
    return speed;
}

bool Player::isFlight() const {
    return flight;
}

void Player::setFlight(bool flag) {
    this->flight = flag;
}

bool Player::isNoclip() const {
    return noclip;
}

void Player::setNoclip(bool flag) {
    this->noclip = flag;
}

entityid_t Player::getEntity() const {
    return eid;
}

void Player::setEntity(entityid_t eid) {
    this->eid = eid;
}

entityid_t Player::getSelectedEntity() const {
    return selectedEid;
}

const std::shared_ptr<Inventory>& Player::getInventory() const {
    return inventory;
}

void Player::setSpawnPoint(glm::vec3 spawnpoint) {
    this->spawnpoint = spawnpoint;
}

glm::vec3 Player::getSpawnPoint() const {
    return spawnpoint;
}

dv::value Player::serialize() const {
    auto root = dv::object();

    root["position"] = dv::to_value(position);
    root["rotation"] = dv::to_value(cam);
    root["spawnpoint"] = dv::to_value(spawnpoint);

    root["flight"] = flight;
    root["noclip"] = noclip;
    root["chosen-slot"] = chosenSlot;
    root["entity"] = eid;
    root["inventory"] = inventory->serialize();
    auto found =
        std::find(level->cameras.begin(), level->cameras.end(), currentCamera);
    if (found != level->cameras.end()) {
        root["camera"] = level->content->getIndices(ResourceType::CAMERA)
                .getName(found - level->cameras.begin());
    }
    return root;
}

void Player::deserialize(const dv::value& src) {
    const auto& posarr = src["position"];

    dv::get_vec(posarr, position);
    fpCamera->position = position;

    const auto& rotarr = src["rotation"];
    dv::get_vec(rotarr, cam);

    const auto& sparr = src["spawnpoint"];
    setSpawnPoint(glm::vec3(
        sparr[0].asNumber(), sparr[1].asNumber(), sparr[2].asNumber()));

    flight = src["flight"].asBoolean();
    noclip = src["noclip"].asBoolean();
    setChosenSlot(src["chosen-slot"].asInteger());
    eid = src["entity"].asNumber();

    if (src.has("inventory")) {
        getInventory()->deserialize(src["inventory"]);
    }

    if (src.has("camera")) {
        std::string name = src["camera"].asString();
        if (auto camera = level->getCamera(name)) {
            currentCamera = camera;
        }
    }
}

void Player::convert(dv::value& data, const ContentReport* report) {
    if (data.has("players")) {
        auto& players = data["players"];
        for (uint i = 0; i < players.size(); i++) {
            auto& playerData = players[i];
            if (playerData.has("inventory")) {
                Inventory::convert(playerData["inventory"], report);
            }
        }

    } else if (data.has("inventory")){
        Inventory::convert(data["inventory"], report);
    }
}
