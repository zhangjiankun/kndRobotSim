/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE
class GLWidget;
class RobotControlPanel;
class modelTree;
class position;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void renderIntoPixmap();
    void grabFrameBuffer();
    void clearPixmap();
    void about();
    void showControlPanel();
    void showModelPanel();
    void showPositionPanel();
    void loadRobotModel();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createContextMenu();
    void createStatusBar();
    QSlider *createSlider(const char *changedSignal, const char *setterSlot);
    void setPixmap(const QPixmap &pixmap);
    QSize getSize();

    QWidget *centralWidget;
    QScrollArea *glWidgetArea;
    QScrollArea *pixmapLabelArea;
    GLWidget *glWidget;
    QLabel *pixmapLabel;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    RobotControlPanel *controlPanelUI;
    modelTree *modelPanelUI;
    position *positionPanelUI;

    QMenu *fileMenu;
    QMenu *helpMenu;
    QMenu *modelMenu;
    QAction *grabFrameBufferAct;
    QAction *renderIntoPixmapAct;
    QAction *clearPixmapAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *controlPanelAction;
    QAction *modelPanelAction;
    QAction *positionPanelAction;
    QAction *loadModelAct;

    QToolBar *controlTool;
    QToolBar *modelTool;
};

#endif
