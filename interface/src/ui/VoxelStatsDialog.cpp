//
//  VoxelStatsDialog.cpp
//  interface
//
//  Created by Brad Hefta-Gaub on 7/19/13.
//  Copyright (c) 2013 High Fidelity, Inc. All rights reserved.
//

#include <QFormLayout>
#include <QDialogButtonBox>

#include <QPalette>
#include <QColor>

#include <VoxelSceneStats.h>

#include "ui/VoxelStatsDialog.h"


VoxelStatsDialog::VoxelStatsDialog(QWidget* parent, VoxelSceneStats* model) :
    QDialog(parent, Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint),
    _model(model) {

    char strBuf[64];

    this->setWindowTitle("Voxel Statistics");

    // Create layouter
    QFormLayout* form = new QFormLayout();
    this->QDialog::setLayout(form);

    // Setup labels
    for (int i = 0; i < VoxelSceneStats::ITEM_COUNT; i++) {
        VoxelSceneStats::Item item = (VoxelSceneStats::Item)(i);
        VoxelSceneStats::ItemInfo& itemInfo = _model->getItemInfo(item);
        QLabel* label = _labels[item] = new QLabel();  

        // Set foreground color to 62.5% brightness of the meter (otherwise will be hard to read on the bright background)
        QPalette palette = label->palette();
        unsigned rgb = itemInfo.colorRGBA >> 8;
        const unsigned colorpart1 = 0xfefefeu;
        const unsigned colorpart2 = 0xf8f8f8;
        rgb = ((rgb & colorpart1) >> 1) + ((rgb & colorpart2) >> 3);
        palette.setColor(QPalette::WindowText, QColor::fromRgb(rgb));
        label->setPalette(palette);
        
        const int STATS_LABEL_WIDTH = 550;
        label->setFixedWidth(STATS_LABEL_WIDTH);

        snprintf(strBuf, sizeof(strBuf), " %s:", itemInfo.caption);
        form->addRow(strBuf, label);
    }
}

VoxelStatsDialog::~VoxelStatsDialog() {
    for (int i = 0; i < VoxelSceneStats::ITEM_COUNT; ++i) {
        delete _labels[i];
    }
}

void VoxelStatsDialog::paintEvent(QPaintEvent* event) {

    // Update labels
    char strBuf[256];
    for (int i = 0; i < VoxelSceneStats::ITEM_COUNT; i++) {
        VoxelSceneStats::Item item = (VoxelSceneStats::Item)(i);
        QLabel* label = _labels[item];
        snprintf(strBuf, sizeof(strBuf), "%s", _model->getItemValue(item));
        label->setText(strBuf);
    }

    this->QDialog::paintEvent(event);
    this->setFixedSize(this->width(), this->height());
}

void VoxelStatsDialog::reject() {
    // Just regularly close upon ESC
    this->QDialog::close();
}

void VoxelStatsDialog::closeEvent(QCloseEvent* event) {
    this->QDialog::closeEvent(event);
    emit closed();
}


