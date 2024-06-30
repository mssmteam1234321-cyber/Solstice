#pragma once
//
// Created by vastrakai on 6/28/2024.
//



#include <vector>

class ActorUtils {
public:
    static std::vector<class Actor*> getActorList(bool playerOnly = true);
};