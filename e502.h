#ifndef E502_H
#define E502_H

#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QTimer>

#define SIGNATURE 0x314C5443

#define ETHERR_PACKET_LEN (1)

#define ETHRETCODE_OK (0)
#define ETHRETCODE_ERROR (1)


#define E502_TCP_DEFAULT_CMD_PORT   (11114)
#define E502_TCP_DEFAULT_DATA_PORT  (11115)


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

typedef enum {
    E502_CM4_CMD_GET_MODULE_NAME        = 11,
    E502_CM4_CMD_GET_USB_SPEED          = 6,

    E502_CM4_CMD_BOOT                   = 0x0F,
    E502_CM4_CMD_FPGA_REG_READ          = 0x10,
    E502_CM4_CMD_FPGA_REG_WRITE         = 0x11,
    E502_CM4_CMD_STREAM_START           = 0x12,
    E502_CM4_CMD_STREAM_STOP            = 0x13,
    E502_CM4_CMD_STREAM_SET_STEP        = 0x14,
    E502_CM4_CMD_STREAM_IS_RUNNING      = 0x15,
    E502_CM4_CMD_FIRM_BUF_WRITE         = 0x16,
    E502_CM4_CMD_FLASH_RD               = 0x17,
    E502_CM4_CMD_FLASH_WR               = 0x18,
    E502_CM4_CMD_FLASH_ERASE            = 0x19,
    E502_CM4_CMD_FLASH_SET_PROTECT      = 0x1A,
    E502_CM4_CMD_FLASH_RELOAD_INFO      = 0x1B,
    E502_CM4_CMD_ETH_CFG_SET            = 0x1C,
    E502_CM4_CMD_ETH_CFG_GET            = 0x1D,
    E502_CM4_CMD_BF_MEM_WRITE           = 0x20,
    E502_CM4_CMD_BF_MEM_READ            = 0x21,
    E502_CM4_CMD_BF_FIRM_LOAD           = 0x22,
    E502_CM4_CMD_DROP_DATA_CON          = 0x23,
    E502_CM4_CMD_RELOAD_FPGA            = 0x24,
    E502_CM4_CMD_GET_DEVFLAGS           = 0x25,

    E502_CM4_CMD_OUT_CYCLE_LOAD         = 0x26,
    E502_CM4_CMD_OUT_CYCLE_SETUP        = 0x27,
    E502_CM4_CMD_OUT_CYCLE_STOP         = 0x28,
    E502_CM4_CMD_OUT_CYCLE_SETUP_CHECK  = 0x29,



    E502_CM4_CMD_TEST_START             = 0x40,
    E502_CM4_CMD_TEST_STOP              = 0x41,
    E502_CM4_CMD_TEST_GET_STATE         = 0x42,

    E502_CM4_CMD_GET_MODULE_INFO        = 0x80,
    E502_CM4_CMD_GET_MODULE_MODE        = 0x81,
    E502_CM4_CMD_GET_LAST_ERROR         = 0x82
} t_e502_cm4_cmd_codes;


typedef enum {
    /** Признак наличия двухканального канального ЦАП */
    X502_DEVFLAGS_DAC_PRESENT           = 0x00000001,
    /** Признак наличия гальваноразвязки */
    X502_DEVFLAGS_GAL_PRESENT           = 0x00000002,
    /** Признак наличия сигнального процессора BlackFin */
    X502_DEVFLAGS_BF_PRESENT            = 0x00000004,

    /** Признак, что устройство поддерживает интерфейс USB */
    X502_DEVFLAGS_IFACE_SUPPORT_USB     = 0x00000100,
    /** Признак, что устройство поддерживает Ethernet */
    X502_DEVFLAGS_IFACE_SUPPORT_ETH     = 0x00000200,
    /** Признак, что устройство поддерживает интерфейс PCI/PCI-Express */
    X502_DEVFLAGS_IFACE_SUPPORT_PCI     = 0x00000400,

    /** Признак, что устройство выполнено в индустриалном исполнении */
    X502_DEVFLAGS_INDUSTRIAL            = 0x00008000,

    /** Признак, что во Flash-памяти присутствует информация о модуле */
    X502_DEVFLAGS_FLASH_DATA_VALID      = 0x00010000,
    /** Признак, что во Flash-памяти присутствуют действительные калибровочные
        коэффициенты АЦП */
    X502_DEVFLAGS_FLASH_ADC_CALIBR_VALID = 0x00020000,
    /** Признак, что во Flash-памяти присутствуют действительные калибровочные
        коэффициенты ЦАП */
    X502_DEVFLAGS_FLASH_DAC_CALIBR_VALID = 0x00040000,

    /** Признак, что присутствует прошивка ПЛИС и она успешно была загружена */
    X502_DEVFLAGS_FPGA_LOADED            = 0x00800000,
    /** Признак, что устройство уже открыто (действителен только внутри t_x502_devrec) */
    X502_DEVFLAGS_DEVREC_OPENED          = 0x01000000
} t_x502_dev_flags;

typedef enum {
    X502_LCH_MODE_COMM = 1, /**< Измерение напряжения относительно общей земли */
    X502_LCH_MODE_DIFF = 0, /**< Дифференциальное измерение напряжения */
    X502_LCH_MODE_ZERO = 3  /**< Измерение собственного нуля */
} t_x502_lch_mode;


