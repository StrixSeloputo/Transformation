
#include <QtGui>
#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>

#include "dialogaboutdot.h"
#include "dialogabouttestmatrix.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image.h"
#include "point.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centralW = new QWidget;
    orignW = new QWidget;
    transW = new QWidget;
    qDebug("original");
    original = new Image(orignW, this);
    qDebug("transformed");
    transformed = new Image(transW, this);
    imageLayout = new QHBoxLayout;
    imageLayout->addWidget(orignW);
    imageLayout->addWidget(transW);
    centralW->setLayout(imageLayout);

    setCentralWidget(centralW);

    createActions();
    createMenus();

    setWindowTitle(tr("Transformation"));
    resize(1350, 700);

    connect(this, SIGNAL(saveImages(QString)), transformed, SLOT(saveImage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete original;
    delete transformed;
    delete orignW;
    delete transW;
    delete imageLayout;
    delete centralW;

    delete enterRefPointAct;
    delete exitAct;
    delete showMatrixAct;
    delete transformAct;

    delete fileMenu;
    delete transformMenu;
}

void MainWindow::enterRefPoint()
{
    DialogAboutDot *dialog = new DialogAboutDot();
    connect(dialog, SIGNAL(acceptEnterRefPoint(DialogAboutDot*)), original, SLOT(setNewMatrix(DialogAboutDot*)));
    dialog->show();
}
void MainWindow::createActions()
{
    enterRefPointAct = new QAction(tr("Enter Reference Point"), this);
    enterRefPointAct->setShortcut(tr("Ctrl+P"));
    connect(enterRefPointAct, SIGNAL(triggered()), this, SLOT(enterRefPoint()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(tr("Esc, Alt+F12"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveTransfImgAct = new QAction(tr("Save Transformed Image"), this);
    saveTransfImgAct->setShortcut(tr("Ctrl+S"));
    saveTransfImgAct->setEnabled(false);
    connect(saveTransfImgAct, SIGNAL(triggered()), this, SLOT(saveTransfImg()));

    showMatrixAct = new QAction(tr("Show Matrix"), this);
    showMatrixAct->setShortcut(tr("Ctrl+M"));
    connect(showMatrixAct, SIGNAL(triggered()), this, SLOT(showMatrix()));

    transformAct = new QAction(tr("&Tranform"), this);
    transformAct->setShortcut(tr("Ctrl+T"));
    connect(transformAct, SIGNAL(triggered()), this, SLOT(transform()));

    testMatrixAct = new QAction(tr("Set Test Matrix"), this);
    testMatrixAct->setShortcut(tr("Ctrl+E"));
    connect(testMatrixAct, SIGNAL(triggered()), this, SLOT(setTestMatrix()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
}
void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveTransfImgAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    transformMenu = new QMenu(tr("Transformation"), this);
    transformMenu->addAction(transformAct);
    transformMenu->addSeparator();
    transformMenu->addAction(enterRefPointAct);
    transformMenu->addAction(testMatrixAct);
    transformMenu->addAction(showMatrixAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(transformMenu);
}
void MainWindow::normalSize()
{
    original->normalSize();
    original->adjustSize();
    transformed->normalSize();
    transformed->adjustSize();
    scaleFactor = 1.0;
}
void MainWindow::open()
{
    imgfilename = QFileDialog::getOpenFileName(
                this, tr("Open Document"), QDir::currentPath(),
                tr("Image files (*.jpg *.jpeg *.bmp);;Exotic Image (*.bmp *.png *.gif);;All files (*.*)"),
                0, QFileDialog::DontUseNativeDialog
            );
    if (!imgfilename.isEmpty()) {
        QImage image(imgfilename);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(imgfilename));
            return;
        }
        original->setImage(image);

        scaleFactor = 1.0;

        normalSizeAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
    }
}
void MainWindow::saveTransfImg()
{
    QString str_matrix = original->showMatrix();
    str_matrix.replace(QRegExp("[^\\w]+"), "_");
    emit saveImages(imgfilename+str_matrix);
}
void MainWindow::setTestMatrix()
{
    DialogAboutTestMatrix *dialog = new DialogAboutTestMatrix();
    connect(dialog, SIGNAL(readyTestMatrix(double *)), original, SLOT(setTestMatrix(double *)));
    dialog->show();
}
void MainWindow::showMatrix()
{
    QString info_about_matrix1 = original->showMatrix();
    QString info_about_matrix2 = transformed->showMatrix();
    QMessageBox::about(0, tr("Info About Matrix Of Transformation"),
        tr("First matrix: \n").append(info_about_matrix1).append(tr("\nSecond matrix:\n")).append(info_about_matrix2));
}
void MainWindow::transform()
{
    original->getNewImage(transformed);
    saveTransfImgAct->setEnabled(true);
}
void MainWindow::zoomIn()
{
    original->zoomIn();
    transformed->zoomIn();
}
void MainWindow::zoomOut()
{
    original->zoomOut();
    transformed->zoomOut();
}
