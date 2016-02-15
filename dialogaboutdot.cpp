#include "dialogaboutdot.h"
#include "ui_dialogaboutdot.h"
#include "point.h"

DialogAboutDot::DialogAboutDot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAboutDot)
{
    ui->setupUi(this);
    ui->messageLabel->setText(tr("Не введено ни одной точки. Отображение идентичное"));
    QObject::connect(ui->enterButton, SIGNAL(clicked()), this, SLOT(enterReferencePoint()));
}

DialogAboutDot::~DialogAboutDot()
{
    delete ui;
}

void DialogAboutDot::enterReferencePoint()
{
    bool indicator = ui->checkBox->checkState();
    QString src = ui->sourceLineEdit->text();
    QStringList srcArgs = src.split(QChar(' '));
    if ((srcArgs.length()<2) || (indicator && srcArgs.length()<4)) {
        ui->messageLabel->setText(tr("Мало координат, что-то пропустили"));
        return;
    }
    long x0, y0, x1, y1;
    bool ok = true;
    x0 = srcArgs.takeFirst().toLong(&ok, 10);
    y0 = srcArgs.takeFirst().toLong(&ok, 10);
    if (!indicator) {
        srcArgs = ui->distLineEdit->text().split(QChar(' '));
        if (srcArgs.length()<2) {
            ui->messageLabel->setText(tr("Мало координат, что-то пропустили"));
            return;
        }
    }
    x1 = srcArgs.takeFirst().toLong(&ok, 10);
    y1 = srcArgs.takeFirst().toLong(&ok, 10);
    ui->sourceLineEdit->clear();
    ui->distLineEdit->clear();
    outSrc.append(Point(x0, y0));
    outDst.append(Point(x1, y1));
    switch(outSrc.length()) {
    case 0:
        ui->messageLabel->setText(tr("Не введено ни одной точки. Отображение идентичное"));
        break;
    case 1:
        ui->messageLabel->setText(tr("Введена одна точка. Отображение - сдвиг"));
        break;
    case 2:
        ui->messageLabel->setText(tr("Введено две точки. Отображение - сдвиг + растяжение"));
        break;
    case 3:
        ui->messageLabel->setText(tr("Введено три точки. Отображение - аффинное (точное)"));
        break;
    default:
        ui->messageLabel->setText(tr("Введено больше трех точек. Отображение - усредненное аффинное"));
    }
}

QList<Point> DialogAboutDot::getListSrc()
{
    return outSrc;
}
QList<Point> DialogAboutDot::getListDst()
{
    return outDst;
}

void DialogAboutDot::accept()
{
    emit acceptEnterRefPoint(this);
    close();
}