#include "engine.h"

extern  Controller controller;

/* dynamics and collision objects */


extern container<Vehicle*> entities;

extern std::vector<BoxIsland*> islands;

extern std::vector<std::string> messages;

// SYNC
Vehicle* gVehicle(dBodyID body)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *vehicle = entities[i];
        if (vehicle->getBodyID() == body)
        {
            return vehicle;
        }
    }
    return NULL;
}

Vehicle* gVehicle(dGeomID geom)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *vehicle = entities[i];
        if (vehicle->getGeom() == geom)
        {
            return vehicle;
        }
    }
    return NULL;
}

void gVehicle(Vehicle* &v1, Vehicle* &v2, dBodyID b1, dBodyID b2, Structure* &s1, Structure* &s2, dGeomID g1, dGeomID g2)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *vehicle = entities[i];
        if (vehicle->getBodyID() == b1)
        {
            v1 = vehicle;
        }
        if (vehicle->getBodyID() == b2)
        {
            v2 = vehicle;
        }
        if (vehicle->getGeom() == g1 && vehicle->getType() >= COLLISIONABLE)
        {
            s1 = (Structure*)vehicle;
        }
        if (vehicle->getGeom() == g2 && vehicle->getType() >= COLLISIONABLE)
        {
            s2 = (Structure*)vehicle;
        }

    }
}

// SYNC
bool stranded(Vehicle *carrier, Island *island)
{
    if (island && carrier && carrier->getType() == CARRIER && carrier->getStatus() != Balaenidae::OFFSHORING)
    {
        Balaenidae *b = (Balaenidae*)carrier;

        b->offshore();
        controller.reset();
        b->doControl(controller);
        b->setStatus(Balaenidae::OFFSHORING);
        b->disableAuto();  // Check the controller for the enemy aircraft.  Force field is disabled for destiny island.
        char str[256];
        sprintf(str, "Carrier has stranded on %s.", island->getName().c_str());
        messages.insert(messages.begin(), str);
    }
}

bool departed(Vehicle *walrus)
{
    if (walrus && walrus->getType() == WALRUS && walrus->getStatus() == Walrus::ROLLING)
    {
        Walrus *w = (Walrus*)walrus;

        w->setStatus(Walrus::OFFSHORING);
        BoxIsland *island = w->getIsland();
        w->setIsland(NULL);
        char str[256];
        sprintf(str, "Walrus has departed from %s.", island->getName().c_str());
        messages.insert(messages.begin(), str);
    }
    return true;
}

// SYNC
bool arrived(Vehicle *walrus, Island *island)
{
    if (island && walrus && walrus->getType() == WALRUS && walrus->getStatus() == Walrus::SAILING)
    {
        Walrus *w = (Walrus*)walrus;

        w->setStatus(Walrus::INSHORING);
        w->setIsland((BoxIsland*)island);
        char str[256];
        sprintf(str, "Walrus has arrived to %s.", island->getName().c_str());
        messages.insert(messages.begin(), str);
    }
    return true;
}

// SYNC
bool landed(Vehicle *manta, Island *island)
{
    if (manta && island && manta->getType() == MANTA)
    {
        if (manta->getStatus() == Manta::FLYING)
        {
            char str[256];
            sprintf(str, "Manta has landed on Island %s.", island->getName().c_str());
            messages.insert(messages.begin(), str);

            controller.reset();
            SimplifiedDynamicManta *s = (SimplifiedDynamicManta*)manta;
            struct controlregister c;
            c.thrust = 0.0f;
            c.pitch = 0.0f;
            s->setControlRegisters(c);
            s->setThrottle(0.0f);
            s->setStatus(Manta::LANDED);
        }
    }
    return true;
}

// SYNC
bool isMineFire(Vehicle* vehicle, Gunshot *g)
{
    return (g->getOrigin() == vehicle->getBodyID());
}

// SYNC
bool rayHit(Vehicle *vehicle, LaserRay *l)
{
    vehicle->damage(2);
    return true;
}



