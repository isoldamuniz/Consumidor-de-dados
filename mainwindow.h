#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <vector>
#include "plotter.h"

namespace Ui {
class MainWindow;
}

/**
 * A classe MainWindow é usada para desenvolver a prática de programação orientada a objetos
 * por meio de um sistema de aquisição e supervisão de dados utilizando a comunicação TCP/IP.
 * Nessa classe, foi criado um consumidor de dados que se conecta a uma máquina e produz um gráfico com as informações
 * coletadas de um servidor.
 */

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void tcpConnect();
  void getData();
  void tcpDisconnect();
  void updateIP();
  void iniciaTimer();
  void timerEvent(QTimerEvent *e);
  void stop();
private:
  Ui::MainWindow *ui;
  QTcpSocket *socket;
  int tempo;
};

#endif // MAINWINDOW_H
