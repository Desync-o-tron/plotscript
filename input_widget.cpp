#include "input_widget.hpp"

InputWidget::InputWidget(QWidget * parent) : QPlainTextEdit(parent){
  this->setObjectName("input");
}

void InputWidget::keyPressEvent(QKeyEvent * event)
{
  if ((event->key() == Qt::Key_Return 
    || event->key() == Qt::Key_Enter)
    && ( event->modifiers() == Qt::ShiftModifier))
  {
    sendDataFlag();
    return;
  }
   
  QPlainTextEdit::keyPressEvent(event);
}
