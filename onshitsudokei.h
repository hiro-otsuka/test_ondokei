#ifndef ONSHITSUDOKEI_H
#define ONSHITSUDOKEI_H

#include <QMainWindow>
#include <QTimer>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qmqtt.h>

namespace Ui {
  class Onshitsudokei;
}

class Onshitsudokei : public QMainWindow
{
  Q_OBJECT

public:
  explicit Onshitsudokei(QWidget *parent = 0);
  ~Onshitsudokei();

private slots:
  void onConnected();
  void onReceived(const QMQTT::Message& message);
  void on_btnExit_clicked();

private:
  static const int GRAPH_MAX_X = 240;
  static const int AXIS_ONDO_MAX = 40;
  static const int AXIS_ONDO_MIN = -20;
  static const int AXIS_KIATSU_MAX = 1200;
  static const int AXIS_KIATSU_MIN = 900;
  static const int GRAPH_UPDATE_INTERVAL = 60-1;

  Ui::Onshitsudokei *ui;
  QMQTT::Client *subscriber;
  QTimer *timer;
  QwtPlotGrid *p_grid;
  QwtPlotCurve *c_ondo;
  QwtPlotCurve *c_shitsudo;
  QwtPlotCurve *c_kiatsu;

  int nowX;
  double d_x[GRAPH_MAX_X + 1];
  double d_ondo_y[GRAPH_MAX_X + 1];
  double d_shitsudo_y[GRAPH_MAX_X + 1];
  double d_kiatsu_y[GRAPH_MAX_X + 1];
};

#endif // ONSHITSUDOKEI_H