// SYNC
bool hit(Vehicle *vehicle, Gunshot *g)
{
    /**   Sending too many messages hurts FPS
    if (vehicle && vehicle->getType() == CARRIER)
    {
        messages.insert(messages.begin(), std::string("Carrier is under fire!"));
    }
    if (vehicle && vehicle->getType() == MANTA)
    {
        messages.insert(messages.begin(), std::string("Manta is under fire!"));
    }
    if (vehicle && vehicle->getType() == WALRUS)
    {
        messages.insert(messages.begin(), std::string("Walrus is under fire!"));
    }
    **/

    // @NOTE: Gunshot are a source of inestability.  I added some extra checks and to avoid tweakings I hide bullets after hitting targets.
    g->setVisible(false);

    // Dont hit me
    if (g->getOrigin() != vehicle->getBodyID())
    {
        if (vehicle->getType() == MANTA)
        {
            vehicle->damage(50);
        } else
            vehicle->damage(g->getDamage());
        return false;
    }
    return true;
}

bool hit(Structure* structure, Gunshot *g)
{
    static Island *b = NULL;

    if (structure)
    {
        if (b && structure && structure->island && b != structure->island)
        {
            char str[256];
            sprintf(str, "Island %s under attack!", structure->island->getName().c_str());
            messages.insert(messages.begin(), str);
            b = structure->island;
        }

        //bullethit();
        g->setVisible(false);
        structure->damage(g->getDamage());
    }
}

// SYNC
bool releasecontrol(Vehicle* vehicle)
{
    if (vehicle && vehicle->getType() == MANTA)
    {
        if (vehicle->getStatus() != Manta::ON_DECK && vehicle->getStatus() != Manta::TACKINGOFF)
        {
            controller.reset();

            SimplifiedDynamicManta *s = (SimplifiedDynamicManta*)vehicle;
            struct controlregister c;
            c.thrust = 0.0f;
            c.pitch = 0.0f;
            s->setControlRegisters(c);
            s->setThrottle(0.0f);
            s->setStatus(Manta::ON_DECK);
            s->inert = true;
            messages.insert(messages.begin(), std::string("Manta has landed on Aircraft."));
        }
    }
    return true;
}

void  releasecontrol(dBodyID body)
{
    synchronized(entities.m_mutex)
    {
        Vehicle* vehicle = gVehicle(body);
        releasecontrol(vehicle);
    }
}


// SYNC
bool  isType(Vehicle *vehicle, int type)
{
    bool result = false;
    if (vehicle)
    {
        if (vehicle->getType() == type)
        {
            result = true;
        }
    }
    return result;
}


bool  isType(dBodyID body, int type)
{
    bool result = false;
    synchronized(entities.m_mutex)
    {
        Vehicle *vehicle = gVehicle(body);
        result = isType(vehicle,type);
    }
    return result;
}


bool isType(dBodyID body, int types[], int length)
{
    bool result = false;
    synchronized(entities.m_mutex)
    {
        Vehicle *vehicle = gVehicle(body);

        for(int s=0;s<length;s++)
            if (isType(vehicle,types[s])) result=true;
    }
    return result;

}


bool  isManta(dBodyID body)
{
    return isType(body,3);
}

bool  isManta(Vehicle* vehicle)
{
    return isType(vehicle,3);
}

bool  isCarrier(dBodyID body)
{
    return isType(body, 4);
}

bool  isCarrier(Vehicle* vehicle)
{
    return isType(vehicle,4);
}

bool  isWalrus(Vehicle* vehicle)
{
    return isType(vehicle, WALRUS);
}

bool  isAction(dBodyID body)
{
    return isType(body, ACTION) || isType(body, VehicleTypes::CONTROLABLEACTION);
}

bool  isAction(Vehicle* vehicle)
{
    return isType(vehicle, ACTION) || isType(vehicle, VehicleTypes::CONTROLABLEACTION);
}

// SYNC
bool isRay(dGeomID o)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getGeom() == o && v->getType()==RAY)
        {
            return true;
        }
    }
    return false;
}

bool  isRunway(Structure* s)
{
    if (s && s->getType()==LANDINGABLE)
    {
        return true;
    }
    return false;
}

bool  isRunway(dGeomID candidate)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *vehicle = entities[i];
        if (vehicle && vehicle->getGeom() == candidate && vehicle->getType()==LANDINGABLE)
        {
            return true;
        }
    }
}

Island* getIsland(dGeomID candidate)
{
    for (int j=0;j<islands.size();j++)
    {
        if (candidate == islands[j]->getGeom())
            return islands[j];
    }
    return NULL;
}

