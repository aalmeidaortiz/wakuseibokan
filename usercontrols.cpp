/*
 * usercontrols.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: faturita
 */

#include <GLUT/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <iostream>

#include "math/yamathutil.h"

#include "container.h"

#include "camera.h"
#include "usercontrols.h"
#include "units/Vehicle.h"

Camera Camera;

extern dWorldID world;
extern dSpaceID space;

Controller controller;

extern std::vector<std::string> messages;

extern std::vector<Vehicle*> controlables;

extern container<Vehicle*> vehicles;

// Mouse offset for camera zoom in and out.
int _xoffset = 0;
int _yoffset = 0;

// Right, left or middle button pressed.
int buttonState;

void processMouseEntry(int state) {
    /**if (state == GLUT_LEFT)
        _angle = 0.0;
    else
        _angle = 1.0;**/
    
}


void processMouse(int button, int state, int x, int y) {


    //int specialKey = glutGetModifiers();
    // if both a mouse button, and the ALT key, are pressed  then
    if ((state == GLUT_DOWN)) {

        _xoffset = _yoffset = 0;

        if (GLUT_RIGHT_BUTTON == button)
        {
        	//buttonState = 0;
        }
        // set the color to pure red for the left button
        if (button == GLUT_LEFT_BUTTON) {
			//buttonState = 1;
        }
        // set the color to pure green for the middle button
        else if (button == GLUT_MIDDLE_BUTTON) {

        }
        // set the color to pure blue for the right button
        else {

        }
    }
}



void processMouseActiveMotion(int x, int y) {
    //int specialKey = glutGetModifiers();
    // the ALT key was used in the previous function
    //if (specialKey != GLUT_ACTIVE_ALT) {

    //printf ("X value:%d\n", x);

    if (_xoffset ==0 ) _xoffset = x;

    if (_yoffset == 0 ) _yoffset = y;

    //if (buttonState == 1)
    {
    	Camera.xAngle += ( (x-_xoffset) * 0.005);

    	Camera.yAngle += ( (y - _yoffset ) * 0.005) ;
    } //else if (buttonState == 0)
    {
    	//Vec3f forward = Camera.getForward();
    	//Vec3f pos = Camera.getPos();
    	//Camera.setPos(  ( (y - _yoffset ) * 0.05)*forward+pos   );
    	
        
        //Camera.dx += ( (y - _yoffset ) * 0.05) ;
    }
    //}
}




// Movement of the mouse alone.
void processMousePassiveMotion(int x, int y) {
    //int specialKey = glutGetModifiers();
    // User must press the SHIFT key to change the
    // rotation in the X axis
    //if (specialKey != GLUT_ACTIVE_SHIFT) {

        // setting the angle to be relative to the mouse
        // position inside the window
   /**     if (x < 0)
            _angle = 0.0;
        else if (x > 400)
            _angle = 180.0;
        else
            _angle = 180.0 * ((float) x)/400;
   **/ //}
    //processMouseActiveMotion(x,y);
}

bool pp=false;

void handleKeypress(unsigned char key, int x, int y) {
    if (controller.isTeletype())
    {
        if (key == 13)
        {
            // Analyze commands.

            size_t n = controller.str.find("control");

            if (n != std::string::npos)
            {
                // @FIXME input data should be verified.
                const char *content = controller.str.substr(8).c_str();

                controller.controlling = atoi(content);
            } else

            if (strcmp(controller.str.c_str(),"map")==0)
                controller.view = 2;
            else {
                // Send message to message board
                messages.insert(messages.begin(),controller.str);

                if (messages.size()>5)
                    messages.pop_back();

            }
            controller.teletype = false;
            controller.str.clear();


        } else {
            controller.str += key;
        }
    } else

    switch (key) {
		case 27: //Escape key
            controller.interrupt();
        case '+':Camera.dx+=0.1;break;
        case '-':Camera.dx-=0.1;break;
        case 32 :Camera.dx=0.00001; controller.registers.thrust = 0;break;
        case '^':pp = !(pp);break;
        case 'a':case 'k':controller.registers.roll-=1.0f;break;
        case 'd':case 'l':controller.registers.roll+=1.0f;break;
        case 'w':controller.registers.pitch-=1.0f;break;
        case 's':controller.registers.pitch+=1.0f;break;
        case 'z':controller.registers.yaw-=1.0f;break;
        case 'c':controller.registers.yaw+=1.0f;break;
        case 'v':controller.registers.precesion-=1.0f;break;
        case 'b':controller.registers.precesion+=1.0f;break;
        case '9':controller.registers.thrust+=20.0f;break;
        case 'p':controller.pause = !controller.pause;break;
        case 'r':controller.registers.thrust-=0.05;break;
        case 'R':controller.registers.thrust-=10.00;break;
        case 'f':controller.registers.thrust+=0.05;break;
        case 'F':controller.registers.thrust+=10.00;break;
        case 'q':controller.reset();break;
        case 'Q':controller.registers.thrust = 0.0;break;
        case 'j':controlables[controller.controlling-1]->setEnableAI();break;
        case 'J':controlables[controller.controlling-1]->setDisableAI();break;
        case '0':
        	controller.controlling = 0;
            controller.reset();
            Camera.reset();
        break;
        case '1':case '2':case '3': case '4': case '5': case '6':case '7':
            controlables[controller.controlling-1]->setControlRegisters(controller.registers);
            controller.controlling = (int)(key-48);
            //controller.reset();
            controller.registers = controlables[controller.controlling-1]->getControlRegisters();
        	//spd = vehicles.getThrottle();
        break;
        case 'i':
            {
            int param = 0;
            std::cout << "Param:" << std::endl; std::cin >> param;
            std::cout << "Value:" << std::endl; std::cin >> controller.param[param] ;
            }
        break;
        case '!':controller.view = 1;break;
        case '@':controller.view = 3;break;
        case '~':Camera.control = 0;break;
        case 'S':gltWriteTGA("file.tga");break;
        case 't':controller.teletype = true;break;
        case 'm':
            {
            Vehicle *manta = (controlables[controller.controlling-1])->spawn(world,space,2);
            if (manta != NULL)
                vehicles.push_back(manta);
                controlables.push_back(manta);
            }
        break;
        case 'h':
            Vehicle *action = (controlables[controller.controlling-1])->fire(world,space);
            //int *idx = new int();
            //*idx = vehicles.push_back(action);
            //dBodySetData( action->getBodyID(), (void*)idx);
            if (action != NULL)
            {
                vehicles.push_back(action);
            }
        break;
	}
}

