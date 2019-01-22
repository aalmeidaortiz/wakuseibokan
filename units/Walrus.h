/*
 * Walrus.h
 *
 *  Created on: Jan 16, 2011
 *      Author: faturita
 */

#ifndef WALRUS_H_
#define WALRUS_H_

#include "Vehicle.h"
#include "../terrain/Terrain.h"

class Walrus : public Vehicle
{
private:
    float height;
    float width;
    float length;

protected:
    int number;

    BoxIsland *island;

public:
    static const int INSHORING = 3;
    static const int OFFSHORING = 2;
    static const int ROLLING = 1;
    static const int SAILING = 0;

	void virtual init();
    int  virtual getType();
	void virtual drawModel(float yRot, float xRot, float x, float y, float z);
	void virtual drawModel();
	void virtual drawDirectModel();
	void doMaterial();
	void virtual doDynamics(dBodyID);
    void doDynamics();

    void doControl(Controller controller);
    void doControl();
    void doControl(struct controlregister cons);
    
    void embody(dWorldID world, dSpaceID space);
    void embody(dBodyID myBodySelf);

    Vehicle* fire(dWorldID world, dSpaceID space);
    int getNumber() const;
    void setNumber(int value);

    BoxIsland *getIsland() const;
    void setIsland(BoxIsland *value);

    void virtual setStatus(int status);
};

#endif /* WALRUS_H_ */
