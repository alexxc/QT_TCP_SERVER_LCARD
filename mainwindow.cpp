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

    TestDataTimer=new QTimer(this);
    TestDataTimer->setInterval(10);
    connect(TestDataTimer, SIGNAL(timeout()), this, SLOT(on_TestDataTimer_Timeout()));

    strcpy(devinfo.devname,E502_DEVICE_NAME);
    strcpy(devinfo.serial,E502_DEVICE_SERIAL);
//    strcpy(devinfo.soft_ver,"1");
//    strcpy(devinfo.brd_impl,"1");
//    strcpy(devinfo.brd_revision,"1");
//    strcpy(devinfo.spec_info,"1");

    devflags=(X502_DEVFLAGS_IFACE_SUPPORT_ETH | X502_DEVFLAGS_INDUSTRIAL | X502_DEVFLAGS_FPGA_LOADED);
    bf_ctl=0x0;
    clk_lock=X502_REGBIT_ADC_SLV_CLK_LOCK_Msk;
    stream_is_running=0x0;
}

MainWindow::~MainWindow()
{
    delete ui;
    server_cmd_status=0;
    server_data_status=0;
}

void MainWindow::on_starting_clicked()
{
    tcpServer_cmd = new QTcpServer(this);
    connect(tcpServer_cmd, SIGNAL(newConnection()), this, SLOT(newuser_cmd()));
    if (!tcpServer_cmd->listen(QHostAddress::Any, E502_TCP_DEFAULT_CMD_PORT) && server_cmd_status==0) {
        qDebug() <<  QObject::tr("Unable to start the cmd server: %1.").arg(tcpServer_cmd->errorString());
        ui->textinfo->append(tcpServer_cmd->errorString());
    } else {
        server_cmd_status=1;
        qDebug() << tcpServer_cmd->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Server cmd started!"));
        qDebug() << QString::fromUtf8("Server cmd started!");
    }

    //-----------------------------------------------------------------------------

    tcpServer_data = new QTcpServer(this);
    connect(tcpServer_data, SIGNAL(newConnection()), this, SLOT(newuser_data()));
    if (!tcpServer_data->listen(QHostAddress::Any, E502_TCP_DEFAULT_DATA_PORT) && server_data_status==0) {
        qDebug() <<  QObject::tr("Unable to start the data server: %1.").arg(tcpServer_data->errorString());
        ui->textinfo->append(tcpServer_data->errorString());
    } else {
        server_data_status=1;
        qDebug() << tcpServer_data->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Server data started!"));
        qDebug() << QString::fromUtf8("Server data started!");
    }
}

void MainWindow::on_stoping_clicked()
{
    if(server_cmd_status==1)
    {
        foreach(int i,SClients_cmd.keys()){
            SClients_cmd[i]->close();
            SClients_cmd.remove(i);
        }
        tcpServer_cmd->close();
        ui->textinfo->append(QString::fromUtf8("Server cmd stopped!"));
        qDebug() << QString::fromUtf8("Server cmd stopped!!");
        server_cmd_status=0;
    }

//-----------------------------------------------------------------
    if(server_data_status==1)
    {
        foreach(int i,SClients_data.keys()){
            SClients_data[i]->close();
            SClients_data.remove(i);
        }
        tcpServer_data->close();
        ui->textinfo->append(QString::fromUtf8("Server data stopped!"));
        qDebug() << QString::fromUtf8("Server data stopped!!");
        server_data_status=0;
    }
}


void MainWindow::newuser_cmd()
{
    if(server_cmd_status==1){
        qDebug() << QString::fromUtf8("New cmd connection!");
        ui->textinfo->append(QString::fromUtf8("New cmd connection!"));
        QTcpSocket* clientSocket=tcpServer_cmd->nextPendingConnection();
        int idusersocs=clientSocket->socketDescriptor();
        SClients_cmd[idusersocs]=clientSocket;
        connect(SClients_cmd[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient_cmd()));
    }
}

void MainWindow::newuser_data()
{
    if(server_data_status==1){
        qDebug() << QString::fromUtf8("New data connection!");
        ui->textinfo->append(QString::fromUtf8("New data connection!"));
        clientSocket_data=tcpServer_data->nextPendingConnection();
        int idusersocs=clientSocket_data->socketDescriptor();
        SClients_data[idusersocs]=clientSocket_data;
        connect(SClients_data[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient_data()));
        //TestDataTimer->start();
    }
}

