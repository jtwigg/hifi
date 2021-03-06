//
//  AvatarTouch.cpp
//  interface
//
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.

#include <iostream>
#include <glm/glm.hpp>
#include <SharedUtil.h>
#include "AvatarTouch.h"
#include "InterfaceConfig.h"
#include "Util.h"

const float THREAD_RADIUS = 0.007;
const float HANDS_CLOSE_ENOUGH_TO_GRASP = 0.2;
const float AVATAR_FACING_THRESHOLD = -0.5f;    // (-1 to 1) (larger value indicates narrower angle of influence

AvatarTouch::AvatarTouch() {

    _myHandPosition          = glm::vec3(0.0f, 0.0f, 0.0f);
    _yourHandPosition        = glm::vec3(0.0f, 0.0f, 0.0f);
    _myBodyPosition          = glm::vec3(0.0f, 0.0f, 0.0f);
    _yourBodyPosition        = glm::vec3(0.0f, 0.0f, 0.0f);
    _vectorBetweenHands      = glm::vec3(0.0f, 0.0f, 0.0f);
    _myHandState             = HAND_STATE_NULL;
    _yourHandState           = HAND_STATE_NULL;  
    _reachableRadius         = 0.0f;  
    _weAreHoldingHands       = false;
    _canReachToOtherAvatar   = false;
    _handsCloseEnoughToGrasp = false;
    _hasInteractingOther     = false;

    for (int p=0; p<NUM_PARTICLE_POINTS; p++) {
        _point[p] = glm::vec3(0.0, 0.0, 0.0);
    }
}

void AvatarTouch::simulate (float deltaTime) {

    _canReachToOtherAvatar = false; // default

    if (_hasInteractingOther) {

        glm::vec3 vectorBetweenBodies = _yourBodyPosition - _myBodyPosition;
        float distanceBetweenBodies = glm::length(vectorBetweenBodies);
        
        //KEEP THIS - it is another variation that we are considering getting rid of 
        //the following code take into account of the two avatars are facing each other
        /*
        glm::vec3 directionBetweenBodies = vectorBetweenBodies / distanceBetweenBodies;
        
        bool facingEachOther = false;
        
        glm::vec3 myFront = _myOrientation * AVATAR_FRONT;
        glm::vec3 yourFront = _yourOrientation * AVATAR_FRONT;
        
        if (( glm::dot(myFront, yourFront              ) < -AVATAR_FACING_THRESHOLD)      // we're facing each other
        &&  ( glm::dot(myFront, directionBetweenBodies ) >  AVATAR_FACING_THRESHOLD)) {   // I'm facing you
            facingEachOther = true;
        }
        */
        
        if (distanceBetweenBodies < _reachableRadius)
        {
            _canReachToOtherAvatar = true;

            _vectorBetweenHands = _yourHandPosition - _myHandPosition;
            
            float distanceBetweenHands = glm::length(_vectorBetweenHands);
            if (distanceBetweenHands < HANDS_CLOSE_ENOUGH_TO_GRASP) {
                _handsCloseEnoughToGrasp = true;
            } else {
                _handsCloseEnoughToGrasp = false;
            }
        }
    }
}


