//
//  BoxVehicle.h
//  mycarrier
//
//  Created by Rodrigo Ramele on 24/05/14.
//  Copyright (c) 2014 Baufest. All rights reserved.
//

#ifndef __mycarrier__BoxVehicle__
#define __mycarrier__BoxVehicle__

#include <iostream>

#include "Vehicle.h"

class BoxVehicle : public Vehicle
{
    GLuint _textureBox;
    float boxRotatingAngle;
    dGeomID geom;
    bool tweakOde = true;
    bool repelling = false;
    
public:
	void  init();
    int   getType();
	void  drawModel(float yRot, float xRot, float x, float y, float z);
    
	void drawModel();
    
	void  drawDirectModel();
	void doMaterial();
	void  doDynamics(dBodyID);
    void  doDynamics();
    void  embody(dBodyID);
    void  embody(dWorldID world, dSpaceID space);
    void  doControl(Controller);
    dBodyID  getBodyID();
    void  stop();
};


#endif /* defined(__mycarrier__BoxVehicle__) */
