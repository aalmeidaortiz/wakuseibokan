#ifndef MISSILE_H
#define MISSILE_H

#include "Gunshot.h"


class Missile : public Gunshot
{
protected:
    float range;

    float elevator = 0;
    float rudder = 0;

    float et1=0,et2=0, et3=0;
    float ett1=0, ett2=0, ett3=0;
    float r1=0, r2=0, r3=0;
    float rt1=0, rt2=0, rt3=0;
    float midpointpitch = -5;
public:
    Missile();
    ~Missile();
    void init();
    void drawModel(float yRot, float xRot, float x, float y, float z);
    void drawModel();
    void doMaterial();

    void doDynamics(dBodyID body);

    void embody(dWorldID world, dSpaceID space);
    void embody(dBodyID myBodySelf);

    int getType();

    void doControlControl2(Vec3f target, float thrust);
    void doControl();

    void doControl(Controller controller);

    void doControl(struct controlregister conts);
};

#endif // MISSILE_H
