#include "output_widget.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

//message_queue<std::string> msgs_to_tui;
//message_queue<Expression> msgs_to_gui;

//struct expsNmsgs
//{
//  Expression exp;
//  std::string msg;
//  bool msgPresent;
//};
//message_queue<expsNmsgs> outputMsgs;

message_queue<std::string> inputMsgs;
message_queue<Expression> outputMsgs;
Interpreter interp;
std::thread th1;
bool threadOff = 0; // 0 for off, 1 for on

void concurrent_tui_access(Interpreter * interp);

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
  GScene = new QGraphicsScene;
  GView = new QGraphicsView(GScene);
  rawText = new QString;
  interp = new Interpreter;
  
  layout = new QVBoxLayout;
  layout->addWidget(GView);
  setLayout(layout);

  setObjectName("output");

  GView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  GView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  //boot up file
  std::ifstream ifs(STARTUP_FILE);
  if (!ifs)
    qDebug() << "couldn't read file";
  else
  {
    interp->parseStream(ifs);
    Expression exp = interp->evaluate();
  }
  
  th1 = std::thread(concurrent_tui_access, interp);
}

OutputWidget::~OutputWidget()
{
  if (!threadOff)
  {
    inputMsgs.push("EXIT_LOOP_");
    th1.join();
  }
}

void OutputWidget::receiveText(QString text) {
  *rawText = text;
  processText();
}

void OutputWidget::processText() {
  GScene->clear();
  std::string in = rawText->toUtf8().constData();
  if (in.empty()) return;

  if (threadOff)
  {
    Expression exp(Atom("Error: interpreter kernel not running"));
    gval(exp);
    return;
  }


  inputMsgs.push(in);
  //std::thread th1 concurrent_tui_access();//soon 2 b threaded
  gui_message_checker();
}

void OutputWidget::gui_message_checker() 
{
  Expression exp;
  /*for (int i = 0; i < 18999999; ++i){
    if (outputMsgs.try_pop(exp))
    {
      gval(exp);
    }
  }*/
  outputMsgs.wait_and_pop(exp);
  gval(exp);
}

void OutputWidget::startThread() {
  if (threadOff) {
    th1 = std::thread(concurrent_tui_access, interp);
    threadOff = 0;
  }
}
void OutputWidget::resetThread() {
  interp = new Interpreter();
  stopThread();
  startThread();
  /*if (threadOff) {
    th1 = std::thread(concurrent_tui_access, interp);
    threadOff = 0;
  }*/
}
void OutputWidget::stopThread() {
  if (!threadOff) {
    inputMsgs.push("EXIT_LOOP_");
    th1.join();
    threadOff = 1;
  }
}

void concurrent_tui_access(Interpreter * interp)
{
  while (1)
  {
    Expression inputExp;
    Expression outputExp;
    std::string msg;
    inputMsgs.wait_and_pop(msg);
    
    if (msg == "EXIT_LOOP_")
      return;
    
    std::istringstream expression(msg);
    if (!interp->parseStream(expression)) {
      //GScene->addText("Error: Invalid Expression. Could not parse.");
      outputExp = Expression(Atom("Error: Invalid Expression. Could not parse."));
    }
    else {
      try {
        outputExp = interp->evaluate();
        //gval(inputExp);
      }
      catch (const SemanticError & ex) {
        std::ostringstream oss;
        oss << ex.what();
        //GScene->addText(oss.str().c_str());
        outputExp = Expression(Atom(oss.str().c_str()));
      }
    }
    outputMsgs.push(outputExp);
  }
}

