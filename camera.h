/*
 * camera.h
 *
 *  Created on: Feb 5, 2012
 *      Author: faturita
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <GLUT/glut.h>
#include "carrier/yamathutil.h"
#include "observable.h"



class Camera : Observable
{
public:
	Camera();
	void getViewPort(Vec3f &Up, Vec3f &position, Vec3f &forward);
	Vec3f getForward();
	Vec3f pos;
	Vec3f getPos();
	void setPos(Vec3f newpos);
	float dx;
	float dy;
	float dz;
	float yAngle;
	float xAngle;

	int control;
    
    void lookAtFrom(Vec3f up, Vec3f pos, Vec3f forward);
    
    void lookAtFrom(Vec3f pos, Vec3f forward);
} ;







#endif /* CAMERA_H_ */
