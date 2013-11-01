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
    QUADRANT_LEFT,
    QUADRANT_RIGHT,
    QUADRANT_UP,
    QUADRANT_DOWN
};

class Application;

class VoxelMouseAction
{
public:
    VoxelMouseAction(Application * application);
    
    bool isActive(){return _isVoxelSelected;};
    
    void mouseMoveEvent(int mouseX, int mouseY);
    void mousePressEvent(int mouseX, int mouseY,const VoxelDetail & hoverVoxel, float hoverVoxelDistance, BoxFace hoverBoxFace  );
    
    void mouseReleaseEvent();
    
    void display();
    
private:
    
    void BeginPlaceNewVoxel();
    void UpdatePlaceNewVoxel(int mouseX);
    void FinalizePlaceNewVoxel();
    
    Application * _application;
    
    
    //Voxel that was downclicked upon.
    VoxelDetail _clickedVoxel;
    float _clickedVoxelDistance;
    BoxFace _clickedVoxelFace;

    int _clickedMouseX;
    int _clickedMouseY;
    glm::vec3 _clickedOrigin;
    glm::vec3 _clickedDirection;

    //-----------------------------------
    
    VoxelDetail _newVoxel;
    bool _isVoxelSelected;
    bool _isVoxelActionInProgress;
    eQuadrant _quadrant;
    
    bool _placingNewVoxel;
    int  _currentSize;

    
};



#endif /* defined(__hifi__VoxelMouseAction__) */
