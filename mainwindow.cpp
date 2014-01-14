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

#include <QtGui>
#include <QtOpenGL>

#include "glwidget.h"
#include "mainwindow.h"
#include "ui/robotcontrolpanel.h"
#include "ui/modeltree.h"
#include "ui/position.h"

MainWindow::MainWindow()
{
    centralWidget = new QWidget;

    controlPanelUI = NULL;
    modelPanelUI = NULL;
    positionPanelUI = NULL;

    setCentralWidget(centralWidget);

    glWidget = new GLWidget;
    pixmapLabel = new QLabel;

    glWidgetArea = new QScrollArea;
    glWidgetArea->setWidget(glWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(50, 50);

    zSlider = createSlider(SIGNAL(zRotationChanged(int)),
                           SLOT(setZRotation(int)));

    createActions();
    createMenus();
    createToolBars();

    QGridLayout *centralLayout = new QGridLayout;
    centralLayout->addWidget(glWidgetArea, 0, 0);

    centralLayout->addWidget(zSlider, 1, 0);
    centralWidget->setLayout(centralLayout);

    zSlider->setValue(0 * 16);

    setWindowTitle(tr("Grabber"));
    setWindowIcon(QIcon(":/resource/imgs/controlPanel.jpg"));
    resize(800, 600);
}

void MainWindow::renderIntoPixmap()
{
    QSize size = getSize();
    if (size.isValid()) {
        QPixmap pixmap = glWidget->renderPixmap(size.width(), size.height());
        setPixmap(pixmap);
    }
}

void MainWindow::grabFrameBuffer()
{
    QImage image = glWidget->grabFrameBuffer();
    setPixmap(QPixmap::fromImage(image));
}

void MainWindow::clearPixmap()
{
    setPixmap(QPixmap());
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Grabber"),
            tr("The <b>Grabber</b> example demonstrates two approaches for "
               "rendering OpenGL into a Qt pixmap."));
}

void MainWindow::createActions()
{
    positionPanelAction = new QAction(tr("&position"), this);
    positionPanelAction->setIcon(QIcon(":/resource/imgs/position.png"));
    positionPanelAction->setShortcut(tr("Ctrl+P"));
    connect(positionPanelAction, SIGNAL(triggered()), this, SLOT(showPositionPanel()));

    modelPanelAction = new QAction(tr("&model"), this);
    modelPanelAction->setIcon(QIcon(":/resource/imgs/modelTree.png"));
    modelPanelAction->setShortcut(tr("Ctrl+M"));
    connect(modelPanelAction, SIGNAL(triggered()), this, SLOT(showModelPanel()));

    controlPanelAction = new QAction(tr("&New"), this);
    controlPanelAction->setIcon(QIcon(":/resource/imgs/controlPanel1.png"));
    controlPanelAction->setShortcut(tr("Ctrl+P"));
    controlPanelAction->setStatusTip(tr("Control panel"));
    connect(controlPanelAction, SIGNAL(triggered()), this, SLOT(showControlPanel()));

    renderIntoPixmapAct = new QAction(tr("&Render into Pixmap..."), this);
    renderIntoPixmapAct->setShortcut(tr("Ctrl+R"));
    connect(renderIntoPixmapAct, SIGNAL(triggered()),
            this, SLOT(renderIntoPixmap()));

    grabFrameBufferAct = new QAction(tr("&Grab Frame Buffer"), this);
    grabFrameBufferAct->setShortcut(tr("Ctrl+G"));
    connect(grabFrameBufferAct, SIGNAL(triggered()),
            this, SLOT(grabFrameBuffer()));

    clearPixmapAct = new QAction(tr("&Clear Pixmap"), this);
    clearPixmapAct->setShortcut(tr("Ctrl+L"));
    connect(clearPixmapAct, SIGNAL(triggered()), this, SLOT(clearPixmap()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setIcon(QIcon(":/resource/imgs/exit.png"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(renderIntoPixmapAct);
    fileMenu->addAction(grabFrameBufferAct);
    fileMenu->addAction(clearPixmapAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    controlTool = addToolBar(tr("&controlTool"));
    controlTool->addAction(controlPanelAction);
    controlTool->addSeparator();
    controlTool->addAction(exitAct);

    modelTool = addToolBar(tr("&modelTool"));
    modelTool->addAction(modelPanelAction);
    modelTool->addAction(positionPanelAction);
}

void MainWindow::createContextMenu()
{
    ;
}
void MainWindow::createStatusBar()
{

    ;
}

void MainWindow::showPositionPanel()
{
    if (!positionPanelUI) {
        positionPanelUI = new position(this);
        //connect(positionPanelUI, SIGNAL(modelChanged()), glWidget, SLOT(setNode()));
    }
    if (positionPanelUI->isHidden()) {
        positionPanelUI->show();
    } else {
        positionPanelUI->activateWindow();
    }
}

void MainWindow::showModelPanel()
{
    if (!modelPanelUI) {
        modelPanelUI = new modelTree(glWidget->getNode(), this);
        connect(modelPanelUI, SIGNAL(modelChanged()), glWidget, SLOT(setNode()));
    }
    if (modelPanelUI->isHidden()) {
        modelPanelUI->show();
    } else {
        modelPanelUI->activateWindow();
    }
}
void MainWindow::showControlPanel()
{

    if (!controlPanelUI) {
        controlPanelUI = new robotcontrolpanel(this);
        #if 0
        connect(controlPanelUI, SIGNAL (findNext(const QString &,
                                            Qt::CaseSensitivity)),
                spreadsheet, SLOT (findNext(const QString &,
                                            Qt::CaseSensitivity)));
        connect(controlPanelUI, SIGNAL(findPrevious(const QString &,
                                                Qt::CaseSensitivity)),
                 spreadsheet, SLOT(findPrevious(const QString &,
                                                Qt::CaseSensitivity)));
        #endif
    }
    if (controlPanelUI->isHidden()) {
        controlPanelUI->show();
    } else {
        controlPanelUI->activateWindow();
    }

}
QSlider *MainWindow::createSlider(const char *changedSignal,
                                  const char *setterSlot)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    connect(slider, SIGNAL(valueChanged(int)), glWidget, setterSlot);
    connect(glWidget, changedSignal, slider, SLOT(setValue(int)));
    return slider;
}

void MainWindow::setPixmap(const QPixmap &pixmap)
{
    pixmapLabel->setPixmap(pixmap);
    QSize size = pixmap.size();
    if (size - QSize(1, 0) == pixmapLabelArea->maximumViewportSize())
        size -= QSize(1, 0);
    pixmapLabel->resize(size);
}

QSize MainWindow::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Grabber"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(glWidget->width())
                                                      .arg(glWidget->height()),
                                         &ok);
    if (!ok)
        return QSize();

    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }

    return glWidget->size();
}
