#ifndef DIALOGABOUTDOT_H
#define DIALOGABOUTDOT_H

#include <QDialog>

#include "constdef.h"

class Point;

namespace Ui {
class DialogAboutDot;
}

class DialogAboutDot : public QDialog
{
    Q_OBJECT

private:
    Ui::DialogAboutDot *_ui;
    QList<Point> _outSrc;
    QList<Point> _outDst;

public:
    explicit DialogAboutDot(QWidget *parent = 0);
    ~DialogAboutDot();
    QList<Point>    getListSrc();
    QList<Point>    getListDst();
signals:
    void            acceptEnterRefPoint(DialogAboutDot *);
private slots:
    void            enterReferencePoint();
    void            accept();
};

#endif // DIALOGABOUTDOT_H
