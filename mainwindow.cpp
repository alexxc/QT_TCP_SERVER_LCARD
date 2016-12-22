#include "mainwindow.h"
#include "ui_mainwindow.h"


QDataStream &operator<<(QDataStream &out, const t_e502_tcp_resp_hdr &tcp_resp_hdr)
{
    out <<tcp_resp_hdr.sign;
    out <<tcp_resp_hdr.res;
    out <<tcp_resp_hdr.len;

    return out;
}

QDataStream &operator<<(QDataStream &out, const t_lboot_devinfo &lboot_devinfo)
{
//    out.writeBytes(lboot_devinfo.devname,LBOOT_DEVNAME_SIZE);
//    out.writeBytes(lboot_devinfo.serial,LBOOT_SERIAL_SIZE);
//    out.writeBytes(lboot_devinfo.soft_ver,LBOOT_SOFTVER_SIZE);
//    out.writeBytes(lboot_devinfo.brd_revision,LBOOT_REVISION_SIZE);
//    out.writeBytes(lboot_devinfo.brd_impl,LBOOT_IMPLEMENTATION_SIZE);
//    out.writeBytes(lboot_devinfo.spec_info,LBOOT_SPECINFO_SIZE);

    out.writeRawData(lboot_devinfo.devname,LBOOT_DEVNAME_SIZE);
    out.writeRawData(lboot_devinfo.serial,LBOOT_SERIAL_SIZE);
    out.writeRawData(lboot_devinfo.soft_ver,LBOOT_SOFTVER_SIZE);
    out.writeRawData(lboot_devinfo.brd_revision,LBOOT_REVISION_SIZE);
    out.writeRawData(lboot_devinfo.brd_impl,LBOOT_IMPLEMENTATION_SIZE);
    out.writeRawData(lboot_devinfo.spec_info,LBOOT_SPECINFO_SIZE);

    return out;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    strcpy(devinfo.devname,E502_DEVICE_NAME);
    strcpy(devinfo.serial,E502_DEVICE_SERIAL);
//    strcpy(devinfo.soft_ver,"1");
//    strcpy(devinfo.brd_impl,"1");
//    strcpy(devinfo.brd_revision,"1");
//    strcpy(devinfo.spec_info,"1");

    devflags=(X502_DEVFLAGS_IFACE_SUPPORT_ETH | X502_DEVFLAGS_INDUSTRIAL | X502_DEVFLAGS_FPGA_LOADED);
    bf_ctl=0x0;
}

MainWindow::~MainWindow()
{
    delete ui;
    server_status=0;
}

void MainWindow::on_starting_clicked()
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newuser()));
    if (!tcpServer->listen(QHostAddress::Any, 11114) && server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(tcpServer->errorString());
        ui->textinfo->append(tcpServer->errorString());
    } else {
        server_status=1;
        qDebug() << tcpServer->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Сервер запущен!"));
        qDebug() << QString::fromUtf8("Сервер запущен!");
    }
}

void MainWindow::on_stoping_clicked()
{
    if(server_status==1){
        foreach(int i,SClients.keys()){
            QTextStream os(SClients[i]);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i]->close();
            SClients.remove(i);
        }
        tcpServer->close();
        ui->textinfo->append(QString::fromUtf8("Сервер остановлен!"));
        qDebug() << QString::fromUtf8("Сервер остановлен!");
        server_status=0;
    }
}


void MainWindow::newuser()
{
    if(server_status==1){
        qDebug() << QString::fromUtf8("У нас новое соединение!");
        ui->textinfo->append(QString::fromUtf8("У нас новое соединение!"));
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
    }
}

void MainWindow::slotReadClient()
{
    QByteArray data;
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();

    data=clientSocket->readAll();
    ui->textinfo->append("ReadClient:"+ data +"\n\r");

    processCommand(clientSocket,data);

    // Если нужно закрыть сокет
  //  clientSocket->close();
  //  SClients.remove(idusersocs);
}

void MainWindow::sendCommand(
QTcpSocket* clientSocket,                             /* (вх) - дескриптор сокета */
quint32 cmd_code,                      /* (вх) - код команды */
quint32 len,                                /* (вх) - длина передаваемых данных */
const quint8 *data                     /* (вх) - передаваемые данные */
)
{

    QDataStream sendStream(clientSocket);

    t_e502_tcp_resp_hdr e502_tcp_resp_hdr;
    sendStream.setByteOrder(QDataStream::LittleEndian);

    e502_tcp_resp_hdr.sign = SIGNATURE;
    e502_tcp_resp_hdr.res  = cmd_code;
    e502_tcp_resp_hdr.len  = len;

    sendStream<<e502_tcp_resp_hdr;

    if ((len > 0) && (data != NULL))
       sendStream.writeRawData((const char *)data,len);
}

void MainWindow::processCommand(QTcpSocket* clientSocket, QByteArray data)
{

quint32 *command_buf=(quint32*)data.data();
QByteArray devinfo_data;
QDataStream DevInfoStream(&devinfo_data,QIODevice::ReadWrite);
DevInfoStream.setByteOrder(QDataStream::LittleEndian);

switch (command_buf[ ETHREQIDX_CMDCODE ])
    {
    case E502_CM4_CMD_GET_MODULE_INFO:
        DevInfoStream<<devinfo;
        sendCommand( clientSocket, ETHRETCODE_OK, devinfo_data.size(), (const quint8 *)devinfo_data.data());
        break;

    case E502_CM4_CMD_GET_DEVFLAGS:
        sendCommand( clientSocket, ETHRETCODE_OK, sizeof(devflags), (const quint8 *)&devflags );
        break;

    case E502_CM4_CMD_GET_MODULE_NAME:
        sendCommand( clientSocket, ETHRETCODE_OK, LBOOT_DEVNAME_SIZE, (const quint8 *)devinfo.devname );
        break;

    case E502_CM4_CMD_FPGA_REG_READ:
        sendCommand( clientSocket, ETHRETCODE_OK, sizeof(bf_ctl), (const quint8 *)&bf_ctl );
        break;

    }
}


