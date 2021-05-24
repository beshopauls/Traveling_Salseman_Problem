#include "mainwindow.h"



#define sqr(x) ((x)*(x))
#define frand() (float(qrand()) / float(RAND_MAX))
MainWindow::MainWindow(QWidget* parent):
  QMainWindow(parent),
  viewport_(new QGraphicsView(this)),
  max_cities_(new QLineEdit(this))
{
  createToolbar();

  resize(640, 680);
  setWindowTitle("Traveling salseman");
  setCentralWidget(viewport_);
}

MainWindow::~MainWindow() {}

void MainWindow::createToolbar() {
  auto toolbar = addToolBar("");


  auto label = new QLabel(" Max cities: ", toolbar);
  auto run = new QPushButton("Run", toolbar);
  label->setFixedWidth(64);
  run->setFixedWidth(128);

  max_cities_->setFixedWidth(150);
  max_cities_->setValidator(new QIntValidator(2, 1000000, this));
  max_cities_->setText(QString::number(10));

  toolbar->addWidget(label);
  toolbar->addWidget(max_cities_);
  toolbar->addWidget(run);

  connect(run, &QPushButton::clicked, [this]() {
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    int count = max_cities_->text().toInt();
    int s = static_cast<int>(qCeil(qSqrt(count)));
    for (int i = 1; i <= count; ++i) {
      state_ << i;
      for (;;) {
        QPointF p(frand() * s, frand() * s);

        bool unique = true;
        for (auto& e : coords_.values()) {
          if (e == p) {
            unique = false;
            break;
          }
        }

        if (unique) {
          coords_[i] = p;
          break;
        }
      }
    }

    std::random_shuffle(state_.begin(), state_.end());
    current_energy_ = getCurrentEnergy();
    current_temp_ = initial_temp_;
    counter_ = 1;

    startTimer(10);
  });
}

void MainWindow::timerEvent(QTimerEvent* event) {
  Q_UNUSED(event);
  auto state_candidate = generateStateCandidate(state_);
  auto candidate_energy = getEnergy(state_candidate);

  if (candidate_energy < current_energy_) {
    current_energy_ = candidate_energy;
    state_ = state_candidate;
  }
  else {
    auto p = getTransitionProbability(candidate_energy - current_energy_, current_temp_);
    if (isTransition(p)) {
      current_energy_ = candidate_energy;
      state_ = state_candidate;
    }
  }

  current_temp_ = decreaseTemperatureFunc(initial_temp_, counter_);
  counter_ += 1;

  if (current_temp_ == end_temp_) {
   killTimer(event->timerId());
  }


  if (!viewport_->scene()) {
    auto scene = new QGraphicsScene(viewport_);
    viewport_->setScene(scene);
  }

  QPixmap pixmap(getFieldSize());
  QPainter painter;
  painter.begin(&pixmap);
  painter.setPen(Qt::green);
  painter.fillRect(0, 0, pixmap.width(), pixmap.height(), Qt::SolidPattern);


  float count = max_cities_->text().toInt();
  float s = static_cast<float>(qFloor(qSqrt(count)));
  float sy = pixmap.height() * 1.0f / (s + 2.0f);
  float sx = pixmap.width() * 1.0f / (s + 2.0f);
  float dx = sx * 0.5f, dy = sy * 0.5f;

  for (auto& e : state_) {
    auto p = coords_[e];
    painter.drawEllipse(dx + p.x() * sx - 2, dy + p.y() * sy - 2, 8, 8);
  }
 painter.setPen(Qt::blue);
  for (int i = 0; i < state_.size() - 1; ++i) {
    auto p1 = coords_[state_[i]];
    auto p2 = coords_[state_[i + 1]];
    painter.drawLine(dx + p1.x() * sx, dy + p1.y() * sy, dx + p2.x() * sx, dy + p2.y() * sy);
  }

  auto p1 = coords_[state_.back()];
  auto p2 = coords_[state_.front()];
  painter.drawLine(dx + p1.x() * sx, dy + p1.y() * sy, dx + p2.x() * sx, dy + p2.y() * sy);

  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 14));
  painter.drawText(4, 24, QString("T= %1").arg(current_temp_));


  painter.end();

  viewport_->scene()->clear();
  viewport_->scene()->addPixmap(pixmap);
  repaint();
  update();
}

QSize MainWindow::getFieldSize() const {
  return centralWidget()->size() * 0.975;
}

bool MainWindow::isTransition(float probability) {
  auto value = float(qrand()) / float(RAND_MAX);
  return value <= probability;
}

float MainWindow::getTransitionProbability(float de, float t) {
  return qExp(-de / t);
}

QList<int> MainWindow::generateStateCandidate(const QList<int>& seq) {
  int i = rand() % seq.size(), j = i;
  while (i == j) {
    j = rand() % seq.size();
  }

  if (i > j) qSwap(i, j);

  QList<int> target;
  for (int k = 0; k < i; ++k) target.push_back(seq[k]);
  for (int k = j; k >= i; --k) target.push_back(seq[k]);
  for (int k = j + 1; k < seq.size(); ++k) target.push_back(seq[k]);

  return target;
}

float MainWindow::decreaseTemperatureFunc(float initial_temp, float i) {
  return initial_temp * 0.1f / i;
}

float MainWindow::dist(const QPointF& p1, const QPointF& p2) {
  return qSqrt(sqr(p1.x() - p2.x()) + sqr(p1.y() - p2.y()));
}

float MainWindow::getEnergy(const QList<int>& seq) const {
  auto acc = 0.0f;
  for (int i = 0; i < seq.size() - 1; ++i) {
    acc += dist(coords_[seq[i]], coords_[seq[i + 1]]);
  }

  acc += dist(coords_[seq.front()], coords_[seq.back()]);

  return acc;
}

float MainWindow::getCurrentEnergy() const {
  return getEnergy(state_);
}

