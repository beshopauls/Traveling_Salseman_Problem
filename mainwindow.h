#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QPoint>
#include <QList>
#include <QMap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QIntValidator>
#include <QPushButton>
#include <QTimerEvent>
#include <QLineEdit>
#include <QToolBar>
#include <QLabel>
#include <QtMath>
#include <QTime>

class QGraphicsView;
class QLineEdit;

class MainWindow : public QMainWindow
{
  Q_OBJECT
private:
  QGraphicsView* viewport_;
  QLineEdit* max_cities_;
  QList<int> state_;
  QMap<int, QPointF> coords_;
  float current_energy_;
  float current_temp_;
  int counter_ = 0;
  float initial_temp_ = 10.0f;
  float end_temp_ = 0.00001f;
  QSize getFieldSize() const;
  static bool isTransition(float probability);
  static float getTransitionProbability(float de, float t);
  static float decreaseTemperatureFunc(float initial_temp, float i);
  static QList<int> generateStateCandidate(const QList<int>& seq);
  static float dist(const QPointF& p1, const QPointF& p2);
  float getEnergy(const QList<int>& seq) const;
  float getCurrentEnergy() const;
protected:
  void timerEvent(QTimerEvent* event) override;
public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
private:
  void createToolbar();
};

#endif // MAINWINDOW_H
