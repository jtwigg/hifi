//
//  VoxelMouseAction.cpp
//  hifi
//
//  Created by John Twigg on 2013-10-31.
//
//


#include "VoxelMouseAction.h"
#include "PacketHeaders.h"
#include "Application.h"
#include "gl.h"

const int  SELECTED_VOXEL_MOUSE_TRANSLATION = 5;
const int  SELECTED_VOXEL_SCALER_WIDTH = 50;
const int  SIZE_VARIANTS_OF_VOXEL = 8;


eQuadrant getQuadrant(int initalMouseX, int initialMouseY, int finalMouseX, int finalMouseY)
{
    
    if(abs( finalMouseX - initalMouseX) >= abs(finalMouseY - initialMouseY ))
    {
        if(finalMouseX < initalMouseX )
        {
            return QUADRANT_LEFT;
        }
        else
        {
            return QUADRANT_RIGHT;
        }
    }
    else
    {
        if(initialMouseY > finalMouseY)
        {
            return QUADRANT_UP;
        }
        else
        {
            return QUADRANT_DOWN;
        }
    }
    
}

VoxelMouseAction::VoxelMouseAction(Application * application){
    
    _application = application;
    _clickedVoxelDistance = 0;
    
    _isVoxelSelected = false;
    _isVoxelActionInProgress = false;
    _quadrant = QUADRANT_LEFT;
    _clickedMouseX = 0;
    _clickedMouseY = 0;
    
    _placingNewVoxel = false;
    _currentSize = 1;

}

void VoxelMouseAction::mouseMoveEvent(int mouseX, int mouseY){
    if(_isVoxelSelected)
    {
        if(!_isVoxelActionInProgress)
        {
            if((abs(mouseX - _clickedMouseX)  > SELECTED_VOXEL_MOUSE_TRANSLATION) ||
               (abs(mouseY - _clickedMouseY)  > SELECTED_VOXEL_MOUSE_TRANSLATION))
            {
                
                _quadrant = getQuadrant(_clickedMouseX, _clickedMouseY, mouseX, mouseY);
                _isVoxelActionInProgress = true;
                switch (_quadrant) {
                    case QUADRANT_LEFT:
                    {
                        BeginPlaceNewVoxel();
                        break;
                    }
                        
                    default:
                        break;
                }
            }
        }
        
        if(_isVoxelActionInProgress)
        {
            switch (_quadrant) {
                case QUADRANT_LEFT:
                {
                    UpdatePlaceNewVoxel(mouseX);
                    break;
                }
                    
                default:
                    break;
            }
            
        }
    }
}

void VoxelMouseAction::mousePressEvent(int mouseX, int mouseY,const VoxelDetail & hoverVoxel, float hoverVoxelDistance,  BoxFace  hoverBoxFace  ){
    
    _isVoxelSelected = true;
    _clickedMouseX = mouseX;
    _clickedMouseY = mouseY;
    
    _clickedVoxel = hoverVoxel;
    _clickedVoxelDistance =  hoverVoxelDistance;
    _clickedVoxelFace = hoverBoxFace;
    
    _application->getAvatar()->getMouseRay( _clickedOrigin, _clickedDirection);
}

void VoxelMouseAction::mouseReleaseEvent(){
    if(_isVoxelActionInProgress)
    {
        switch (_quadrant) {
            case QUADRANT_UP:
                
                break;
            case QUADRANT_DOWN:
            {
                _application->moveTowardsVoxel(_clickedVoxel);
            }
                break;
            case QUADRANT_LEFT:
                FinalizePlaceNewVoxel();
                
                break;
            case QUADRANT_RIGHT:
                _application->deleteVoxel(_clickedVoxel);
                break;
                
            default:
                break;
        }
    }
    
    _isVoxelActionInProgress = false;
    _isVoxelSelected = false;

}

void VoxelMouseAction::display(){
    if(_placingNewVoxel)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glScalef(TREE_SCALE, TREE_SCALE, TREE_SCALE);
        glTranslatef(_newVoxel.x + _newVoxel.s*0.5f,
                     _newVoxel.y + _newVoxel.s*0.5f,
                     _newVoxel.z + _newVoxel.s*0.5f);
        glLineWidth(4.0f);
        glutWireCube(_newVoxel.s);
        glLineWidth(1.0f);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

}


void VoxelMouseAction::BeginPlaceNewVoxel(){
    
    _placingNewVoxel = true;
    _currentSize = 1;

}

void VoxelMouseAction::UpdatePlaceNewVoxel(int mouseX){
    int voxelScaler = _clickedMouseX - mouseX - SELECTED_VOXEL_MOUSE_TRANSLATION;
    
    if(voxelScaler < 1)
        voxelScaler = 1;
    else if(voxelScaler > SELECTED_VOXEL_SCALER_WIDTH)
        voxelScaler = SELECTED_VOXEL_SCALER_WIDTH;
    
    float percentage = (float)voxelScaler / (float)SELECTED_VOXEL_SCALER_WIDTH;
    _currentSize = percentage * SIZE_VARIANTS_OF_VOXEL + 1;
    
    float newVoxelScale = _clickedVoxel.s * 1.0f/8.0f * (float)powf(2, _currentSize);
    
    //  Find the voxel we are hovering over, and respond if clicked
    
    _newVoxel = _clickedVoxel;
    
    // find the nearest voxel with the desired scale
    if (newVoxelScale > _newVoxel.s) {
        // choose the larger voxel that encompasses the one selected
        _newVoxel.x = newVoxelScale * floorf(_newVoxel.x / newVoxelScale);
        _newVoxel.y = newVoxelScale * floorf(_newVoxel.y / newVoxelScale);
        _newVoxel.z = newVoxelScale * floorf(_newVoxel.z / newVoxelScale);
        _newVoxel.s = newVoxelScale;
        
    }
    else
    {
        glm::vec3 faceVector = AABox::getFaceVector(_clickedVoxelFace);
        if (newVoxelScale < _newVoxel.s) {
            // find the closest contained voxel
            glm::vec3 pt = (_clickedOrigin + _clickedDirection * _clickedVoxelDistance) / (float)TREE_SCALE +
            faceVector * (newVoxelScale * 0.5f);
            _newVoxel.x = newVoxelScale * floorf(pt.x / newVoxelScale);
            _newVoxel.y = newVoxelScale * floorf(pt.y / newVoxelScale);
            _newVoxel.z = newVoxelScale * floorf(pt.z / newVoxelScale);
            _newVoxel.s = newVoxelScale;
        }
        else
        {
            // use the face to determine the side on which to create a neighbor
            _newVoxel.x += faceVector.x * _newVoxel.s;
            _newVoxel.y += faceVector.y * _newVoxel.s;
            _newVoxel.z += faceVector.z * _newVoxel.s;
        }
    }
}

void VoxelMouseAction::FinalizePlaceNewVoxel(){
    _placingNewVoxel = false;
    
    
    PACKET_TYPE message = PACKET_TYPE_SET_VOXEL_DESTRUCTIVE;
    
    _application->getVoxelEditSender()->sendVoxelEditMessage(message, _newVoxel);
    
    _application->getVoxels()->createVoxel(_newVoxel.x, _newVoxel.y, _newVoxel.z, _newVoxel.s,
                        _newVoxel.red, _newVoxel.green, _newVoxel.blue,
                        true);

}