void handleSpecKeypress(int key, int x, int y)
{
    //specialKey = glutGetModifiers();

    switch (key) {
        case GLUT_KEY_LEFT :
            ;break;
        case GLUT_KEY_RIGHT :
            ;break;
        case GLUT_KEY_UP :
            ;break;
        case GLUT_KEY_DOWN :
            ;break;
    }

}


// ------------------------
typedef struct
{
	unsigned char idLength;
	unsigned char colorMapType;
	unsigned char imageTypeCode;
	unsigned char colorMapSpec[5];
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char imageDesc;
} tgaheader;

typedef struct
{
    GLubyte header[6];          // Holds The First 6 Useful Bytes Of The File
    GLuint bytesPerPixel;           // Number Of BYTES Per Pixel (3 Or 4)
    GLuint imageSize;           // Amount Of Memory Needed To Hold The Image
    GLuint type;                // The Type Of Image, GL_RGB Or GL_RGBA
    GLuint height;              // Height Of Image
    GLuint width;               // Width Of Image
    GLuint bits;             // Number Of BITS Per Pixel (24 Or 32)
} TGAHEADER;

GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight,
		GLint *iComponents, GLenum *eFormat)
{
	FILE		*pFile;
	tgaheader	tgaHeader;
	unsigned long lImageSize;
	short		sDepth;
	GLbyte		*pBits = NULL;

	*iWidth = 0;
	*iHeight = 0;
	*eFormat = GL_RGB;
	*iComponents = GL_RGB;


	// Open the file
	pFile = fopen(szFileName, "rb");
	if (pFile == NULL)
		return NULL;

	fread(&tgaHeader, 18/* sizeof(TGAHEADER) */,1,pFile);

	*iWidth = tgaHeader.width;
	*iHeight = tgaHeader.height;
	sDepth = tgaHeader.bpp / 8;

	if (tgaHeader.bpp != 8 && tgaHeader.bpp != 24 && tgaHeader.bpp != 32)
		return NULL;

	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	pBits = (GLbyte*) malloc(lImageSize * sizeof(GLbyte));

	if (pBits == NULL)
		return NULL;

	if (fread(pBits, lImageSize, 1, pFile)!=1)
	{
		free(pBits);
		return NULL;

	}

	switch (sDepth)
	{
	case 3:
		*eFormat = GL_BGR;
		*iComponents = GL_RGB;
		break;
	case 4:
		*eFormat = GL_BGRA;
		*iComponents = GL_RGBA;
		break;
	case 1:
		*eFormat = GL_LUMINANCE;
		*iComponents = GL_LUMINANCE;
		break;
	default:
		break;
	}

	fclose(pFile);

	return pBits;
}




GLint gltWriteTGA(const char *szFileName)
{
	FILE *pFile;
	tgaheader tgaHeader;
	unsigned long lImageSize;
	GLbyte *pBits = NULL;
	GLint iViewport[4];
	GLint lastBuffer;

	//Get the viewport dimension
	glGetIntegerv(GL_VIEWPORT, iViewport);

	lImageSize = iViewport[2] * 3 * iViewport[3];

	pBits = (GLbyte*)malloc(lImageSize);

	if (pBits == NULL)
		return 0;

	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS,0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	glGetIntegerv(GL_READ_BUFFER, &lastBuffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,iViewport[2], iViewport[3], GL_BGR, GL_UNSIGNED_BYTE, pBits);
	glReadBuffer(lastBuffer);

	tgaHeader.imageTypeCode = 2;
	tgaHeader.bpp = 24;
	tgaHeader.width = iViewport[2];
	tgaHeader.height = iViewport[3];

	pFile = fopen(szFileName, "wb");
	if (pFile == NULL)
	{
		free(pBits);
		return 0;

	}

	fwrite(&tgaHeader, sizeof(tgaheader), 1, pFile);

	fwrite(pBits, lImageSize, 1, pFile);

	free(pBits);
	fclose(pFile);

	return 1;
}



