#include <notebook_app.hpp>

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
    
  layout = new QVBoxLayout;
  input = new InputWidget(this);
  output = new OutputWidget(this);
  start = new QPushButton("Start Kernel",this);
  stop = new QPushButton("Stop Kernel", this);
  reset = new QPushButton("Reset Kernel", this);
  interrupt = new QPushButton("Interrupt",this);
  start->setObjectName("start");
  stop->setObjectName("stop");	
  reset->setObjectName("reset");

  buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(start);
  buttonLayout->addWidget(stop);
  buttonLayout->addWidget(reset);
  buttonLayout->addWidget(interrupt);
  
  layout->addLayout(buttonLayout);
  layout->addWidget(input);
  layout->addWidget(output);

  this->setLayout(layout);
    
  QObject::connect(input, &InputWidget::sendDataFlag,
    this, &NotebookApp::grabNgiveText);

  QObject::connect(start, &QPushButton::clicked,
    output, &OutputWidget::startThread);
  
  QObject::connect(stop, &QPushButton::clicked,
    output, &OutputWidget::stopThread);
  
  QObject::connect(reset, &QPushButton::clicked,
    output, &OutputWidget::resetThread);
}

void NotebookApp::grabNgiveText() {
  output->receiveText(input->toPlainText());
}

