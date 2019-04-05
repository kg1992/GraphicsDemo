/*
    KnightPunchingScene.h

    Author : Lee Kyunggeun(kyunggeun1992@gmail.com)

    KnightPunchingScene class definition.
    The scene spawns a ground and a knight who is constantly punching in the air.
*/
#ifndef KNICHT_PUNCHING_SCENE_H_
#define KNICHT_PUNCHING_SCENE_H_

#include "Scene.h"

class KnightPunchingScene : public Scene
{
public:
    KnightPunchingScene();

    void Init() override;
    void Update() override;

private:
    void AddGround();
};

#endif