bool  isIsland(dGeomID candidate)
{
    for (int j=0;j<islands.size();j++)
    {
        if (candidate == islands[j]->getGeom())
        {
            return true;
        }
    }
    return false;
}


// @FIXME Check the island !
CommandCenter* findCommandCenter(Island *island)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == CONTROL)
        {
            Structure *s = (Structure*)v;
            if (s->island == island)
                return (CommandCenter*)s;
        }
    }
    return NULL;
}

// SYNC
bool  groundcollisions(Vehicle *vehicle)
{
    if (vehicle)
    {
        if (vehicle->getSpeed()>10 and vehicle->getType() == MANTA)
        {
            //explosion();
            SimplifiedDynamicManta *s = (SimplifiedDynamicManta*)vehicle;
            struct controlregister c;
            c.thrust = 0.0f;
            c.pitch = 0.0f;
            s->setControlRegisters(c);
            s->setThrottle(0.0f);
            s->damage(1);
        }
    }
    return true;
}

void groundcollisions(dBodyID body)
{
    synchronized(entities.m_mutex)
    {
        Vehicle *vehicle = gVehicle(body);
        groundcollisions(vehicle);
    }
}

Manta* findMantaByNumber(size_t &pos, int number)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == MANTA && ((Manta*)v)->getNumber() == number-1)
        {
            pos = i+1;  // @FIXME Risky
            pos = entities.indexOf(i);
            return (Manta*)v;
        }
    }
    return NULL;
}

Manta* findManta(int status)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == MANTA && v->getStatus() == status)
        {
            return (Manta*)v;
        }
    }
    return NULL;
}

Walrus* findWalrusByNumber(size_t &pos, int number)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == WALRUS  && ((Walrus*)v)->getNumber() == number - 1)
        {
            pos = i+1;   // @FIXME Fix this risky numbering system.
            pos = entities.indexOf(i);
            return (Walrus*)v;
        }
    }
    return NULL;
}


Walrus* findWalrus(int faction)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == WALRUS && v->getFaction() == faction)
        {
            return (Walrus*)v;
        }
    }
    return NULL;
}

Walrus* findWalrus(int status, int faction)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == WALRUS && v->getFaction() == faction && v->getStatus() == status)
        {
            return (Walrus*)v;
        }
    }
    return NULL;
}

Vehicle* findCarrier(int faction)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v->getType() == CARRIER && v->getFaction() == faction)
        {
            return v;
        }
    }
    return NULL;
}

int findNextNumber(int type)
{
    int numbers[256];
    memset(numbers,0,sizeof(int)*256);

    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (type == MANTA && v->getType() == type)
        {
            Manta *m = (Manta*)v;
            numbers[m->getNumber()] = 1;
        }
        if (type == WALRUS && v->getType() == type)
        {
            Walrus *m = (Walrus*)v;
            numbers[m->getNumber()] = 1;
        }
    }
    for(int i=0;i<256;i++)
    {
        if (numbers[i]==0)
            return i;
    }
    assert(!"No more available numbers !!!!!");
}

Vehicle* findNearestEnemyVehicle(int friendlyfaction,int type, Vec3f l, float threshold)
{
    int nearesti = -1;
    float closest = threshold;
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        Vehicle *v=entities[i];
        if (v &&
                ( (type == -1 && v->getType() != ACTION && v->getType() != CONTROLABLEACTION && v->getType() != RAY) || (v->getType() == type) )
                && v->getFaction()!=friendlyfaction)   // Fix this.
        {
            if ((v->getPos()-l).magnitude()<closest) {
                closest = (v->getPos()-l).magnitude();
                nearesti = i;
            }
        }
    }
    if (nearesti < 0)
        return NULL;
    else
        return entities[nearesti];
}
Vehicle* findNearestEnemyVehicle(int friendlyfaction,Vec3f l, float threshold)
{
    return findNearestEnemyVehicle(friendlyfaction,-1,l,threshold);
}

BoxIsland* findNearestIsland(Vec3f Po)
{
    int nearesti = 0;
    float closest = 0;
    for(int i=0;i<islands.size();i++)
    {
        BoxIsland *b = islands[i];
        Vec3f l(b->getX(),0.0f,b->getZ());

        if ((l-Po).magnitude()<closest || closest ==0) {
            closest = (l-Po).magnitude();
            nearesti = i;
        }
    }

    return islands[nearesti];
}


