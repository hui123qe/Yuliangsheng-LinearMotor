#include "login.h"
#include "ui_login.h"
#include <QPixmap>


login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    connect(ui->btnexit, &QPushButton::clicked, this, &Widget::close);
    loginstate=new int(1);



    setWindowFlag(Qt::FramelessWindowHint);

    this->setWindowTitle("登录");
    ui->password->setEchoMode(QLineEdit::Password);
    ui->username->setCurrentIndex(1);





    QPixmap pixmap(":/image/sicarrier.png");
    if (pixmap.isNull()) {
        ui->label_2->setText("无法加载图片");
    } else{
        ui->label_2->setPixmap(pixmap);}
    ui->label_2->setScaledContents(true);


    QString globalStyle2 = R"(
/* 主窗口样式 */
QDialog {
    background-color: #f0f0f0; /* 背景色 */
}

/* 标签样式 */
QLabel {
    color: #333; /* 文字颜色 */
    font-size: 30px; /* 字体大小 */
}

/* 下拉框样式 */
QComboBox {
    background-color: #ffffff; /* 背景色 */
    border: 1px solid #ccc; /* 边框颜色 */
    border-radius: 5px; /* 边框圆角 */
    padding: 5px; /* 内边距 */
    font-size: 25px; /* 字体大小 */
}

QComboBox::drop-down {
    border: 0px; /* 下拉按钮边框 */

}

QComboBox::down-arrow {
    image: url(:/icons/down_arrow.png); /* 下拉箭头图标 */
    width: 12px;
    height: 12px;

}

/* 输入框样式 */
QLineEdit {
    background-color: #ffffff; /* 背景色 */
    border: 1px solid #ccc; /* 边框颜色 */
    border-radius: 5px; /* 边框圆角 */
    padding: 5px; /* 内边距 */
    font-size: 15px; /* 字体大小 */
}

/* 按钮样式 */
QPushButton {
    background-color: #0078d7; /* 背景色 */
    color: #ffffff; /* 文字颜色 */
    border: none; /* 无边框 */
    border-radius: 10px; /* 边框圆角 */
    padding: 10px; /* 内边距 */
    font-size: 25px; /* 字体大小 */
}

QPushButton:hover {
    background-color: #005ea7; /* 鼠标悬停时的背景色 */
}

QPushButton:pressed {
    background-color: #004a80; /* 按钮按下时的背景色 */
}

/* 按钮聚焦样式（可选） */
QPushButton:focus {
    outline: none; /* 去掉默认的焦点边框 */
    border: 2px solid #005ea7; /* 添加自定义焦点边框 */
}

/* 水平布局和垂直布局的间距 */
QHBoxLayout, QVBoxLayout {
    spacing: 10px; /* 控件之间的间距 */
}

/* 空白区域的样式 */
QSpacerItem {
    background-color: transparent; /* 空白区域透明 */
}
)";


    this->setStyleSheet(globalStyle2);
}

login::~login()
{
    delete ui;
}

void login::on_btnlogin_clicked()
{
    QString passwordAdmin;
    QString passwordTester;
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/password.txt";

    // 检查文件是否存在
    QFile file(filePath);
    if (!file.exists())
    {
        QMessageBox::warning(this,"警告","密码文件丢失!!!");
    }
    else
    {

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open file:" << filePath;
            return;
        }

        QTextStream in(&file);
        QString line;


        while ((line = in.readLine()).isNull() == false)
        {
            if (line.startsWith("password_Administrator"))
            {
                passwordAdmin = line.section('=', 1, 1).trimmed();
            }
            else if (line.startsWith("password_Tester"))
            {
                passwordTester = line.section('=', 1, 1).trimmed();
            }
        }

        file.close();

        qDebug()<<passwordAdmin;
        qDebug()<<passwordTester;
        qDebug()<<ui->password->text();


        if(ui->username->currentIndex() == 1 && ui->password->text() == passwordAdmin){
            *loginstate=1;
            this->close();
            //delete this;
            Widget *h=new Widget;
            h->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
            h->setAttribute(Qt::WA_DeleteOnClose);
            h->setGeometry(0, 0, 1920, 1080);
            h->show();
        }
        else if(ui->username->currentIndex() == 0 && ui->password->text() == passwordTester)
        {

            *loginstate=0;
            this->close();
            //delete this;
            Widget *h=new Widget;
            h->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
            h->setAttribute(Qt::WA_DeleteOnClose);
            h->setGeometry(0, 0, 1920, 1080);
            h->show();
            //h->showMaximized();
        }
        else{
            QMessageBox::warning(this,tr("登录失败"),tr("密码输入错误！"),QMessageBox::Ok);
            //ui->username->clear();
            ui->password->clear();
            //ui->username->setFocus();
        }
    }


}




void login::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QWidget *widget = childAt(event->pos());
        if (widget && widget->inherits("QComboBox")) {
            QDialog::mousePressEvent(event);
        } else {
            dragPosition = event->globalPos() - frameGeometry().topLeft();
            isDragging = true;
        }
    }
}

void login::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && isDragging) {
        move(event->globalPos() - dragPosition);
    } else {
        QDialog::mouseMoveEvent(event);
    }
}

void login::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
    QDialog::mouseReleaseEvent(event);
}
