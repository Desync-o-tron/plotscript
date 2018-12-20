#pragma once
#include "input_widget.hpp"
#include "output_widget.hpp"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtGui> 
#include <QtCore>
#include <qwidget.h>
#include <QPushButton>

class NotebookApp : public QWidget {
  Q_OBJECT

public:
  NotebookApp(QWidget * parent = nullptr);

  QMainWindow * mainWindow;
  QVBoxLayout * layout;
  
  QHBoxLayout * buttonLayout;
  QPushButton * start;
  QPushButton * stop;
  QPushButton * reset;
  QPushButton * interrupt;

  InputWidget * input;
  OutputWidget * output;
  
public slots:
  void grabNgiveText();
};