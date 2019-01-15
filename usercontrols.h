#ifndef USERCONTROLS_H
#define USERCONTROLS_H

#include <assert.h>
#include <string>
#include <iostream>

struct controlregister
{
    // R+,F-
    float thrust=0;

    // ModAngleX
    float roll=0;

    // ModAngleY
    float pitch=0;

    // ModAngleZ
    float yaw=0;

    // ModAngleP
    float precesion=0;
};


class Controller
{
public:
    
	// Device ID to be controller.
    int controlling=0;
    
	// Index to Observable interfaces.
	int camera;

    // Which view mode is currently active.
    int view=1;
    
    struct controlregister registers;

    // Custom parameters that can be entered from controller.
    float param[10];
    
	bool pause=false;
    
	bool pp;
    
    bool finish=false;

    bool teletype=false;

    std::string str;
    
    void reset()
    {
        registers.roll=registers.pitch=registers.precesion=registers.yaw=0;
        registers.thrust=0;
    };
    
    void interrupt()
    {
        finish=true;
    }
    
    bool isInterrupted()
    {
        return finish;
    };

    bool isTeletype()
    {
        return teletype;
    };
};


GLint gltWriteTGA(const char *szFileName);

void processMouseEntry(int state) ;

void processMouse(int button, int state, int x, int y) ;

void processMouseActiveMotion(int x, int y) ;

void processMousePassiveMotion(int x, int y) ;

void handleKeypress(unsigned char key, int x, int y) ;

void handleSpecKeypress(int key, int x, int y);

GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight,
		GLint *iComponents, GLenum *eFormat);

GLint gltWriteTGA(const char *szFileName);


#endif /* USERCONTROLS_H */