void MainWindow::slotReadClient_cmd()
{
    QByteArray data;
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();

    data=clientSocket->readAll();
    ui->textinfo->append("ReadClient cmd:"+ data +"\n\r");

    processCommand(clientSocket,data);

    // Если нужно закрыть сокет
  //  clientSocket->close();
  //  SClients.remove(idusersocs);
}


void MainWindow::slotReadClient_data()
{
    QByteArray data;
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();

    data=clientSocket->readAll();
    ui->textinfo->append("ReadClient data:"+ data +"\n\r");

    //processCommand(clientSocket,data);

    // Если нужно закрыть сокет
  //  clientSocket->close();
  //  SClients.remove(idusersocs);
}


#define STREAM_IN_ADC_FLAG (0x80000000)
#define STREAM_IN_ADC_CHN(x) (((x)&0x0F)<<24)
#define STREAM_IN_ADC_MODE(x) (((x)&0x03)<<28)

void MainWindow::on_TestDataTimer_Timeout()
{
    quint32 test_data[8000];
    QDataStream sendStream(clientSocket_data);

    for(quint16 i =0;i<8000;i+=8)
    {

      for(quint8 k=0;k<8;k++)
      {
        test_data[i+k]=(((i+k)|STREAM_IN_ADC_CHN(k))| STREAM_IN_ADC_MODE(X502_LCH_MODE_COMM)|STREAM_IN_ADC_FLAG);
      }
      //test_data[i+1]=((i|STREAM_IN_ADC_CHN(1))| STREAM_IN_ADC_MODE(X502_LCH_MODE_COMM)|STREAM_IN_ADC_FLAG);
    }
    sendStream.writeRawData((const char *)test_data,sizeof(test_data));


//    for(quint16 i =0;i<10000;i++)
//    {
//      test_data[i]=((i|STREAM_IN_ADC_CHN(1))| STREAM_IN_ADC_MODE(X502_LCH_MODE_COMM)|STREAM_IN_ADC_FLAG);
//    }
//    sendStream.writeRawData((const char *)test_data,sizeof(test_data));
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
const quint8 null_buf[32]={0};

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
    {
        switch (command_buf[ ETHREQIDX_CMDPARAM ])
        {
            case X502_REGS_BF_CTL:
                sendCommand( clientSocket, ETHRETCODE_OK, sizeof(bf_ctl), (const quint8 *)&bf_ctl );
            break;

            case X502_REGS_IOHARD_IO_MODE:
                sendCommand( clientSocket, ETHRETCODE_OK, sizeof(clk_lock), (const quint8 *)&clk_lock );
            break;

            case X502_REGS_IOHARD_GO_SYNC_IO:
                sendCommand( clientSocket, ETHRETCODE_OK, sizeof(clk_lock), (const quint8 *)&clk_lock );
            break;

            case 0x10A:
                sendCommand( clientSocket, ETHRETCODE_OK, sizeof(clk_lock), (const quint8 *)&clk_lock );
            break;
        }
        break;

    }
    break;

    case E502_CM4_CMD_FPGA_REG_WRITE:
        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;

    case E502_CM4_CMD_STREAM_IS_RUNNING:
        sendCommand( clientSocket, ETHRETCODE_OK, sizeof(stream_is_running),( const quint8 *)&stream_is_running );
        break;

    case E502_CM4_CMD_STREAM_START:
        stream_is_running=1;
        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        TestDataTimer->start();
        break;

    case E502_CM4_CMD_STREAM_STOP:
        stream_is_running=0;
        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        TestDataTimer->stop();
        break;

    case E502_CM4_CMD_STREAM_SET_STEP:

        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;

    case E502_CM4_CMD_TEST_GET_STATE:


        sendCommand( clientSocket, ETHRETCODE_OK, 32, null_buf );
        break;


    case E502_CM4_CMD_DROP_DATA_CON:

        sendCommand( clientSocket, ETHRETCODE_OK, 0, NULL );
        break;

    }
}


