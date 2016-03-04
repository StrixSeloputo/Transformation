#ifndef IMAGE_H
#define IMAGE_H

#include <QMainWindow>

#include "constdef.h"

class QLabel;
class QScrollArea;
class QScrollBar;
class QWidget;

class DialogAboutDot;
class MainWindow;
class MatrixOfTransformation;
class Picture;

namespace Ui {
class Image;
}

class Image : public QWidget
{
    Q_OBJECT

public:
    explicit Image(QWidget *parent = 0, MainWindow *parentWindow = 0, int flag = ORIGINAL_IMAGE);
    ~Image();
    void setImage(QImage &img);
    Image *getNewImage(Image *dst);
    QString showMatrix();

private:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    double  scaleFactor;
    int     indicator;

    Ui::Image *ui;
    QLabel *imageLabel;
    QScrollArea * imageScrollArea;

    MainWindow *parent;
    Picture *pict;

public slots:
    void normalSize();
    void saveImage(QString flname);
    void setNewMatrix(DialogAboutDot *dialog);
    void setNewMatrix(MatrixOfTransformation *m);
    void setTestMatrix(double *U);
    void zoomIn();
    void zoomOut();
};

#endif // IMAGE_H
