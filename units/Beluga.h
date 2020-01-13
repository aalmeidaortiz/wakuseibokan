#ifndef BELUGA_H
#define BELUGA_H

#include "Balaenidae.h"

class Beluga : public Balaenidae
{
public:
    Beluga(int faction);

    void drawModel(float yRot, float xRot, float x, float y, float z);
    void init();
    void embody(dWorldID world, dSpaceID space);
    void embody(dBodyID myBodySelf);

    void doControl();
};

#endif // BELUGA_H
