//
// Created by vastrakai on 6/28/2024.
//

#pragma once

#include "Packet.hpp"
#include <bitset>
#include <memory>
#include <glm/glm.hpp>
#include <vector>

enum class InputMode : int {
    Undefined        = 0x0,
    Mouse            = 0x1,
    Touch            = 0x2,
    GamePad          = 0x3,
    MotionController = 0x4,
};

enum class ClientPlayMode : int {
    Normal              = 0x0,
    Teaser              = 0x1,
    Screen              = 0x2,
    Viewer              = 0x3,
    Reality             = 0x4,
    Placement           = 0x5,
    LivingRoom          = 0x6,
    ExitLevel           = 0x7,
    ExitLevelLivingRoom = 0x8,
    NumModes            = 0x9,
};

enum class NewInteractionModel : int {
    Touch     = 0x0,
    Crosshair = 0x1,
    Classic   = 0x2,
};

class PlayerBlockActions {
public:
    std::vector<struct PlayerBlockActionData> mActions;
};


enum class PlayerActionType {
    START_DESTROY_BLOCK,
    ABORT_DESTROY_BLOCK,
    STOP_DESTROY_BLOCK,
    GET_UPDATED_BLOCK,
    DROP_ITEM,
    START_SLEEPING,
    STOP_SLEEPING,
    RESPAWN,
    START_JUMP,
    START_SPRINTING,
    STOP_SPRINTING,
    START_SNEAKING,
    STOP_SNEAKING,
    CREATIVE_DESTROY_BLOCK,
    CHANGE_DIMENSION_ACK,
    START_GLIDING,
    STOP_GLIDING,
    DENY_DESTROY_BLOCK,
    CRACK_BLOCK,
    CHANGE_SKIN,
    UPDATED_ENCHANTING_SEED,
    START_SWIMMING,
    STOP_SWIMMING,
    START_SPIN_ATTACK,
    STOP_SPIN_ATTACK,
    INTERACT_WITH_BLOCK,
    PREDICT_DESTROY_BLOCK,
    CONTINUE_DESTROY_BLOCK,
    START_ITEM_USE_ON,
    STOP_ITEM_USE_ON,
    HANDLED_TELEPORT,
    MISSED_SWING,
    START_CRAWLING,
    STOP_CRAWLING,
    START_FLYING,
    STOP_FLYING
};


class PlayerBlockActionData {
public:
    PlayerActionType mAction;
    glm::i32vec3 mPos;
    int mFace;
};


enum class AuthInputAction : uint64_t {
    NONE = 0,
    ASCEND = 1ULL << 0,
    DESCEND = 1ULL << 1,
    NORTH_JUMP = 1ULL << 2,
    JUMP_DOWN = 1ULL << 3,
    SPRINT_DOWN = 1ULL << 4,
    CHANGE_HEIGHT = 1ULL << 5,
    JUMPING = 1ULL << 6,
    AUTO_JUMPING_IN_WATER = 1ULL << 7,
    SNEAKING = 1ULL << 8,
    SNEAK_DOWN = 1ULL << 9,
    UP = 1ULL << 10,
    DOWN = 1ULL << 11,
    LEFT = 1ULL << 12,
    RIGHT = 1ULL << 13,
    UP_LEFT = 1ULL << 14,
    UP_RIGHT = 1ULL << 15,
    WANT_UP = 1ULL << 16,
    WANT_DOWN = 1ULL << 17,
    WANT_DOWN_SLOW = 1ULL << 18,
    WANT_UP_SLOW = 1ULL << 19,
    SPRINTING = 1ULL << 20,
    ASCEND_SCAFFOLDING = 1ULL << 21,
    DESCEND_SCAFFOLDING = 1ULL << 22,
    SNEAK_TOGGLE_DOWN = 1ULL << 23,
    PERSIST_SNEAK = 1ULL << 24,
    START_SPRINTING = 1ULL << 25,
    STOP_SPRINTING = 1ULL << 26,
    START_SNEAKING = 1ULL << 27,
    STOP_SNEAKING = 1ULL << 28,
    START_SWIMMING = 1ULL << 29,
    STOP_SWIMMING = 1ULL << 30,
    START_JUMPING = 1ULL << 31,
    START_GLIDING = 1ULL << 32,
    STOP_GLIDING = 1ULL << 33,
    PERFORM_ITEM_INTERACTION = 1ULL << 34,
    PERFORM_BLOCK_ACTIONS = 1ULL << 35,
    PERFORM_ITEM_STACK_REQUEST = 1ULL << 36,
    HANDLE_TELEPORT = 1ULL << 37,
    EMOTING = 1ULL << 38,
    MISSED_SWING = 1ULL << 39,
    START_CRAWLING = 1ULL << 40,
    STOP_CRAWLING = 1ULL << 41,
};

