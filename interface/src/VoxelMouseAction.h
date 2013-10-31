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
#include "SharedUtils.h"


class VoxelMouseAction
{
    
  
    //Voxel that was downclicked upon.
    VoxelDetail _SelectedVoxel;
    float _SelectedVoxelDistance;
    BoxFace _SelectedVoxelFace;
    
    VoxelDetail _NewVoxel;
    bool _isVoxelSelected;
    bool _isVoxelSelectedBeingModified;
    eQuadrant _quadrant;
    int _ClickedMouseX;
    int _ClickedMouseY;
    glm::vec3 _ClickedOrigin;
    glm::vec3 _ClickedDirection;
    
    bool _PlacingNewVoxel;
    int  _currentSize;

    
};
xx


#endif /* defined(__hifi__VoxelMouseAction__) */
