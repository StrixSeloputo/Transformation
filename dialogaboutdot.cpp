
#include "dialogaboutdot.h"
#include "ui_dialogaboutdot.h"
#include "point.h"

DialogAboutDot::DialogAboutDot(QWidget *parent) :
    QDialog(parent)
    ,_ui(new Ui::DialogAboutDot)
{
    _ui->setupUi(this);
    _ui->messageLabel->setText(tr("Не введено ни одной точки. Отображение идентичное"));
    QObject::connect(_ui->enterButton, SIGNAL(clicked()), this, SLOT(enterReferencePoint()));
}
DialogAboutDot::~DialogAboutDot()
{
    delete _ui;
}

void            DialogAboutDot::accept()
{
    emit acceptEnterRefPoint(this);
    close();
}

void            DialogAboutDot::enterReferencePoint()
{
    bool indicator = _ui->checkBox->checkState();
    QString src = _ui->sourceLineEdit->text();
    QStringList srcArgs = src.split(QChar(' '));
    if ((srcArgs.length()<ORD) || (indicator && srcArgs.length()<2*ORD)) {
        _ui->messageLabel->setText(tr("Мало координат, что-то пропустили"));
        return;
    }
    long X[ORD], U[ORD];
    bool ok = true;
    for (int i = 0; i < ORD; i++)
        X[i] = srcArgs.takeFirst().toLong(&ok, 10);
    if (!indicator) {
        srcArgs = _ui->distLineEdit->text().split(QChar(' '));
        if (srcArgs.length()<ORD) {
            _ui->messageLabel->setText(tr("Мало координат, что-то пропустили"));
            return;
        }
    }
    for (int i = 0; i < ORD; i++)
        U[i] = srcArgs.takeFirst().toLong(&ok, 10);
    _ui->sourceLineEdit->clear();
    _ui->distLineEdit->clear();
    Point p_x(X), p_u(U);
    _outSrc.append(p_x);
    _outDst.append(p_u);
    switch(_outSrc.length()) {
    case 0:
        _ui->messageLabel->setText(tr("Не введено ни одной точки. Отображение идентичное"));
        break;
    case 1:
        _ui->messageLabel->setText(tr("Введена одна точка. Отображение - сдвиг"));
        break;
    case 2:
        _ui->messageLabel->setText(tr("Введено две точки. Отображение - сдвиг + растяжение"));
        break;
    case 3:
        _ui->messageLabel->setText(tr("Введено три точки. Отображение - аффинное (точное)"));
        break;
    default:
        _ui->messageLabel->setText(tr("Введено больше трех точек. Отображение - усредненное аффинное"));
    }
}
QList<Point>    DialogAboutDot::getListSrc()
{
    return _outSrc;
}
QList<Point>    DialogAboutDot::getListDst()
{
    return _outDst;
}