void AvatarTouch::render(glm::vec3 cameraPosition) {

    if (_canReachToOtherAvatar) {

        //show circle indicating that we can reach out to each other...
        glColor4f(0.3, 0.4, 0.5, 0.5); 
        glm::vec3 p(_yourBodyPosition);
        p.y = 0.0005f;
        renderCircle(p, _reachableRadius, glm::vec3(0.0f, 1.0f, 0.0f), 30);

        // show if we are holding hands...
        if (_weAreHoldingHands) {
            renderBeamBetweenHands();
            
            /*
            glPushMatrix();
            glTranslatef(_yourHandPosition.x, _yourHandPosition.y, _yourHandPosition.z);
            glColor4f(1.0, 0.0, 0.0, 0.7); glutSolidSphere(0.020f, 10.0f, 10.0f);
            glColor4f(1.0, 0.0, 0.0, 0.7); glutSolidSphere(0.025f, 10.0f, 10.0f);
            glColor4f(1.0, 0.0, 0.0, 0.7); glutSolidSphere(0.030f, 10.0f, 10.0f);
            glPopMatrix();
            */
        
            /*
            glColor4f(0.9, 0.3, 0.3, 0.5);
            renderSphereOutline(_myHandPosition,   HANDS_CLOSE_ENOUGH_TO_GRASP * 0.3f, 20, cameraPosition);
            renderSphereOutline(_myHandPosition,   HANDS_CLOSE_ENOUGH_TO_GRASP * 0.2f, 20, cameraPosition);
            renderSphereOutline(_myHandPosition,   HANDS_CLOSE_ENOUGH_TO_GRASP * 0.1f, 20, cameraPosition);

            renderSphereOutline(_yourHandPosition, HANDS_CLOSE_ENOUGH_TO_GRASP * 0.3f, 20, cameraPosition);
            renderSphereOutline(_yourHandPosition, HANDS_CLOSE_ENOUGH_TO_GRASP * 0.2f, 20, cameraPosition);
            renderSphereOutline(_yourHandPosition, HANDS_CLOSE_ENOUGH_TO_GRASP * 0.1f, 20, cameraPosition);
            */
        }

        //show that our hands are close enough to grasp..
        if (_handsCloseEnoughToGrasp) {
            glColor4f(0.9, 0.3, 0.3, 0.5);
            renderSphereOutline(_myHandPosition,   0.03f, 20, cameraPosition);
            renderSphereOutline(_yourHandPosition, 0.03f, 20, cameraPosition);
        }
        
        // if your hand is grasping, show it...
        if (_yourHandState == HAND_STATE_GRASPING) {
            glPushMatrix();
            glTranslatef(_yourHandPosition.x, _yourHandPosition.y, _yourHandPosition.z);
            glColor4f(1.0, 0.7, 0.8, 0.4); glutSolidSphere(0.020f, 10.0f, 10.0f);
            glColor4f(1.0, 0.7, 0.4, 0.3); glutSolidSphere(0.025f, 10.0f, 10.0f);
            glColor4f(1.0, 0.7, 0.2, 0.2); glutSolidSphere(0.030f, 10.0f, 10.0f);
            glPopMatrix();
        }
     }
    
    // if my hand is grasping, show it...
    if (_myHandState == HAND_STATE_GRASPING) {
        glPushMatrix();
        glTranslatef(_myHandPosition.x, _myHandPosition.y, _myHandPosition.z);
        glColor4f(1.0, 0.7, 0.8, 0.4); glutSolidSphere(0.020f, 10.0f, 10.0f);
        glColor4f(1.0, 0.7, 0.4, 0.3); glutSolidSphere(0.025f, 10.0f, 10.0f);
        glColor4f(1.0, 0.7, 0.2, 0.2); glutSolidSphere(0.030f, 10.0f, 10.0f);
        glPopMatrix();
    }
}


void AvatarTouch::renderBeamBetweenHands() {

    glm::vec3 v1(_myHandPosition);
    glm::vec3 v2(_yourHandPosition);

    glLineWidth(3.0);
    glColor4f(0.9f, 0.9f, 0.1f, 0.6);
    glBegin(GL_LINE_STRIP);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glEnd();

    glColor3f(0.5f, 0.3f, 0.0f);
    for (int p=0; p<NUM_PARTICLE_POINTS; p++) {

        _point[p] = _myHandPosition + _vectorBetweenHands * ((float)p / (float)NUM_PARTICLE_POINTS);
        _point[p].x += randFloatInRange(-THREAD_RADIUS, THREAD_RADIUS);
        _point[p].y += randFloatInRange(-THREAD_RADIUS, THREAD_RADIUS);
        _point[p].z += randFloatInRange(-THREAD_RADIUS, THREAD_RADIUS);

        glBegin(GL_POINTS);
        glVertex3f(_point[p].x, _point[p].y, _point[p].z);
        glEnd();
    }    
}



