#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "point.h"

#define ORIGINAL_IMAGE          0x0
#define TRANSFORMED_IMAGE       0x1
#define TRANSFORMED_BACK_IMAGE  0x2

#define SYMMETRY_NO             0x10
#define SYMMETRY_OX             0x11
#define SYMMETRY_OY             0x12

class QAction;
class QLabel;
class QHBoxLayout;
class QMenu;
class QScrollArea;
class QScrollBar;
class QWidget;
class DialogAboutDot;
class Image;
//class Point;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    double scaleFactor;
    Ui::MainWindow *ui;
//////////
    QString imgfilename;
    QWidget *centralW;
    QWidget *orignW;
    QWidget *transW;
    QHBoxLayout *imageLayout;

    QAction *enterRefPointAct;
    QAction *exitAct;
    QAction *normalSizeAct;
    QAction *openAct;
    QAction *saveTransfImgAct;
    QAction *showMatrixAct;
    QAction *testMatrixAct;
    QAction *transformAct;
    QAction *transformBackAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *transformMenu;

    Image *original;
    Image *transformed;
//////////////
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
/////////////
signals:
    void homothetySignal(double ratio);
    void saveImages(QString srcimgflname);
    void symmetrySignal(int code);
    void translationSignal(Point vector);
    void turningSignal(double angle);
public slots:
    void enterRefPoint();
    void normalSize();
    void open();
    void saveTransfImg();
    void setTestMatrix();
    void showMatrix();
    void transform();
    void zoomIn();
    void zoomOut();
};

#endif // MAINWINDOW_H
