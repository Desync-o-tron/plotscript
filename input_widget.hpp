#pragma once
#include <QPlainTextEdit> 
#include <qdebug.h>
 
class InputWidget : public QPlainTextEdit{
  Q_OBJECT

public:

  InputWidget(QWidget * parent = nullptr);

  void keyPressEvent(QKeyEvent * event);

signals:
  void sendDataFlag();

};