#define DESC_MODULE_NAME_SIZE          (16)

#define E502_CM4_DEVFLAGS (X502_DEVFLAGS_IFACE_SUPPORT_ETH | \
                            X502_DEVFLAGS_INDUSTRIAL | \
                            X502_DEVFLAGS_FPGA_LOADED)


#define E502_DEVICE_NAME "E502"
#define E502_DEVICE_SERIAL "2T222563"


#define LBOOT_DEVNAME_SIZE            32
#define LBOOT_SERIAL_SIZE             32
#define LBOOT_SOFTVER_SIZE            32
#define LBOOT_REVISION_SIZE           16
#define LBOOT_IMPLEMENTATION_SIZE     16
#define LBOOT_SPECINFO_SIZE           64



#define X502_REGS_BF_CTL_BLOCK               0
#define X502_REGS_BF_CTL                     (X502_REGS_BF_CTL_BLOCK+0)


#define X502_REGS_IOHARD_BLOCK                    0x0200//0x0000//
//Адрес Control Table
#define X502_REGS_IOHARD_LTABLE                   (X502_REGS_IOHARD_BLOCK+0)
#define X502_REGS_IOHARD_LTABLE_MAX_SIZE          0x100 // Максимальный размер Control Table

#define X502_REGS_IOHARD_LCH_CNT                  (X502_REGS_IOHARD_BLOCK+0x100)
#define X502_REGS_IOHARD_ADC_FREQ_DIV             (X502_REGS_IOHARD_BLOCK+0x102)
#define X502_REGS_IOHARD_ADC_FRAME_DELAY          (X502_REGS_IOHARD_BLOCK+0x104)
#define X502_REGS_IOHARD_DIGIN_FREQ_DIV           (X502_REGS_IOHARD_BLOCK+0x106)
#define X502_REGS_IOHARD_IO_MODE                  (X502_REGS_IOHARD_BLOCK+0x108)
#define X502_REGS_IOHARD_GO_SYNC_IO               (X502_REGS_IOHARD_BLOCK+0x10A)
#define X502_REGS_IOHARD_PRELOAD_ADC              (X502_REGS_IOHARD_BLOCK+0x10C)
#define X502_REGS_IOHARD_ASYNC_OUT                (X502_REGS_IOHARD_BLOCK+0x112)
#define X502_REGS_IOHARD_LED                      (X502_REGS_IOHARD_BLOCK+0x114)
#define X502_REGS_IOHARD_DIGIN_PULLUP             (X502_REGS_IOHARD_BLOCK+0x116)
#define X502_REGS_IOHARD_OUTSWAP_BFCTL            (X502_REGS_IOHARD_BLOCK+0x118)
#define X502_REGS_IOHARD_OUTSWAP_ERROR            (X502_REGS_IOHARD_BLOCK+0x120)


#define X502_REGBIT_ADC_SLV_CLK_LOCK_Pos      31
#define X502_REGBIT_ADC_SLV_CLK_LOCK_Msk      (0x1UL << X502_REGBIT_ADC_SLV_CLK_LOCK_Pos)


struct lboot_devinfo_st {
    char devname[LBOOT_DEVNAME_SIZE]; /**< название устройства */
    char serial[LBOOT_SERIAL_SIZE];   /**< серийный номер */
    char soft_ver[LBOOT_SOFTVER_SIZE]; /**< версия прошивки */
    char brd_revision[LBOOT_REVISION_SIZE]; /**< ревизия платы */
    char brd_impl[LBOOT_IMPLEMENTATION_SIZE]; /**< опции платы */
    char spec_info[LBOOT_SPECINFO_SIZE]; /**< резерв */
};
typedef struct lboot_devinfo_st t_lboot_devinfo;


struct t_e502_tcp_resp_hdr_st{
    quint32 sign;
    quint32 res;  //код возврата (0 - ОК, <0 - ошибка)
    quint32 len; //кол-во данных в ответе
} ;
typedef struct t_e502_tcp_resp_hdr_st t_e502_tcp_resp_hdr;


class E502  : public QObject
{
    Q_OBJECT
public:
    E502();
    ~E502();

public slots:
    void start();
    void stop();
private slots:


    void on_TestDataTimer_Timeout();
    void newuser_cmd();
    void newuser_data();
    void slotReadClient_cmd();
    void slotReadClient_data();

private:
    QTcpServer *tcpServer_cmd;
    QTcpServer *tcpServer_data;
    QTcpSocket* clientSocket_data;

    QTimer *TestDataTimer;

    int server_cmd_status;
    int server_data_status;
    QMap<int,QTcpSocket *> SClients_cmd;
    QMap<int,QTcpSocket *> SClients_data;
    quint32 devflags;
    t_lboot_devinfo devinfo;
    quint32 bf_ctl;
    quint32 clk_lock;
    quint8 stream_is_running;


    void sendCommand(QTcpSocket* clientSocket, quint32 cmd_code, quint32 len, const quint8 *data);
    void processCommand(QTcpSocket* clientSocket,QByteArray data);
};

#endif // E502_H
