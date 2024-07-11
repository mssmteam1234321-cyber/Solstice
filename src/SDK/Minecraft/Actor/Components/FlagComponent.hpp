//
// Created by vastrakai on 7/10/2024.
//

#pragma once


template<typename T>
class FlagComponent {};

struct VerticalCollisionFlag {};
struct HorizontalCollisionFlag {};
struct MovingFlag {};
struct WasOnGroundFlag {};
struct JumpFromGroundRequestFlag {};
struct LocalPlayerJumpRequestFlag {};
struct LocalPlayerComponentFlag {};
struct ActorFlag {};
struct PlayerComponentFlag {};
struct InWaterFlag {};
struct InLavaFlag {};
struct PigFlag {};
struct ActorIsImmobileFlag {};
struct DiscardFrictionFlag {};
struct CanStandOnSnowFlag {};

// What the fuck
struct OnGroundFlagComponent{};