BoxIsland* findNearestEmptyIsland(Vec3f Po)
{
    return findNearestIsland(Po,true,-1);
}

BoxIsland* findNearestIsland(Vec3f Po, bool empty, int friendlyfaction)
{
    return findNearestIsland(Po,empty,friendlyfaction,12000 kmf);
}

BoxIsland* findNearestIsland(Vec3f Po, bool empty, int friendlyfaction, float threshold)
{
    int nearesti = 0;
    float closest = 0;
    for(int i=0;i<islands.size();i++)
    {
        BoxIsland *b = islands[i];
        Vec3f l(b->getX(),0.0f,b->getZ());

        Structure *d = b->getCommandCenter();

        if ((!d && empty) || (d && !empty && (d->getFaction()!=friendlyfaction || friendlyfaction == -1)) )
        {
            if ( ((l-Po).magnitude()<closest || closest ==0)  && (l-Po).magnitude()<threshold) {
                closest = (l-Po).magnitude();
                nearesti = i;
            }
        }
    }

    return islands[nearesti];
}

BoxIsland* findIslandByName(std::string islandname)
{
    for (int j=0;j<islands.size();j++)
    {
        if (islandname == islands[j]->getName())
        {
            return islands[j];
        }
    }
    return NULL;
}


void list()
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        printf("[%d]: Body ID (%16p) Position (%d) Type: %d\n", i,(void*)entities[i]->getBodyID(), entities.indexOf(i), entities[i]->getType());
    }
}

void defendIsland(dSpaceID space, dWorldID world)
{
    for (int i = 0; i < islands.size(); i++)
    {
        BoxIsland *island = islands[i];

        // First check whether the radar detect that there is some enemy vehicle close to the island center.

        Vehicle *target = NULL;

        if (Structure *c = island->getCommandCenter())
        {
            target = findNearestEnemyVehicle(c->getFaction(),island->getPos(), 3 * 3.6 kmf);

            //printf("Found target %p\n",  target);

            Vehicle *b = target;

            if (!b)
            {
                continue;
            }

            std::vector<size_t> str = island->getStructures();

            for(int i=0;i<str.size();i++)
            {
                // RTTI Stuff
                if(Artillery* lb = dynamic_cast<Artillery*>(entities[str[i]]))
                {
                    if (b->getPos()[1]<60)   // 60 is island height.  Only aim to ground based units
                    {
                        // Find the vector between them, and the parameters for the turret to hit the vehicle, regardless of its random position.
                        Vec3f firingloc = lb->getFiringPort();

                        lb->elevation = -5;
                        lb->azimuth = getAzimuth((b->getPos())-(firingloc));

                        struct controlregister c;
                        c.pitch = 0.0;
                        c.roll = 0.0;
                        lb->setControlRegisters(c);
                        lb->setForward(toVectorInFixedSystem(0,0,1,lb->azimuth, -lb->elevation));

                        std::cout << lb <<  ":Azimuth: " << lb->azimuth << " Inclination: " << lb->elevation << std::endl;

                        Vehicle *action = (lb)->fire(world,space);

                        if (action != NULL)
                        {
                            entities.push_back(action);
                            //gunshot();
                        }
                    }
                } else
                if(LaserTurret* lb = dynamic_cast<LaserTurret*>(entities[str[i]]))
                {
                    // Find the vector between them, and the parameters for the turret to hit the vehicle, regardless of its random position.
                    Vec3f firingloc = lb->getFiringPort();

                    lb->elevation = getDeclination((b->getPos())-(firingloc));
                    lb->azimuth = getAzimuth((b->getPos())-(firingloc));
                    lb->setForward((b->getPos())-(firingloc));

                    struct controlregister c;
                    c.pitch = 0.0;
                    c.roll = 0.0;
                    lb->setControlRegisters(c);

                    std::cout << lb <<  ":Azimuth: " << lb->azimuth << " Inclination: " << lb->elevation << std::endl;

                    lb->setForward((b->getPos())-(firingloc));

                    Vehicle *action = (lb)->fire(world,space);

                    if (action != NULL)
                    {
                        entities.push_back(action);
                        //gunshot();
                    }
                } else
                if(Turret* lb = dynamic_cast<Turret*>(entities[str[i]]))
                {
                    // Find the vector between them, and the parameters for the turret to hit the vehicle, regardless of its random position.
                    Vec3f firingloc = lb->getFiringPort();

                    lb->elevation = getDeclination((b->getPos())-(firingloc));
                    lb->azimuth = getAzimuth((b->getPos())-(firingloc));
                    lb->setForward((b->getPos())-(firingloc));

                    struct controlregister c;
                    c.pitch = 0.0;
                    c.roll = 0.0;
                    lb->setControlRegisters(c);

                    std::cout << lb <<  ":Azimuth: " << lb->azimuth << " Inclination: " << lb->elevation << std::endl;

                    lb->setForward((b->getPos())-(firingloc));

                    Vehicle *action = (lb)->fire(world,space);

                    if (action != NULL)
                    {
                        entities.push_back(action);
                        //gunshot();
                    }
                } else
                if(Launcher* lb = dynamic_cast<Launcher*>(entities[str[i]]))
                {
                    // Find the vector between them, and the parameters for the turret to hit the vehicle, regardless of its random position.
                    Vec3f firingloc = lb->getFiringPort();

                    lb->elevation = getDeclination((b->getPos())-(firingloc));
                    lb->azimuth = getAzimuth((b->getPos())-(firingloc));
                    lb->setForward((b->getPos())-(firingloc));

                    struct controlregister c;
                    c.pitch = 0.0;
                    c.roll = 0.0;
                    lb->setControlRegisters(c);

                    std::cout << lb <<  ":Azimuth: " << lb->azimuth << " Inclination: " << lb->elevation << std::endl;

                    lb->setForward((b->getPos())-(firingloc));

                    Vehicle *action = (lb)->fire(world,space);

                    if (action != NULL)
                    {
                        entities.push_back(action);
                        //gunshot();
                    }
                }
            }
        }
    }
}

