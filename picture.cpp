
#include <iostream>
#include <math.h>

#include <QDateTime>
#include <QImage>

#include "helpfunction.h"
#include "image.h"
#include "matrixoftransformation.h"
#include "picture.h"
#include "point.h"

bool wasDrawing = false;
//////////////////////////
Picture::Picture(QImage *image, MatrixOfTransformation *matr)
{
    if (image != 0) {
        setImage(*image);
    } else {
        width = height = 0;
        pixs = NULL;
        needTransform = false;
    }
    if (matr == 0) matr = new MatrixOfTransformation();
    matrix = matr;
    newPix = NULL;
    count = NULL;
    qDebug("Initialisation Picture");
    //showMatrix();
}
Picture::~Picture()
{
    delete [] pixs;
    if (matrix != 0) delete matrix;
}

void            Picture::DrawPixelLikeThis(long x, long y, unsigned char *pix)
{
    long i = indexOnNew(x, y);
    long h = abs(maxNewY-minNewY+1);
    long w = abs(maxNewX-minNewX+1);
    if (i < 0 || i > 3*w*h-2) {
        qDebug("Out of range");
        return;
    }
//    qDebug("Draw\t(%d %d %d) on (%ld; %ld) %ld", pix[0], pix[1], pix[2], x, y, i);
    for (int j = 0; j < 3; j++) {
        newPix[i+j] += pix[j];
    }
    count[i/3]++;
    wasDrawing = true;
}
QImage          Picture::getImage()
{
    return QImage(pixs, width, height, QImage::Format_RGB888);
}
void            Picture::getNewImage(Image *dst)
{
    if (!needTransform) return;
    needTransform = false;
    MatrixOfTransformation *invM = matrix->getInverseMatrix();
    dst->setNewMatrix(invM);
    getNewScale();
    long h = abs(maxNewY-minNewY+1);
    long w = abs(maxNewX-minNewX+1);
    newPix = new unsigned char[3*h*w];
    count = new long[h*w];
    for(int i = 0; i < 3*w*h; i++)
        newPix[i] = 0;
    long countAck = 0;
    int     di = HelpFunction::sign(maxNewX - minNewX),
            dj = HelpFunction::sign(maxNewY - minNewY);
    Point M;
    qDebug("paint over");
    for (long i = minNewX; di*i < di*maxNewX; i+= di)
    {
        for (long j = minNewY; dj*j < dj*maxNewY; j+= dj)
        {
            //выясняем цвет исходной точки и красим в него
            long t[] = { i, j, 0l };
            M = invM->transformPoint(t);
            if ((M.X(0)>=0 && M.X(0)<width) && (M.X(1)>=0 && M.X(1)<height))
            {
                DrawPixelLikeThis(i, j, pix(M.X(0), M.X(1)));
                countAck++;
            }
        }
    }
    qDebug("First part of Transformation is done. %ld / %ld", countAck, width*height);

    for (int i = 0; i < w*h; i++)
    {
        if (count[i] == 0) { break; }
        newPix[3*i] /= count[i];
        newPix[3*i+1] /= count[i];
        newPix[3*i+2] /= count[i];
    }
    qDebug("Second part of Transformation is done");
    QImage img = QImage(newPix, w, h, QImage::Format_RGB888);
    qDebug("Transformation ends");
    dst->setImage(img);

    QString str_matrix = matrix->showMatrix();
    str_matrix.replace(QRegExp("[\\s]+"), "_");
    QDateTime curdt = QDateTime::currentDateTime();
    img.save(str_matrix+"-"+curdt.toString(Qt::ISODate), "JPG");
    delete []count;
    delete []newPix;
}
void            Picture::getNewScale()
{
    Point M0, M1, M2, M3;
    long t[DIM] = { 0l };
    M0 = matrix->transformPoint(t);
    t[1] = height-1;
    M1 = matrix->transformPoint(t);
    t[0] = width-1;
    M3 = matrix->transformPoint(t);
    t[1] = 0l;
    M2 = matrix->transformPoint(t);

    minNewX = HelpFunction::min(M0.X(0), M1.X(0), M2.X(0), M3.X(0));
    maxNewX = HelpFunction::max(M0.X(0), M1.X(0), M2.X(0), M3.X(0));
    minNewY = HelpFunction::min(M0.X(1), M1.X(1), M2.X(1), M3.X(1));
    maxNewY = HelpFunction::max(M0.X(1), M1.X(1), M2.X(1), M3.X(1));
    qDebug("new diag: (%ld; %ld) <-> (%ld; %ld) old w & h: %ld %ld", minNewX, minNewY, maxNewX, maxNewY, width, height);
    qDebug("rigth");
    qDebug("(%ld %ld) - > (%ld %ld) %ld", 0l, 0l, M0.X(0), M0.X(1), indexOnNew(M0.X(0), M0.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", 0l, height-1, M1.X(0), M1.X(1), indexOnNew(M1.X(0), M1.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", width-1, 0l, M2.X(0), M2.X(1), indexOnNew(M2.X(0), M2.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", width-1, height-1, M3.X(0), M3.X(1), indexOnNew(M3.X(0), M3.X(1)));

    Point P0, P1, P2, P3;
    MatrixOfTransformation *inverse = matrix->getInverseMatrix();
    qDebug("reverse");
    P0 = inverse->transformPoint(M0);
    P1 = inverse->transformPoint(M1);
    P2 = inverse->transformPoint(M2);
    P3 = inverse->transformPoint(M3);
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M0.X(0), M0.X(1), P0.X(0), P0.X(1), indexOnNew(P0.X(0), P0.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M1.X(0), M1.X(1), P1.X(0), P1.X(1), indexOnNew(P1.X(0), P1.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M2.X(0), M2.X(1), P2.X(0), P2.X(1), indexOnNew(P2.X(0), P2.X(1)));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M3.X(0), M3.X(1), P3.X(0), P3.X(1), indexOnNew(P3.X(0), P3.X(1)));
}
long            Picture::Height()
{
    return height;
}
long            Picture::index(Point *p)
{
    return index(p->X(0), p->X(1));
}
long            Picture::index(long x, long y)
{
    return 3*(x+y*width);
}
long            Picture::indexOnNew(long x, long y)
{
    long w = abs(maxNewX-minNewX+1);
    return 3*(x-minNewX+(y-minNewY)*w);
}
unsigned char  *Picture::pix(long x, long y)
{
    int i = index(x, y);
    if (i < 0)
        qDebug("point (%ld %ld) has index < 0", x, y);
    return pixs+index(x, y);
}
void            Picture::setMatrix(MatrixOfTransformation *m)
{
    if (m->isEq(matrix)) return;
    needTransform = (pixs != NULL);
    //qDebug((needTransform ? "true" : "false"));
    delete matrix;
    matrix = m;
}
void            Picture::setTestMatrix(double *U) {
    if (matrix->isEq(U)) return;
    needTransform = (pixs != NULL);
   // qDebug((needTransform ? "true" : "false"));
    matrix->setMatrix(U);
}
void            Picture::setImage(QImage img)
{

    width = img.width();
    height = img.height();
    pixs = new unsigned char[3*width*height];
    for (int i = 0; i < 3*width*height; i++)
        pixs[i] = 0;
    for (long i = 0; i < width; i++) {
        for (long j = 0; j < height; j++) {
            long indx = index(i, j);
            QRgb rgb = img.pixel(i,j);    // 0xAARRGGBB
            pixs[indx] = (rgb & 0x00ff0000) >> 16;
            pixs[indx+1] = (rgb & 0x0000ff00) >> 8;
            pixs[indx+2] = (rgb & 0x000000ff);
        }
    }
    needTransform = matrix != NULL;
   // qDebug((needTransform ? "true" : "false"));
}
QString         Picture::showMatrix()
{
    if (matrix == 0) {
        return QString::asprintf("no matrix");
    }
    return matrix->showMatrix();
}
long            Picture::Width()
{
    return width;
}

