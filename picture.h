#ifndef PICTURE_H
#define PICTURE_H

//#include <typeinfo.h>

class QImage;

class MatrixOfTransformation;
class Point;

class Picture
{
public:

    Picture(QImage *image = 0, MatrixOfTransformation *matr = 0);
    ~Picture();
    long Width();
    long Height();
    void getNewImage(Image *dst);
    void setMatrix(MatrixOfTransformation *m);
    void setTestMatrix(double *U);
    void setImage(QImage img);
    QString showMatrix();
    QImage  getImage();
private:
    long    index(long x, long y);
    long    index(Point *p);
    long    indexOnNew(long x, long y);
    void    getNewScale();
    void    DrawPixelLikeThis(long x, long y, unsigned char *pix);
    unsigned char *pix(long x, long y);

private:
    bool    needTransform;
    long     width;
    long     height;
    long    minNewX;
    long    maxNewX;
    long    minNewY;
    long    maxNewY;
    long    *count;
    unsigned char   *pixs;
    unsigned char   *newPix;
    MatrixOfTransformation *matrix;
};

#endif // PICTURE_H
