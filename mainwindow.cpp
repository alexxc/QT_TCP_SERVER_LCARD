#include "mainwindow.h"
#include "ui_mainwindow.h"


static qint32 f_cmdthread_run;
static qint32 f_dataconn;
static quint32 f_ethcommand[ 128 ];
static qint32 f_init_require;

//quint8 g_altera_info[ 510 ];

t_lboot_devinfo devinfo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    f_cmdthread_run = 0;
    f_dataconn = 0;
    f_init_require = 0;


//    for (quint16 i = 0; (i < 510); i += 2)
//     {
//        g_altera_info[ i ]   = 0x00;
//        g_altera_info[ i+1 ] = 0xE0;
//     }

    strcpy(devinfo.devname,E502_DEVICE_NAME);
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
//    int idusersocs=clientSocket->socketDescriptor();
 //   QTextStream os(clientSocket);
//


    data=clientSocket->readAll();
    processCommand(clientSocket,data);

    ui->textinfo->append("ReadClient:"+ data +"\n\r");
    // Если нужно закрыть сокет
    //clientSocket->close();
    //SClients.remove(idusersocs);
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
    e502_tcp_resp_hdr.sign = SIGNATURE;
    e502_tcp_resp_hdr.res  = cmd_code;
    e502_tcp_resp_hdr.len  = len;


    sendStream.writeBytes((const char *)&e502_tcp_resp_hdr,sizeof(e502_tcp_resp_hdr));
    if ((len > 0) && (data != NULL))
       sendStream.writeBytes((const char *)data,len);
}

void MainWindow::processCommand(QTcpSocket* clientSocket, QByteArray data)
{

quint32 *command_buf=(quint32*)data.data();
switch (command_buf[ ETHREQIDX_CMDCODE ])
    {
    case ETHCMD_INIT:
        if (f_dataconn)
            f_init_require = 1;
        sendCommand( clientSocket, ETHRETCODE_OK, sizeof(devinfo), (const quint8 *)&devinfo);
        break;
    case ETHCMD_NOP:
        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;
    case ETHCMD_GETNAME:
        sendCommand( clientSocket, ETHRETCODE_OK, LBOOT_DEVNAME_SIZE, (const quint8 *)devinfo.devname );
        break;
    case ETHCMD_GETARRAY:
//        f_get_array_cmd( clientSocket, command_buf[ ETHREQIDX_CMDPARAM ] >> 24,
//            command_buf[ ETHREQIDX_CMDPARAM ] & 0xFFFFFF, command_buf[ ETHREQIDX_REPLYLEN ] );

            sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;
    case ETHCMD_PUTARRAY:
//        f_put_array_cmd( clientSocket, command_buf[ ETHREQIDX_CMDPARAM ] >> 24,
//            command_buf[ ETHREQIDX_CMDPARAM ] & 0xFFFFFF, command_buf[ ETHREQIDX_DATALEN ] );
          sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;
    default:
        break;
    }
}
