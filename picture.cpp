
#include <iostream>
#include <math.h>

#include <QDateTime>
#include <QImage>

#include "image.h"
#include "matrixoftransformation.h"
#include "picture.h"
#include "point.h"

bool wasDrawing = false;
//////////////////////////
int sign (long x) {
    if (x == 0) return 0;
    return (x < 0) ? -1 : 1;
}
long max(long x, long y)
{
    return (x < y) ? y : x;
}
long min(long x, long y)
{
    return (x < y) ? x : y;
}

long min(long a, long b, long c, long d)
{
    return min(min(a,b),min(c,d));
}
long max(long a, long b, long c, long d)
{
    return max(max(a,b), max(c,d));
}
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
long Picture::Height()
{
    return height;
}
long Picture::Width()
{
    return width;
}

void Picture::setMatrix(MatrixOfTransformation *m)
{
    if (m->isEq(matrix)) return;
    needTransform = (pixs != NULL);
    //qDebug((needTransform ? "true" : "false"));
    delete matrix;
    matrix = m;
}
void Picture::setTestMatrix(double *U) {
    if (matrix->isEq(U)) return;
    needTransform = (pixs != NULL);
   // qDebug((needTransform ? "true" : "false"));
    matrix->setTestMatrix(U);
}
void Picture::setImage(QImage img)
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

//////////////////////////////////////////
QString Picture::showMatrix()
{
    if (matrix == 0) {
        return QString::asprintf("no matrix");
    }
    return matrix->showMatrix();
}

long Picture::index(long x, long y)
{
    return 3*(x+y*width);
}
long Picture::index(Point *p)
{
    return index(p->X(), p->Y());
}
////////////////////////////
QImage Picture::getImage()
{
    return QImage(pixs, width, height, QImage::Format_RGB888);
}

void Picture::getNewImage(Image *dst)
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
    int     di = sign(maxNewX - minNewX),
            dj = sign(maxNewY - minNewY);
    Point *M;
    qDebug("paint over");
    for (long i = minNewX; di*i < di*maxNewX; i+= di) {
        for (long j = minNewY; dj*j < dj*maxNewY; j+= dj) {
            //выясняем цвет исходной точки и красим в него
            M = invM->transformPoint(i, j, 0l);
            if ((M->X()>=0 && M->X()<width) && (M->Y()>=0 && M->Y()<height)) {
                DrawPixelLikeThis(i, j, pix(M->X(), M->Y()));
                countAck++;
            }
            delete M;
        }
    }
    qDebug("First part of Transformation is done. %ld / %ld", countAck, width*height);

    for (int i = 0; i < w*h; i++) {
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

unsigned char *Picture::pix(long x, long y)
{
    int i = index(x, y);
    if (i < 0)
        qDebug("point (%ld %ld) has index < 0", x, y);
    return pixs+index(x, y);
}

long Picture::indexOnNew(long x, long y)
{
    long w = abs(maxNewX-minNewX+1);
    return 3*(x-minNewX+(y-minNewY)*w);
}

void Picture::DrawPixelLikeThis(long x, long y, unsigned char *pix)
{
    long i = indexOnNew(x, y);
    long h = abs(maxNewY-minNewY+1);
    long w = abs(maxNewX-minNewX+1);
    if (i<0 || i > 3*w*h-2) {
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

//////////////////////////////////////////
void Picture::getNewScale()
{
    Point *M0, *M1, *M2, *M3;
    M0 = matrix->transformPoint(0l,0l, 0l);
    M1 = matrix->transformPoint(0l, height-1, 0l);
    M2 = matrix->transformPoint(width-1, 0l, 0l);
    M3 = matrix->transformPoint(width-1, height-1, 0l);

    minNewX = min(M0->X(), M1->X(), M2->X(), M3->X());
    maxNewX = max(M0->X(), M1->X(), M2->X(), M3->X());
    minNewY = min(M0->Y(), M1->Y(), M2->Y(), M3->Y());
    maxNewY = max(M0->Y(), M1->Y(), M2->Y(), M3->Y());
    qDebug("new diag: (%ld; %ld) <-> (%ld; %ld) old w & h: %ld %ld", minNewX, minNewY, maxNewX, maxNewY, width, height);
    qDebug("rigth");
    qDebug("(%ld %ld) - > (%ld %ld) %ld", 0l, 0l, M0->X(), M0->Y(), indexOnNew(M0->X(), M0->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", 0l, height-1, M1->X(), M1->Y(), indexOnNew(M1->X(), M1->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", width-1, 0l, M2->X(), M2->Y(), indexOnNew(M2->X(), M2->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", width-1, height-1, M3->X(), M3->Y(), indexOnNew(M3->X(), M3->Y()));

    Point *P0, *P1, *P2, *P3;
    MatrixOfTransformation *inverse = matrix->getInverseMatrix();
    qDebug("reverse");
    P0 = inverse->transformPoint(M0->X(), M0->Y(), M0->Z());
    P1 = inverse->transformPoint(M1->X(), M1->Y(), M1->Z());
    P2 = inverse->transformPoint(M2->X(), M2->Y(), M2->Z());
    P3 = inverse->transformPoint(M3->X(), M3->Y(), M3->Z());
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M0->X(), M0->Y(), P0->X(), P0->Y(), indexOnNew(P0->X(), P0->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M1->X(), M1->Y(), P1->X(), P1->Y(), indexOnNew(P1->X(), P1->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M2->X(), M2->Y(), P2->X(), P2->Y(), indexOnNew(P2->X(), P2->Y()));
    qDebug("(%ld %ld) - > (%ld %ld) %ld", M3->X(), M3->Y(), P3->X(), P3->Y(), indexOnNew(P3->X(), P3->Y()));
}
