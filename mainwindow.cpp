#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <vector>
#include "plotter.h"


using namespace std;

/**
 * @brief MainWindow::MainWindow é a função construtora da janela e do soquete de rede.
 * Nela, os sinais e slots de cada objeto dessa classe são conectados.
 */

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  socket = new QTcpSocket(this);

  connect(ui->pushButtonConnect,
          SIGNAL(clicked(bool)),
          this,
          SLOT(tcpConnect()));

  connect(ui->pushButtonDisconnect,
          SIGNAL(clicked(bool)),
          this,
          SLOT(tcpDisconnect()));

  connect(ui->pushButtonUpdate,
          SIGNAL(clicked(bool)),
          this,
          SLOT(updateIP()));

  connect(ui->horizontalSliderTime,
          SIGNAL(valueChanged(int)),
          ui->lcdNumberTime,
          SLOT(display(int)));

  connect(ui->pushButtonStart,
          SIGNAL(clicked(bool)),
          this,
          SLOT(getData()));
}

/**
 * @brief MainWindow::tcpConnect é a função que faz a conexão a uma máquina
 * por meio de um IP selecionado da listwidget na porta 1234.
 */

void MainWindow::tcpConnect(){
    socket->connectToHost(ui->lineEditIP->text(),1234);
    if(socket->waitForConnected(3000)){
      qDebug() << "Connected";
    }
    else{
      qDebug() << "Disconnected";
    }
    QString str;
    if(socket->state() == QAbstractSocket::ConnectedState){
      if(socket->isOpen()){
        qDebug() << "reading...";
        socket->write("list");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        while(socket->bytesAvailable()){
          str = socket->readLine().replace("\n","").replace("\r","");
          ui->listWidget->addItem(str);
        }
      }
    }
}

/**
 * @brief MainWindow::tcpDisconnect é a função que encerra a conexão do IP informado.
 */

void MainWindow::tcpDisconnect(){
    socket->disconnectFromHost();
    if(socket->waitForDisconnected(3000) || socket->state() == QAbstractSocket::UnconnectedState){
        qDebug() << "Disconnected";
    }
}

/**
 * @brief MainWindow::updateIP é a função que atualiza a listas de IPs conectados a um servidor.
 */

void MainWindow::updateIP(){
    ui->listWidget->clear();
    QString str;
    if(socket->state() == QAbstractSocket::ConnectedState){
      if(socket->isOpen()){
        qDebug() << "reading...";
        socket->write("list");
        socket->waitForBytesWritten();
        socket->waitForReadyRead();
        while(socket->bytesAvailable()){
          str = socket->readLine().replace("\n","").replace("\r","");
          ui->listWidget->addItem(str);
        }
      }
    }
}

/**
 * @brief MainWindow::getData é a função que gerencia os dados que a serem plotados.
 */

void MainWindow::getData(){
  QString str, ip;
  QStringList list;
  QDateTime datetime;
  vector<float> tempo, dados;
  vector<qint64> tmp2;
  double tmp;
  qDebug() << "to get data...";
  if(socket->state() == QAbstractSocket::ConnectedState){
    if(socket->isOpen()){
       ip = ui->listWidget->currentItem()->text();
       str = "get " + ip;
      qDebug() << "reading...";
      socket->write(str.toStdString().c_str());
      socket->waitForBytesWritten();
      socket->waitForReadyRead();
      qDebug() << socket->bytesAvailable();
      while(socket->bytesAvailable()){
        str = socket->readLine().replace("\n","").replace("\r","");
        list = str.split(" ");
        if(list.size() == 2){
          datetime= QDateTime::fromString(list.at(0),Qt::ISODate);
          tmp = datetime.toMSecsSinceEpoch();
          tmp2.push_back(tmp);
          str = list.at(1);
          dados.push_back(str.toFloat());
          qDebug() << tmp << ": " << str;
        }
      }
      for (int i=0;i<tmp2.size();i++){
          tempo.push_back(tmp2[i]-tmp2[0]);
      }
    }
  }
  vector<float>::iterator maximodados, minimodados, maximotempo, minimotempo;
  vector<float> dadosnorm, temponorm;

  if(dados.size() > 30){
      dados.erase(dados.begin(),dados.end()-30);
      tempo.erase(tempo.begin(),tempo.end()-30);
  }

  maximodados = max_element(dados.begin(),dados.end());
  minimodados = min_element(dados.begin(),dados.end());

  for(int i=0; i<dados.size(); i++){
      tmp = (dados[i] - *minimodados)/(*maximodados-*minimodados);
      dadosnorm.push_back(tmp);
  }

  maximotempo = max_element(tempo.begin(),tempo.end());
  minimotempo = min_element(tempo.begin(),tempo.end());
  for(int i=0;i<tempo.size();i++){
      float tmp;
      tmp = (tempo[i] - *minimotempo)/(*maximotempo-*minimotempo);
      qDebug() << "tmp= " << tmp;
      temponorm.push_back(tmp);
  }
  ui->widgetPlotter->putData(dadosnorm,temponorm);
}

/**
 * @brief MainWindow::iniciaTimer é a função que inicia o temporizador escolhido pelo usuário.
 * Esse temporizador é usado para plotar dados a cada timeout.
 */

void MainWindow::iniciaTimer(){
    tempo = startTimer(ui->lcdNumberTime->intValue()*1000);
}

/**
 * @brief MainWindow::timerEvent é a função manipuladora de eventos de tempo.
*/

void MainWindow::timerEvent(QTimerEvent *e){
    getData();
}

/**
 * @brief MainWindow::stop é a função que deleta o temporizador escolhido pelo usuário.
 */

void MainWindow::stop(){
    killTimer(tempo);
}

/**
 * @brief MainWindow::~MainWindow é a função destrutora da janela e do soquete de rede TCP
 */

MainWindow::~MainWindow()
{
  delete socket;
  delete ui;
}