void buildAndRepair(dSpaceID space, dWorldID world)
{
    for (int i = 0; i < islands.size(); i++)
    {
        BoxIsland *island = islands[i];

        if (island->getStructures().size()<8)
        {
            CommandCenter *c = findCommandCenter(island);
            if (c)
            {
                if (c->getTtl()<=0)
                {
                    // @TODO Structures should be rotated also.  Now they all have the same orientation.
                    int which = (rand() % 30 + 1);
                    Structure *s;


                    if (1<=which && which<=5)
                        s = new LaserTurret(c->getFaction());
                    else if (6<=which && which<=7)
                        s = new Structure(c->getFaction());
                    else if (8<=which && which<=10)
                        s = new Runway(c->getFaction());
                    else if (11<=which && which<=13)
                        s = new Warehouse(c->getFaction());
                    else if (14<=which && which<=20)
                        s = new Turret(c->getFaction());
                    else if (21<=which && which<=23)
                        s = new Artillery(c->getFaction());
                    else if (24<=which && which<=26)
                        s = new Launcher(c->getFaction());
                    else
                        s = new Warehouse(c->getFaction());


                    //int x = (rand() % 2000 + 1); x -= 1000;
                    //int z = (rand() % 2000 + 1); z -= 1000;

                    island->addStructure(s,world);

                    //island->addStructure(s,x,z,space,world);

                    //entities.push_back(s);

                    if (8<=which && which<=10)
                    {
                        //Structure *s2 = new Hangar(c->getFaction());
                        //z-=550;
                        //island->addStructure(s2,s->getPos()[0]-island->getPos()[0],s->getPos()[2]-island->getPos()[2]-550,space,world);

                        //entities.push_back(s2);
                    }

                    c->restart();
                }

            }
        }

    }
}

Manta* spawnManta(dSpaceID space, dWorldID world,Vehicle *spawner)
{
    int mantaNumber = findNextNumber(MANTA);
    Vehicle *manta = (spawner)->spawn(world,space,MANTA,mantaNumber);
    if (manta != NULL)
    {
        entities.push_back(manta);
        char msg[256];
        sprintf(msg, "Manta %2d is ready to takeoff.",mantaNumber+1);
        messages.insert(messages.begin(), std::string(msg));
    }
    return (Manta*)manta;
}

