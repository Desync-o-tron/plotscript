#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QVBoxLayout>

#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QList>
#include <QVector>
#include <QRect>
#include <QtMath>
#include <vector>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "message_queue.h"


class OutputWidget : public QWidget {
  Q_OBJECT

public:
  OutputWidget(QWidget * parent = nullptr);
  ~OutputWidget();
  QGraphicsScene * GScene;
  QGraphicsView * GView;
  QVBoxLayout * layout;
  QString * rawText;
  Interpreter * interp;
  std::thread th1;

  void receiveText(QString text);
  void processText();
  void gui_message_checker();
  void startThread();
  void resetThread();
  void stopThread();
  void gval(Expression exp);
  void handle_point(Expression exp);
  void handle_line(Expression exp);
  void handle_text(Expression exp);
  void resizeEvent(QResizeEvent *event);

};