inline AuthInputAction operator|(AuthInputAction a, AuthInputAction b) {
    return static_cast<AuthInputAction>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
}

inline AuthInputAction operator&(AuthInputAction a, AuthInputAction b) {
    return static_cast<AuthInputAction>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
}

inline AuthInputAction operator^(AuthInputAction a, AuthInputAction b) {
    return static_cast<AuthInputAction>(static_cast<uint64_t>(a) ^ static_cast<uint64_t>(b));
}

inline AuthInputAction operator~(AuthInputAction a) {
    return static_cast<AuthInputAction>(~static_cast<uint64_t>(a));
}

inline void operator|=(AuthInputAction& a, AuthInputAction b) {
    a = a | b;
}

inline void operator&=(AuthInputAction& a, AuthInputAction b) {
    a = a & b;
}

inline void operator^=(AuthInputAction& a, AuthInputAction b) {
    a = a ^ b;
}

inline bool operator!(AuthInputAction a) {
    return static_cast<uint64_t>(a) == 0;
}

class PlayerAuthInputPacket : public Packet {
public:
    // PlayerAuthInputPacket inner types define
    enum class InputData : int {
        Ascend                     = 0x0,
        Descend                    = 0x1,
        NorthJump_Deprecated       = 0x2,
        JumpDown                   = 0x3,
        SprintDown                 = 0x4,
        ChangeHeight               = 0x5,
        Jumping                    = 0x6,
        AutoJumpingInWater         = 0x7,
        Sneaking                   = 0x8,
        SneakDown                  = 0x9,
        Up                         = 0xA,
        Down                       = 0xB,
        Left                       = 0xC,
        Right                      = 0xD,
        UpLeft                     = 0xE,
        UpRight                    = 0xF,
        WantUp                     = 0x10,
        WantDown                   = 0x11,
        WantDownSlow               = 0x12,
        WantUpSlow                 = 0x13,
        Sprinting                  = 0x14,
        AscendBlock                = 0x15,
        DescendBlock               = 0x16,
        SneakToggleDown            = 0x17,
        PersistSneak               = 0x18,
        StartSprinting             = 0x19,
        StopSprinting              = 0x1A,
        StartSneaking              = 0x1B,
        StopSneaking               = 0x1C,
        StartSwimming              = 0x1D,
        StopSwimming               = 0x1E,
        StartJumping               = 0x1F,
        StartGliding               = 0x20,
        StopGliding                = 0x21,
        PerformItemInteraction     = 0x22,
        PerformBlockActions        = 0x23,
        PerformItemStackRequest    = 0x24,
        HandledTeleport            = 0x25,
        Emoting                    = 0x26,
        MissedSwing                = 0x27,
        StartCrawling              = 0x28,
        StopCrawling               = 0x29,
        StartFlying                = 0x2A,
        StopFlying                 = 0x2B,
        ReceivedServerData         = 0x2C,
        InClientPredictedInVehicle = 0x2D,
        PaddlingLeft               = 0x2E,
        PaddlingRight              = 0x2F,
        BlockBreakingDelayEnabled  = 0x30,
        Input_Num                  = 0x31,
    };

public:
    static const PacketID ID = PacketID::PlayerAuthInput;

    glm::vec2                                                           mRot;
    glm::vec3                                                           mPos;
    float                                                               mYHeadRot;
    glm::vec3                                                           mPosDelta;
    glm::vec2                                                           mAnalogMoveVector;
    glm::vec2                                                           mVehicleRotation;
    glm::vec2                                                           mMove;
    glm::vec3                                                           mGazeDir;
    AuthInputAction                                                     mInputData;
    InputMode                                                           mInputMode;
    ClientPlayMode                                                      mPlayMode;
    NewInteractionModel                                                 mNewInteractionModel;
    uint64_t                                                            mClientTick;
    std::unique_ptr<class PackedItemUseLegacyInventoryTransaction>      mItemUseTransaction;
    std::unique_ptr<class ItemStackRequestData>                         mItemStackRequest;
    PlayerBlockActions                                                  mPlayerBlockActions;
    uint64_t                                                            mPredictedVehicle;
};