Walrus* spawnWalrus(dSpaceID space, dWorldID world, Vehicle *spawner)
{
    int walrusNumber = findNextNumber(WALRUS);
    Vehicle *walrus = (spawner)->spawn(world,space,WALRUS,walrusNumber);
    if (walrus != NULL)
    {
        entities.push_back(walrus);
        char msg[256];
        sprintf(msg, "Walrus %2d has been deployed.",NUMBERING(walrusNumber));
        messages.insert(messages.begin(), std::string(msg));
    }
    return (Walrus*)walrus;
}

// SYNCJ
void dockWalrus(Vehicle *dock)
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        // @FIXME: Only put back the walrus that is close to the carrier.
        //printf("Type and ttl: %d, %d\n", vehicles[i]->getType(),vehicles[i]->getTtl());
        if (entities[i]->getType()==WALRUS && entities[i]->getStatus()==Walrus::SAILING &&
                entities[i]->getFaction()==dock->getFaction())
        {
            int walrusNumber = ((Walrus*)entities[i])->getNumber()+1;
            dBodyDisable(entities[i]->getBodyID());
            entities.erase(i);
            char msg[256];
            sprintf(msg, "Walrus %d is now back on deck.",walrusNumber+1);
            messages.insert(messages.begin(), std::string(msg));
        }
    }
}

// SYNC
void dockManta()
{
    for(size_t i=entities.first();entities.hasMore(i);i=entities.next(i))
    {
        //printf("Type and ttl: %d, %d\n", vehicles[i]->getType(),vehicles[i]->getTtl());
        if (entities[i]->getType()==MANTA && entities[i]->getStatus()==Manta::ON_DECK)
        {
            //printf("Eliminating....\n");
            dBodyDisable(entities[i]->getBodyID());
            entities.erase(i);
            messages.insert(messages.begin(), std::string("Manta is now on bay."));
        }
    }
}


void landManta(Vehicle *v)
{
    // Auto control
    if (v->getType() == CARRIER)
    {
        Balaenidae *b = (Balaenidae*)v;
        Manta *m = findManta(Manta::HOLDING);

        if (m)
        {
            m->setDestination(b->getPos());             // @FIXME: This needs to be performed all the time while manta is landing.
            m->setAttitude(b->getForward());
            m->land();
            m->enableAuto();
        }
    }
}


void launchManta(Vehicle *v)
{
    if (v->getType() == CARRIER)
    {
        Balaenidae *b = (Balaenidae*)v;
        Manta *m = findManta(Manta::ON_DECK);
        if (m)
        {
            b->launch(m);
            char msg[256];
            sprintf(msg, "Manta %2d has been launched.", m->getNumber()+1);
            messages.insert(messages.begin(), std::string(msg));
            takeoff();
        }
    }
}

void captureIsland(BoxIsland *island, int faction, dSpaceID space, dWorldID world)
{

    Structure *s = island->addStructure(new CommandCenter(faction),world);

    char msg[256];
    sprintf(msg, "Island %s is now under control of %s.", island->getName().c_str(),FACTION(faction));
    messages.insert(messages.begin(), std::string(msg));
}


