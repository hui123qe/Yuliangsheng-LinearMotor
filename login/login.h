#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "ttools.h"
#include <QMessageBox>
#include "widget.h"
//#include "mainwindow.h"
#include <QDesktopWidget>
#include "tt_tools/ttools.h"

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();




private slots:
    void on_btnlogin_clicked();



private:
    Ui::login *ui;


    //设置可拖拽
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);
    QPoint dragPosition;
    bool isDragging = false;
};

#endif // LOGIN_H
