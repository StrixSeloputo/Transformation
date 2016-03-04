
#include <QDateTime>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>

#include "dialogaboutdot.h"
#include "mainwindow.h"
#include "matrixoftransformation.h"
#include "picture.h"
#include "point.h"
#include "image.h"
#include "ui_image.h"

Image::Image(QWidget *parent, MainWindow *parentWindow, int flag) :
    QWidget(parent),
    ui(new Ui::Image)
{
    ui->setupUi(this);

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    imageScrollArea = new QScrollArea(this);
    imageScrollArea->setBackgroundRole(QPalette::Dark);
    imageScrollArea->setWidget(imageLabel);
    imageScrollArea->resize(650,700);
    if (parent == 0)
        switch(flag) {
        case ORIGINAL_IMAGE:
            setWindowTitle(tr("Conversion")); break;
        case TRANSFORMED_IMAGE:
            setWindowTitle(tr("Direct Conversion")); break;
        case TRANSFORMED_BACK_IMAGE:
            setWindowTitle(tr("Inverse Conversion. Corrected Image")); break;
        }
    indicator = flag;

    this->parent = parentWindow;
    pict = new Picture();
    resize(650, 650);
}
Image::~Image()
{
    delete ui;
    delete imageLabel;
    delete imageScrollArea;
}

void    Image::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
Image  *Image::getNewImage(Image *dst)
{
   pict->getNewImage(dst);
   return dst;
}
void    Image::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}
void    Image::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(imageScrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(imageScrollArea->verticalScrollBar(), factor);
}
void    Image::setImage(QImage &img)
{
    QPixmap pixmap = QPixmap::fromImage(img);
    pict->setImage(img);
    imageLabel->setPixmap(pixmap);
    scaleFactor = 1.0;
    normalSize();
    resize(img.width(), img.height());
}
void    Image::saveImage(QString flname)
{
    QImage img = pict->getImage();
    QDateTime curdt = QDateTime::currentDateTime();
    img.save(flname+"_"+curdt.toString(Qt::ISODate), "JPG");
}
void    Image::setNewMatrix(DialogAboutDot *dialog)
{
    QList<Point> src = dialog->getListSrc();
    QList<Point> dst = dialog->getListDst();
    qDebug("Получили списки точек (TI)");
    MatrixOfTransformation *matrix = new MatrixOfTransformation(src, dst);
    pict->setMatrix(matrix);
    matrix->showMatrix();
}
void    Image::setNewMatrix(MatrixOfTransformation *m)
{
    pict->setMatrix(m);
}
QString Image::showMatrix()
{
    return pict->showMatrix();
}
void    Image::setTestMatrix(double *U)
{
    pict->setTestMatrix(U);
}
void    Image::zoomIn() { scaleImage(1.25); }
void    Image::zoomOut() { scaleImage(0.8); }
