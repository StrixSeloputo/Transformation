#ifndef DIALOGABOUTDOT_H
#define DIALOGABOUTDOT_H

#include <QDialog>

class Point;

namespace Ui {
class DialogAboutDot;
}

class DialogAboutDot : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAboutDot(QWidget *parent = 0);
    ~DialogAboutDot();
    QList<Point> getListSrc();
    QList<Point> getListDst();
signals:
    void acceptEnterRefPoint(DialogAboutDot *);
private:
    Ui::DialogAboutDot *ui;
    QList<Point> outSrc;
    QList<Point> outDst;
private slots:
    void enterReferencePoint();
    void accept();
};

#endif // DIALOGABOUTDOT_H
