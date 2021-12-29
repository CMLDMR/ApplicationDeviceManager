#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExpValidator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We validate the input data IP-addresses
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";

    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");

    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->lineEdit->setValidator(ipValidator);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    // We declare variables
    HANDLE hIcmpFile;                       // Handler
    unsigned long ipaddr = INADDR_NONE;     // Destination address
    DWORD dwRetVal = 0;                     // Number of replies
    char SendData[32] = "Data Buffer";      // The buffer data being sent
    LPVOID ReplyBuffer = NULL;              // buffer replies
    DWORD ReplySize = 0;                    // Buffer Size responses

    // Set the IP-address of the field qlineEdit
    ipaddr = inet_addr(ui->lineEdit->text().toStdString().c_str());
    hIcmpFile = IcmpCreateFile();   // create a handler

    // Select the buffer memory responses
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*) malloc(ReplySize);

    // Call the ICMP echo request function
    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
                NULL, ReplyBuffer, ReplySize, 1000);

    // We create a row in which we write the response message
    QString strMessage = "";

    if (dwRetVal != 0) {
        // The structure of the echo response
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;

        strMessage += "Sent icmp message to " + ui->lineEdit->text() + " ";
        if (dwRetVal > 1) {
            strMessage += "Received " + QString::number(dwRetVal) + " icmp message responses  ";
            strMessage += "Information from the first response: ";
        }
        else {
            strMessage += "Received " + QString::number(dwRetVal) + " icmp message response  ";
            strMessage += "Information from the first response: ";
        }
            strMessage += "Received from ";
            strMessage += inet_ntoa( ReplyAddr );
            strMessage += " ";
            strMessage += "Status = " + pEchoReply->Status;
            strMessage += "Roundtrip time = " + QString::number(pEchoReply->RoundTripTime) + " milliseconds \n";
    } else {
        strMessage += "Call to IcmpSendEcho failed.\n";
        strMessage += "IcmpSendEcho returned error: ";
        strMessage += QString::number(GetLastError());
    }


    qDebug() <<  strMessage;
//    ui->textEdit->setText(strMessage); // Display information about the received data
    free(ReplyBuffer); // frees memory
}

