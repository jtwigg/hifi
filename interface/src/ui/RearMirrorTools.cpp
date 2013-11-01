//
//  RearMirrorTools.cpp
//  interface
//
//  Created by stojce on 23.10.2013.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.

#include "RearMirrorTools.h"
#include "Util.h"

#include <SharedUtil.h>
#include <QMouseEvent>

const char SETTINGS_GROUP_NAME[] = "Rear View Tools";
const char ZOOM_LEVEL_SETTINGS[] = "ZoomLevel";
const int ICON_SIZE = 20;
const int ICON_PADDING = 5;
const int MID_ICON_PADDING = 70;

RearMirrorTools::RearMirrorTools(QGLWidget* parent, QRect& bounds, QSettings* settings) :
    _parent(parent),
    _bounds(bounds),
    _windowed(false),
    _fullScreen(false)
{
    _zoomLevel = HEAD,
    switchToResourcesParentIfRequired();
    _closeTextureId = _parent->bindTexture(QImage("./resources/images/close.png"));
    _resetTextureId = _parent->bindTexture(QImage("./resources/images/reset.png"));
    _zoomHeadTextureId = _parent->bindTexture(QImage("./resources/images/head.png"));
    _zoomBodyTextureId = _parent->bindTexture(QImage("./resources/images/body.png"));

    _shrinkIconRect = QRect(ICON_PADDING, ICON_PADDING, ICON_SIZE, ICON_SIZE);
    _closeIconRect = QRect(_bounds.left() + ICON_PADDING, _bounds.top() + ICON_PADDING, ICON_SIZE, ICON_SIZE);
    _resetIconRect = QRect(_bounds.width() - ICON_SIZE - ICON_PADDING, _bounds.top() + ICON_PADDING, ICON_SIZE, ICON_SIZE);
    _bodyZoomIconRect = QRect(_bounds.width() - MID_ICON_PADDING - ICON_SIZE, _bounds.bottom() - ICON_PADDING - ICON_SIZE, ICON_SIZE, ICON_SIZE);
    _headZoomIconRect = QRect(_bounds.left() + MID_ICON_PADDING, _bounds.bottom() - ICON_PADDING - ICON_SIZE, ICON_SIZE, ICON_SIZE);
    
    settings->beginGroup(SETTINGS_GROUP_NAME);
    _zoomLevel = loadSetting(settings, ZOOM_LEVEL_SETTINGS, 0) == HEAD ? HEAD : BODY;
    settings->endGroup();
};

void RearMirrorTools::render(bool fullScreen) {
    if (fullScreen) {
        _fullScreen = true;
        displayIcon(_parent->geometry(), _shrinkIconRect, _closeTextureId);
    } else {
        // render rear view tools if mouse is in the bounds
        QPoint mousePosition = _parent->mapFromGlobal(QCursor::pos());
        _windowed = _bounds.contains(mousePosition.x(), mousePosition.y());
        if (_windowed) {
            displayIcon(_bounds, _closeIconRect, _closeTextureId);
            displayIcon(_bounds, _resetIconRect, _resetTextureId);
            displayIcon(_bounds, _headZoomIconRect, _zoomHeadTextureId, _zoomLevel == HEAD);
            displayIcon(_bounds, _bodyZoomIconRect, _zoomBodyTextureId, _zoomLevel == BODY);
        }
    }
}

bool RearMirrorTools::mousePressEvent(int x, int y) {
    if (_windowed) {
        if (_closeIconRect.contains(x, y)) {
            _windowed = false;
            emit closeView();
            return true;
        }
        
        if (_resetIconRect.contains(x, y)) {
            emit resetView();
            return true;
        }
        
        if (_headZoomIconRect.contains(x, y)) {
            _zoomLevel = HEAD;
            return true;
        }
        
        if (_bodyZoomIconRect.contains(x, y)) {
            _zoomLevel = BODY;
            return true;
        }
        
        if (_bounds.contains(x, y)) {
            _windowed = false;
            emit restoreView();
            return true;
        }
    }
    
    if (_fullScreen) {
        if (_shrinkIconRect.contains(x, y)) {
            _fullScreen = false;
            emit shrinkView();
            return true;
        }
    }
    return false;
}

void RearMirrorTools::saveSettings(QSettings* settings) {
    settings->beginGroup(SETTINGS_GROUP_NAME);
    settings->setValue(ZOOM_LEVEL_SETTINGS, _zoomLevel);
    settings->endGroup();
}

void RearMirrorTools::displayIcon(QRect bounds, QRect iconBounds, GLuint textureId, bool selected) {

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    gluOrtho2D(bounds.left(), bounds.right(), bounds.bottom(), bounds.top());
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    if (selected) {
        glColor3f(1, 1, 0);
    } else {
        glColor3f(1, 1, 1);
    }
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);
        glVertex2f(iconBounds.left(), iconBounds.bottom());
        
        glTexCoord2f(0, 1);
        glVertex2f(iconBounds.left(), iconBounds.top());
        
        glTexCoord2f(1, 1);
        glVertex2f(iconBounds.right(), iconBounds.top());
        
        glTexCoord2f(1, 0);
        glVertex2f(iconBounds.right(), iconBounds.bottom());
        
    }
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
