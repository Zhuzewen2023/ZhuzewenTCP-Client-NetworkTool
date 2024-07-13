#include "widget.h"
#include "ui_widget.h"

#include <QAbstractSocket>
#include <QString>
#include <QMetaType>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextCharFormat>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);

    client = new QTcpSocket(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(5000);

    connect(client, &QAbstractSocket::connected, this, &Widget::on_socket_connected);
    connect(client, &QIODevice::readyRead, this, &Widget::on_socket_readyRead);

    connect(timer, &QTimer::timeout, this, &Widget::on_timeout);
    //connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbsractSocket::SocketError)));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_connect_clicked()
{
    timer->start();
    qDebug() << "on_pushButton_connect_clicked";
    client->connectToHost(ui->lineEdit_ipaddr->text(), ui->lineEdit_port->text().toInt());


    switch(client->state())
    {
           case QAbstractSocket::UnconnectedState:
               qDebug() << "Socket is unconnected";
               ui->textEdit_Receive->insertPlainText("socket未连接\n");
               break;
           case QAbstractSocket::ConnectingState:
               qDebug() << "Socket is connecting...";
              //  注意：这里的状态可能是暂时的，因为连接是异步的
               ui->textEdit_Receive->insertPlainText("socket正在连接\n");
               ui->pushButton_connect->setEnabled(false);
               ui->lineEdit_port->setEnabled(false);
               ui->lineEdit_ipaddr->setEnabled(false);
               ui->pushButton_disconnect->setEnabled(true);
               break;
           case QAbstractSocket::ConnectedState:
               qDebug() << "Socket is connected!";
               ui->textEdit_Receive->insertPlainText("socket已连接\n");
               // 连接成功后的操作
               break;
           case QAbstractSocket::BoundState:
               // 通常用于服务器套接字
               qDebug() << "Socket is bound to an address and port";
               ui->textEdit_Receive->insertPlainText("socket已绑定至一个地址和端口号\n");

               break;
           case QAbstractSocket::ListeningState:
               // 同样，通常用于服务器套接字
               qDebug() << "Socket is listening for incoming connections";
               ui->textEdit_Receive->insertPlainText("socket正在监听\n");

               break;
           case QAbstractSocket::ClosingState:
               qDebug() << "Socket is closing";
               ui->textEdit_Receive->insertPlainText("socket正在关闭\n");

               break;
           default:
               qDebug() << "Socket is in an unknown state";
               ui->textEdit_Receive->insertPlainText("socket处于未知状态\n");

               break;
    }

    QTextCursor cursor = ui->textEdit_Receive->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_Receive->setTextCursor(cursor);
}

void Widget::on_socket_connected()
{
    timer->stop();
    ui->textEdit_Receive->insertPlainText("服务器已连接\n");
    QTextCursor cursor = ui->textEdit_Receive->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_Receive->setTextCursor(cursor);
    ui->pushButton_send->setEnabled(true);
    ui->pushButton_connect->setEnabled(false);
    ui->lineEdit_port->setEnabled(false);
    ui->lineEdit_ipaddr->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
}

void Widget::on_socket_readyRead()
{
    QTextCursor cursor = ui->textEdit_Receive->textCursor();
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat format;
    format.setForeground(QBrush(QColor(Qt::black)));
    cursor.setCharFormat(format);
    ui->textEdit_Receive->setTextCursor(cursor);
    QByteArray byteArray = client->readAll();
    cursor.insertText(QString(byteArray));

    ui->textEdit_Receive->setTextCursor(cursor);
}


void Widget::on_pushButton_send_clicked()
{
    if(client->write(ui->textEdit_Send->toPlainText().toUtf8()) != -1)
    {
        //ui->textEdit_Receive->insertPlainText("发送成功: ");
        QTextCursor cursor = ui->textEdit_Receive->textCursor();
        cursor.movePosition(QTextCursor::End);

        QTextCharFormat format;
        format.setForeground(QBrush(QColor(Qt::red)));
        cursor.setCharFormat(format);

        ui->textEdit_Receive->setTextCursor(cursor);
        cursor.insertText("发送成功: " + ui->textEdit_Send->toPlainText() + "\n");
        format.setForeground(QBrush(QColor(Qt::black)));
        cursor.setCharFormat(format);
        ui->textEdit_Receive->setTextCursor(cursor);
    }
    else
    {
        ui->textEdit_Receive->insertPlainText("发送失败\n");
    }

}

void Widget::on_pushButton_disconnect_clicked()
{
    client->close();
    ui->textEdit_Receive->insertPlainText("断开连接\n");
    QTextCursor cursor = ui->textEdit_Receive->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_Receive->setTextCursor(cursor);
    ui->pushButton_send->setEnabled(false);
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->lineEdit_port->setEnabled(true);
    ui->lineEdit_ipaddr->setEnabled(true);
}

void Widget::on_timeout()
{
    ui->textEdit_Receive->insertPlainText("连接超时\n");
    ui->pushButton_disconnect->click();
}


