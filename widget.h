#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QtNetwork/qtcpsocket.h>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QTcpSocket * client;
    QTimer* timer;

private slots:
    void on_pushButton_connect_clicked();
    void on_socket_connected();
    void on_socket_readyRead();

    void on_pushButton_send_clicked();

    void on_pushButton_disconnect_clicked();
    void on_timeout();


private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