void OutputWidget::gval(Expression exp) {

  if (exp.isTypePoint())
    handle_point(exp);
  else if (exp.isTypeLine())
    handle_line(exp);
  else if (exp.isHeadList()) {
    for (auto it = exp.tailConstBegin(); it < exp.tailConstEnd(); ++it) {
      gval(*it);
    }
  }
  else if (!exp.head().isLambda())
    handle_text(exp);    
  
  GView->fitInView(GScene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::handle_text(Expression exp) {
  double x = 0, y = 0;
  double textScale = 1;
  double rotation = 0; //in rads
  QGraphicsItem * item;
  if (exp.isTypeText()){
    if (exp.pList.find("\"position\"") != exp.pList.end()) 
    {
      x = exp.pList.find("\"position\"")->second.tailConstBegin()->head().asNumber();
      auto backit = exp.pList.find("\"position\"")->second.tailConstEnd() - 1;
      y = backit->head().asNumber();

      QString text = exp.head().asSymbol().c_str();
      if (text[0] == '\"' && text[text.size() - 1] == '\"') {
        text.remove(0, 1);
        text.remove(text.length() - 1, 1);
      }
      if (exp.pList.find("\"text-scale\"") != exp.pList.end()) {
        textScale = exp.pList.find("\"text-scale\"")->second.head().asNumber();
        if (textScale <= 0) { 
          textScale = 1;
        }
      }
      if (exp.pList.find("\"text-rotation\"") != exp.pList.end()) {
        rotation = exp.pList.find("\"text-rotation\"")->second.head().asNumber()*180.0 / M_PI;
        if (rotation > 180)
          rotation = rotation - 360;
      }
      
      auto font = QFont("Monospace");
      font.setStyleHint(QFont::TypeWriter);
      font.setPixelSize(1);
      item = GScene->addText(text, font);
      item->setScale(textScale);

      QRectF wordBBox = item->boundingRect();
      item->setPos(x - wordBBox.width() / 2.0, y - wordBBox.height() / 2.0);
      
      item->setTransformOriginPoint(item->boundingRect().center());
      item->setRotation(rotation);

    }
  }
  else
  {
    std::stringstream ss;
    ss << exp;
	std::string str = ss.str().c_str();

	size_t found = str.find("Error: interpreter kernel not");
	if(found != std::string::npos)
	{
		ss.str(std::string());
		ss << "Error: interpreter kernel not running";
		qDebug() << ss.str().c_str();
	}
    item = GScene->addText(ss.str().c_str());
  }

}

void OutputWidget::resizeEvent(QResizeEvent * event)
{
  event = nullptr;
  // resize as needed
  GView->fitInView(GScene->itemsBoundingRect(), Qt::KeepAspectRatio);
  if (!event)
    return;
}

void OutputWidget::handle_line(Expression exp) {
  Expression pt1 = *(exp.tailConstBegin());
  Expression pt2 = *(--exp.tailConstEnd());
  qreal x1;
  qreal x2;
  qreal y1;
  qreal y2;

  if ((*pt1.tailConstBegin()).head().isNumber())
    x1 = (*pt1.tailConstBegin()).head().asNumber();
  else
    return;
  if ((--pt1.tailConstEnd())->head().isNumber())
    y1 = (--pt1.tailConstEnd())->head().asNumber();
  else
    return;
  if ((*pt2.tailConstBegin()).head().isNumber())
    x2 = (*pt2.tailConstBegin()).head().asNumber();
  else
    return;
  if ((--pt2.tailConstEnd())->head().isNumber())
    y2 = (--pt2.tailConstEnd())->head().asNumber();
  else
    return;
  
  QPen pen;
  if (exp.pList.find("\"thickness\"") != exp.pList.end())
  {
    if (exp.pList.find("\"thickness\"")->second.head().asNumber() < 0) {
      GScene->addText("ERROR: thickness is not positive");  
      return;
    }
    pen.setWidth(exp.pList.find("\"thickness\"")->second.head().asNumber());
  }
  GScene->addLine(x1, y1, x2, y2, pen);
}

void OutputWidget::handle_point(Expression exp) 
{
  double x = exp.tailConstBegin()->head().asNumber();
  auto backit = --exp.tailConstEnd();
  double y = backit->head().asNumber();
  double width = exp.pList.find("\"size\"")->second.head().asNumber();
  
  if (width < 0) {
    GScene->addText("Error: point size not positive.");
    return;
  }

  auto * item = new QGraphicsEllipseItem();
  QRectF rectangle(x - (width / 2.), y - (width / 2.), width, width);

  QPen * penX = new QPen;
  penX->setWidth(0);
  item->setPen(*penX);
  item->setBrush((Qt::black));
  item->setRect(rectangle);
  GScene->addItem(item);
}