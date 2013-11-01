//
//  VoxelMouseAction.h
//  hifi
//
//  Created by John Twigg on 2013-10-31.
//
//

#ifndef __hifi__VoxelMouseAction__
#define __hifi__VoxelMouseAction__

#include <iostream>
#include "SharedUtil.h"
#include "AABox.h"
#include "PieMenu.h"


enum eQuadrant
{
    QUADRANT_LEFT, //Create voxel
    QUADRANT_RIGHT, //delete voxel.
    QUADRANT_UP,   //TODO: delete/chip away at voxel.
    QUADRANT_DOWN //move to voxel
};

class Application;

//Responsible for handling actions that occur when you select a voxel, and then drag the cursor left/right/up/down
class VoxelMouseAction
{
public:
    VoxelMouseAction(Application * application);
    
    //Is a  voxel mouse action currently in progress?
    bool isActive(){return _isVoxelSelected;};

    
    //Mouse Events.
    void mouseMoveEvent(int mouseX, int mouseY);
    void mousePressEvent(int mouseX, int mouseY,const VoxelDetail & hoverVoxel, float hoverVoxelDistance, BoxFace hoverBoxFace  );
    void mouseReleaseEvent();
    
    //Render
    void display();
    
private:
    Application * _application;
    
    //-----------------------------------
    
    //Voxel that was downclicked upon.
    bool _isVoxelSelected;
    
    VoxelDetail _clickedVoxel;
    float _clickedVoxelDistance;
    BoxFace _clickedVoxelFace;
    int _clickedMouseX;
    int _clickedMouseY;
    glm::vec3 _clickedOrigin;
    glm::vec3 _clickedDirection;
    eQuadrant _quadrant;
    
    //If we've crossed the boudary for a voxel manipilation action.
    bool _isVoxelActionInProgress;
    //-----------------------------------
    
    //Create Voxel workflow.
    void BeginPlaceNewVoxel();
    void UpdatePlaceNewVoxel(int mouseX);
    void FinalizePlaceNewVoxel();
    
    bool _placingNewVoxel;
    int  _currentSize;
    VoxelDetail _newVoxel;
    
  
    //-----------------------------------
    
};



#endif /* defined(__hifi__VoxelMouseAction__) */
