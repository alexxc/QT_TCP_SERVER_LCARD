#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QDebug>


#define SIGNATURE 0x314C5443

#define ETHERR_PACKET_LEN (1)

#define ETHRETCODE_OK (0)
#define ETHRETCODE_ERROR (1)

#define LTRETH_CMD_PORT (11113)
#define LTRETH_DATA_PORT (11110)
#define ETH_COMMAND_LEN (16)
#define ETH_HEADER_LEN (20)

#define ETHREQIDX_SIGNATURE (0)
#define ETHREQIDX_CMDCODE   (1)
#define ETHREQIDX_CMDPARAM  (2)
#define ETHREQIDX_DATALEN   (3)
#define ETHREQIDX_REPLYLEN  (4)
#define ETHREQIDX_STARTDATA (5)


#define ETHCMD_TASK_PRIO (5)
#define ETHERDATARECV_TASK_PRIO (8)

#define ETHERR_PACKET_LEN (1)

#define ETHRETCODE_OK (0)
#define ETHRETCODE_ERROR (1)

#define ETHCMD_NOP (0)
#define ETHCMD_GETNAME (0x00000001)
#define ETHCMD_GETARRAY (0x00000002)
#define ETHCMD_PUTARRAY (0x00000003)
#define ETHCMD_INIT (0x00000080)//(0x80000000)


#define DESC_MODULE_NAME_SIZE          (16)

#define E502_CM4_DEVFLAGS (X502_DEVFLAGS_IFACE_SUPPORT_ETH | \
                            X502_DEVFLAGS_INDUSTRIAL | \
                            X502_DEVFLAGS_FPGA_LOADED)


#define E502_DEVICE_NAME "E502"


#define LBOOT_DEVNAME_SIZE            32
#define LBOOT_SERIAL_SIZE             32
#define LBOOT_SOFTVER_SIZE            32
#define LBOOT_REVISION_SIZE           16
#define LBOOT_IMPLEMENTATION_SIZE     16
#define LBOOT_SPECINFO_SIZE           64


#pragma pack(push, 1)
struct lboot_devinfo_st {
    char devname[LBOOT_DEVNAME_SIZE]; /**< название устройства */
    char serial[LBOOT_SERIAL_SIZE];   /**< серийный номер */
    char soft_ver[LBOOT_SOFTVER_SIZE]; /**< версия прошивки */
    char brd_revision[LBOOT_REVISION_SIZE]; /**< ревизия платы */
    char brd_impl[LBOOT_IMPLEMENTATION_SIZE]; /**< опции платы */
    char spec_info[LBOOT_SPECINFO_SIZE]; /**< резерв */
};
typedef struct lboot_devinfo_st t_lboot_devinfo;


typedef struct {
    quint32 sign;
    quint32 res;  //код возврата (0 - ОК, <0 - ошибка)
    quint32 len; //кол-во данных в ответе
} t_e502_tcp_resp_hdr;

#pragma pack(pop)

namespace Ui {
    class MainWindow;
}

class QTcpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_starting_clicked();
    void on_stoping_clicked();
    void newuser();
    void slotReadClient();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    int server_status;
    QMap<int,QTcpSocket *> SClients;



    void sendCommand(QTcpSocket* clientSocket, quint32 cmd_code, quint32 len, const quint8 *data);
    void processCommand(QTcpSocket* clientSocket,QByteArray data);
};

#endif // MAINWINDOW_H