void playFaction(unsigned long timer, int faction, dSpaceID space, dWorldID world)
{
    static int statuses[2] = {9,9};
    static unsigned long timeevent = 0;

    // @FIXME: This is SO wrong.
    int status = statuses[faction-1];

    int action=-1;

    switch (status) {
    case 9:
    {
        Vehicle *b = findCarrier(faction);

        if (b)
        {
            BoxIsland *is = findNearestIsland(b->getPos(),false, faction,100 kmf);

            if (!is)
            {
                // Closest enemy island is not found, lets check for an empty island.
                status=0;
                break;
            }

            Vec3f vector = (b->getPos()) - (is->getPos());

            vector = vector.normalize();

            b->setDestination(is->getPos()+Vec3f(12500.0f * vector));
            b->enableAuto();
            status = 10;
        }

        break;
    }
    case 10:
    {
        Vehicle *b = findCarrier(faction);

        if (b)
        {
            BoxIsland *is = findNearestIsland(b->getPos(),false, faction);

            if (!b->isAuto())
            {
                printf("Carries has arrived to destination.\n");

                //Walrus* w = spawnWalrus(space,world,b);
                //w->setDestination(is->getPos());
                //w->enableAuto();
                status = 11;timeevent = timer;

            }
        }

        break;
    }
    case 11:
    {
        Vehicle *b = findCarrier(faction);

        if (b)
        {
            if (timer==(timeevent + 200))
            {
                Manta *m = spawnManta(space,world,b);

            }

            if (timer==(timeevent + 300))
            {
                launchManta(b);
            }

            if (timer==(timeevent + 400))
            {
                BoxIsland *i = findNearestIsland(b->getPos());

                Manta *m = findManta(Manta::FLYING);

                CommandCenter *c = (CommandCenter*)i->getCommandCenter();

                m->attack(c->getPos());
                m->enableAuto();
            }


            if (timer>(timeevent + 400))
            {
                BoxIsland *i = findNearestIsland(b->getPos());

                Manta *m = findManta(Manta::FLYING);

                if (!m)
                {
                    status = 11;
                    timeevent = timer;
                    break;
                }

                CommandCenter *c = (CommandCenter*)i->getCommandCenter();

                if (!c)
                {
                    // Command Center is destroyed.

                    // @FIXME: find the right manta, the one which is closer.
                    Manta *m = findManta(Manta::FLYING);


                    if (m)
                    {
                        m->setDestination(Vec3f(i->getPos()[0],1000,i->getPos()[2]));
                        m->enableAuto();
                    }


                    status = 0;
                }
            }
        }

        break;
    }
    case 0: //find nearest island.
    {
        Vehicle *b = findCarrier(faction);

        if (b)
        {
            //BoxIsland *enemyis = findNearestIsland(b->getPos(), false, faction);

            BoxIsland *is = findNearestEmptyIsland(b->getPos());

            Vec3f vector = (b->getPos()) - (is->getPos());

            vector = vector.normalize();

            b->setDestination(is->getPos()+Vec3f(3500.0f * vector));
            b->enableAuto();
            status = 1;
        }

        break;
    }
    case 1:
    {
        Vehicle *b = findCarrier(faction);

        if (b)
        {
            BoxIsland *is = findNearestEmptyIsland(b->getPos());

            if (!b->isAuto())
            {
                printf("Carries has arrived to destination.\n");

                Walrus* w = spawnWalrus(space,world,b);
                w->setDestination(is->getPos());
                w->enableAuto();
                status = 2;timeevent = timer;

                Manta *m = findManta(Manta::HOLDING);

                if (m)
                {
                    landManta(b);
                } else {
                    m = findManta(Manta::FLYING);

                    if (m)
                    {
                        landManta(b);
                    }
                }

            }
        }

        break;
    }
    case 2:
    {
        // Need to check if walrus arrived to island.
        Walrus *w = findWalrus(faction);

        if (w)
        {
            BoxIsland *is = findNearestEmptyIsland(w->getPos());

            // @FIXME: It may happen that the walrus is wandering do nothing never de-autopilot and this is endless.
            if (!w->isAuto() || ( w->getIsland() == is && timer>(timeevent + 1000))  )
            {
                captureIsland(is,w->getFaction(),space, world);
                status=3;
            }
        } else
        {
            // Walrus has been destroyed.
            status = 1;
        }

        break;
    }
    case 3:
    {
        Vehicle *b = findCarrier(faction);
        // @FIXME I should find the one that actually went to the island.
        Walrus *w = findWalrus(faction);

        if (w)
        {
            w->setDestination(b->getPos()+Vec3f(0.0f,0.0f,-50.0f));
            w->enableAuto();
            status = 4;
        } else {
            // There are no walrus so I can departure to the next island.
            status = 9;
        }
        break;
    }
    case 4:
    {
        Vehicle *b = findCarrier(faction);
        Walrus *w = findWalrus(faction);

        Manta *m = findManta(Manta::ON_DECK);

        if (w)
        {
            if (b && w && !(w->isAuto()))
            {
                synchronized(entities.m_mutex)
                {
                    dockWalrus(b);
                    if (m)
                    {
                        dockManta();
                    }
                }
                status=9;
            }
        } else {
            // There are no walrus so I can departure to the next island.
            status = 9;
        }

        break;
    }
    default:
        break;

    }

    statuses[faction-1] = status;

}
