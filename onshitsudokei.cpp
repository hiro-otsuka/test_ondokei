#include "onshitsudokei.h"
#include "ui_onshitsudokei.h"
#include "qwt_legend.h"
#include <QMessageBox>

Onshitsudokei::Onshitsudokei(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Onshitsudokei)
{
  ui->setupUi(this);

  //変数初期化
  nowX = 0;

  //間隔制御用タイマ生成
  timer = new QTimer(this);

  //グラフ設定
//  ui->qwtPlot->setTitle("温度・湿度・気圧計");
  //  ui->qwtPlot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
  ui->qwtPlot->setCanvasBackground(QColor(Qt::black));

  ui->qwtPlot->enableAxis(QwtPlot::xBottom, false);
  ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, GRAPH_MAX_X, GRAPH_MAX_X / 4);

//  ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "温度");
  ui->qwtPlot->setAxisScale(QwtPlot::yLeft, AXIS_ONDO_MIN, AXIS_ONDO_MAX, 0);

  ui->qwtPlot->enableAxis(QwtPlot::yRight, true);
//  ui->qwtPlot->setAxisTitle(QwtPlot::yRight, "気圧");
  ui->qwtPlot->setAxisScale(QwtPlot::yRight, AXIS_KIATSU_MIN, AXIS_KIATSU_MAX, 0);

  //グラフ内のグリッドを設定
  p_grid = new QwtPlotGrid;
  p_grid->setMajorPen(QPen(Qt::gray, 0, Qt::DashDotLine));
  p_grid->setMinorPen(QPen(Qt::gray, 0, Qt::DotLine));
  p_grid->attach(ui->qwtPlot);

  //温度用グラフを設定
  c_ondo = new QwtPlotCurve("温度");
  c_ondo->setRenderHint(QwtPlotItem::RenderAntialiased);
  c_ondo->setPen(QPen(Qt::red));
  c_ondo->attach(ui->qwtPlot);

  //湿度用グラフを設定
  c_shitsudo = new QwtPlotCurve("湿度");
  c_shitsudo->setRenderHint(QwtPlotItem::RenderAntialiased);
  c_shitsudo->setPen(QPen(Qt::cyan));
  c_shitsudo->attach(ui->qwtPlot);
  c_shitsudo->setYAxis(QwtPlot::yRight);

  //気圧用グラフを設定
  c_kiatsu = new QwtPlotCurve("気圧");
  c_kiatsu->setRenderHint(QwtPlotItem::RenderAntialiased);
  c_kiatsu->setPen(QPen(Qt::yellow));
  c_kiatsu->attach(ui->qwtPlot);
  c_kiatsu->setYAxis(QwtPlot::yRight);

  //MQTTサブスクライバを設定
  subscriber = new QMQTT::Client(QHostAddress("192.168.192.99"), 1883, this);
  connect(subscriber, &QMQTT::Client::connected, this, &Onshitsudokei::onConnected);
  connect(subscriber, &QMQTT::Client::received, this, &Onshitsudokei::onReceived);
  subscriber->connectToHost();
}

Onshitsudokei::~Onshitsudokei()
{
  delete ui;
}

void Onshitsudokei::onConnected()
{
  //MQTT接続後にトピックを設定
  timer->setSingleShot(true);
  timer->start(100);
  subscriber->subscribe("hoge/");
}

void Onshitsudokei::onReceived(const QMQTT::Message &message)
{
//  QMessageBox::information(this, tr("A clicked"), QString::number(timer->remainingTime()));
  //メッセージ受信時に規定間隔経過しているかどうかを調べる
  if (timer->remainingTime() != -1) return;

  //規定時間経過していた場合は再度タイマを起動する
  timer->start(GRAPH_UPDATE_INTERVAL * 1000);

  //メッセージを取り出す
  QString msg = QString::fromUtf8(message.payload());

  //グラフが右端まで到達していた場合は左にスクロールさせる
  if (nowX > GRAPH_MAX_X)
    {
      for (int i = 0; i < GRAPH_MAX_X; i ++)
        {
          d_ondo_y[i] = d_ondo_y[i+1];
          d_shitsudo_y[i] = d_shitsudo_y[i+1];
          d_kiatsu_y[i] = d_kiatsu_y[i+1];
        }
      nowX --;
    }

  //グラフ描画用の配列に値をセットする
  d_x[nowX] = nowX;
  d_ondo_y[nowX] = msg.mid(5, 4).toDouble() / 100;
  d_shitsudo_y[nowX] = msg.mid(10, 4).toDouble() / 100;
  d_kiatsu_y[nowX] = msg.mid(15, 4).toDouble();

  //下部のテキストにも値を描画する
  ui->valOndo->setText(QString::number(d_ondo_y[nowX]));
  ui->valShitsudo->setText(QString::number(d_shitsudo_y[nowX]));
  ui->valKiatsu->setText(QString::number(d_kiatsu_y[nowX]));

  //湿度は気圧の軸を使うのでグラフ描画用の配列のみ値を補正する
  d_shitsudo_y[nowX] = d_shitsudo_y[nowX] * (AXIS_KIATSU_MAX - AXIS_KIATSU_MIN) / 100 + AXIS_KIATSU_MIN;

  //グラフを描画する
  nowX++;
  c_ondo->setSamples(d_x, d_ondo_y, nowX);
  c_shitsudo->setSamples(d_x, d_shitsudo_y, nowX);
  c_kiatsu->setSamples(d_x, d_kiatsu_y, nowX);
  ui->qwtPlot->replot();
}

void Onshitsudokei::on_btnExit_clicked()
{
  //閉じるボタン
  QApplication::exit(0);
}
