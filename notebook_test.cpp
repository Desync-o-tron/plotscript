#include <QTest>
#include <QApplication>
#include <notebook_app.hpp>

class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testTextSend();
  void testPointCreation();
  void testPointArray();
  void testTextArray();
  void testLineArray();
  void testDiscretePlotLayout();
  void testDiscretePlot_linear(); //TODO FIXERup? or del
  void test_start_stop();

public:
  NotebookApp nApp;
  InputWidget * input;
  OutputWidget * output;
};

void NotebookTest::initTestCase(){
 
  input = nApp.findChild<InputWidget *>("input");
  QVERIFY2(input, "couldn't find input widget boss");
  output = nApp.findChild<OutputWidget *>("output");
  QVERIFY2(output, "couldn't find output widget");
  
  nApp.show();
}

void NotebookTest::testPointCreation() {

  QTest::keyClicks(input, "(set-property \"size\" 4 (make-point 0 0))");
  QTest::qWait(200);
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  QTest::qWait(100);
  
  QList<QGraphicsItem *> items = output->GScene->items();
  QCOMPARE(items.size(), 1);
  
  QPointF ptf(0, 0);
  QVERIFY2(items[0]->contains(ptf), "point doesn't exist at 0,0");

}

void NotebookTest::testPointArray()
{

  QTest::keyClick(input, Qt::Key_A, Qt::ControlModifier);
  QTest::keyClick(input, Qt::Key_Delete);
  QTest::keyClicks(input, "(list (set-property \"size\" 1 (make-point 0 0)) (set-property \"size\" 2 (make-point 4 0)) (set-property \"size\" 4 (make-point 8 0)) (set-property \"size\" 8 (make-point 16 0)) (set-property \"size\" 16 (make-point 32 0)) (set-property \"size\" 32 (make-point 64 0)) )");
  QTest::qWait(200);
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  QTest::qWait(400);

  QList<QGraphicsItem *> items = output->GScene->items();
  QTest::qWait(4000);
  QVERIFY(items.size() == 6);
}

void NotebookTest::testTextArray()
{
  QTest::keyClick(input, Qt::Key_A, Qt::ControlModifier);
  QTest::keyClick(input, Qt::Key_Delete);
  QTest::qWait(200);
  QTest::keyClicks(input, "(begin   (define xloc 0)  (define yloc 0)  (list   (set-property \"position\" (make-point (+ xloc 20) yloc) (make-text \"Hi\"))  (set-property \"position\" (make-point (+ xloc 40) yloc) (make-text \"Hi\"))  (set-property \"position\" (make-point (+ xloc 60) yloc) (make-text \"Hi\"))  (set-property \"position\" (make-point (+ xloc 80) yloc) (make-text \"Hi\"))  (set-property \"position\" (make-point (+ xloc 100) yloc) (make-text \"Hi\"))  )  )");
  QTest::qWait(200);
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  QTest::qWait(500);

  QList<QGraphicsItem *> items = output->GScene->items();
  
  QVERIFY(items.size() == 5);
}

void NotebookTest::testLineArray()
{
  QTest::keyClick(input, Qt::Key_A, Qt::ControlModifier);
  QTest::keyClick(input, Qt::Key_Delete);
  QTest::qWait(200);
  QTest::keyClicks(input, "(list  (make-line  (make-point 10 0)  (make-point 10 20))  (set-property \"thickness\" 2  (make-line  (make-point 0 0)  (make-point 0 20)))  (make-line  (make-point 20 0)  (make-point 20 20))  )  ");
  QTest::qWait(200);
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  QTest::qWait(200);

  QList<QGraphicsItem *> items = output->GScene->items();

  QVERIFY(items.size() == 3);
  
}

void NotebookTest::testTextSend()
{ 
  QTest::qSleep(200);
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  QTest::qSleep(200);
    
  QPointF ptf(0, 0);
  QTransform qtransf;
  //auto outputItem = output->GScene->itemAt(ptf,qtransf);
  QList<QGraphicsItem *> items = output->GScene->items();
  
  //qDebug() << items.size();
  //for (QGraphicsItem *it : items)
  //{
  //  QRectF rectf = it->boundingRect();
  //  qDebug() << rectf.center().rx() << rectf.center().ry();
  //}

}

/*
findLines - find lines in a scene contained within a bounding box
with a small margin
*/
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

  QPainterPath selectPath;

  QMarginsF margins(margin, margin, margin, margin);
  selectPath.addRect(bbox.marginsAdded(margins));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numlines(0);
  foreach(auto item, scene->selectedItems()) {
    if (item->type() == QGraphicsLineItem::Type) {
      numlines += 1;
    }
  }

  return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
*/
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numpoints(0);
  foreach(auto item, scene->selectedItems()) {
    if (item->type() == QGraphicsEllipseItem::Type) {
      numpoints += 1;
    }
  }

  return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
rotation and string contents
*/
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {

  int numtext(0);
  foreach(auto item, scene->items(center)) {
    //qDebug() << "NUM ITEMS: " << scene->items().size();//.children.size();
    if (item->type() == QGraphicsTextItem::Type) {
      QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
      if ((text->toPlainText() == contents) &&
        (text->rotation() == rotation) &&
        (text->pos() + text->boundingRect().center() == center)) {
        numtext += 1;
      }
      //if (rotation != 0 && text->rotation() != 0)
      //{
      //  qDebug() << text->toPlainText();
      //  qDebug() << text->rotation();
      //  qDebug() << text->pos() + text->boundingRect().center();
      //}
    }
  }
  return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
*/
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
  scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

  int numlines(0);
  foreach(auto item, scene->selectedItems()) {
    if (item->type() == QGraphicsLineItem::Type) {
      numlines += 1;
    }
  }

  return numlines;
}

void NotebookTest::testDiscretePlotLayout() {

  std::string program = R"( 
  (discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
      (list "abscissa-label" "X Label") 
      (list "ordinate-label" "Y Label") ))
  )";
  auto inputWidget = input;
  auto outputWidget = output;

  inputWidget->setPlainText(QString::fromStdString(program));
  QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

  auto view = outputWidget->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();
  QCOMPARE(items.size(), 17);

  // make them all selectable
  foreach(auto item, items) {
    item->setFlag(QGraphicsItem::ItemIsSelectable);
  }

  double scalex = 20.0 / 2.0;
  double scaley = 20.0 / 2.0;

  double xmin = scalex * -1;
  double xmax = scalex * 1;
  double ymin = scaley * -1;
  double ymax = scaley * 1;
  double xmiddle = (xmax + xmin) / 2;
  double ymiddle = (ymax + ymin) / 2;

  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

  // check ordinate label
  //qDebug() << "le point " << QPointF(xmin - 3, -ymiddle);
  QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);
  
  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

  // check the bounding box bottom
  QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

  // check the bounding box top
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

  // check the bounding box left and (-1, -1) stem
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

  // check the bounding box right and (1, 1) stem
  QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis 
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

  // check the point at (-1,-1)
  QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

  // check the point at (1,1)
  QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
} 

//void NotebookTest::testDiscretePlotLinear()
//{
//  std::string program = R"( 
//  (begin
//      (define f (lambda (x) 
//          (list x (+ (* 2 x) 1))))
//      (discrete-plot (map f (range -2 2 0.5))
//         (list
//         (list "title" "The Data")
//         (list "abscissa-label" "X Label")
//         (list "ordinate-label" "Y Label")
//         (list "text-scale" 1)))))";
//
//  //f(x) = 2x+1 .. (-2,-3),(-1.5,-2),(-1, ),(-.5, ),(0, ),(.5, ),(1, ),(1.5, ),(2,5)
//  //9 points
//  //9 lollilines
//  //3 title labels
//  //4 axis point labels
//  //4 boundry lines
//  //2 axis lines
//  //  
//
//  auto inputWidget = input;
//  auto outputWidget = output;
//
//  inputWidget->setPlainText(QString::fromStdString(program));
//  QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);
//
//  auto view = outputWidget->findChild<QGraphicsView *>();
//  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");
//
//  auto scene = view->scene();
//
//  // first check total number of items
//  // 16 lines + 8 points + 7 text = 31
//  auto items = scene->items();
//  QCOMPARE(items.size(), 31);
//
//  // make them all selectable
//  foreach(auto item, items) {
//    item->setFlag(QGraphicsItem::ItemIsSelectable);
//  }
//
//  double scalex = 20.0 / 4.; //20/(xmax-xmin)
//  double scaley = 20.0 / 4.;
//
//  double xmin = scalex * -2;
//  double xmax = scalex * 2;
//  double ymin = scaley * -(-2*2+1);
//  double ymax = scaley * (2*2+1);
//  double xmiddle = (xmax + xmin) / 2.;
//  double ymiddle = (ymax + ymin) / 2.;
//
//
//  // check title
//  qDebug() << "le title point " << QPointF(xmiddle, -(ymax + 3));
//  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);
//
//  // check abscissa label
//  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);
//
//  // check ordinate label
//  qDebug() << "le point " << QPointF(xmin - 3, -ymiddle);
//  QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);
//
//  // check abscissa min label
//  QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-2")), 1); 
//
//  // check abscissa max label
//  QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2")), 1); 
//
//  // check ordinate min label
//  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-3")), 1); //-2*2+1
//
//  // check ordinate max label
//  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("5")), 1); //2*2+1
//  ///////////////////asdf
//  // check the bounding box bottom
//  QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);
//
//  // check the bounding box top
//  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);
//
//  // check the bounding box left and (-1, -1) stem
//  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);
//
//  // check the bounding box right and (1, 1) stem
//  QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);
//
//  // check the abscissa axis
//  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);
//
//  // check the ordinate axis 
//  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);
//
//  // check the point at (-1,-1)
//  QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);
//
//  // check the point at (1,1)
//  QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
//}

void NotebookTest::testDiscretePlot_linear() {

  std::string program = R"( 
  (begin
      (define f (lambda (x) 
          (list x (+ (* 2 x) 1))))
      (discrete-plot (map f (range -2 2 0.5))
         (list
         (list "title" "The Data")
         (list "abscissa-label" "X Label")
         (list "ordinate-label" "Y Label")
         (list "text-scale" 1)))))";
  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
  

  auto view = output->findChild<QGraphicsView *>();
  auto scene = view->scene();

  // first check total number of items
  // 15 lines + 9 points + 7 text = 31
  auto items = scene->items();
  QCOMPARE(items.size(), 31);
}

void NotebookTest::test_start_stop()
{
	std::string program = R"( 
	(begin
		(define f (lambda (x) 
			(list x (+ (* 2 x) 1))))
		(discrete-plot (map f (range -2 2 0.5))
			(list
			(list "title" "The Data")
			(list "abscissa-label" "X Label")
			(list "ordinate-label" "Y Label")
			(list "text-scale" 1)))))";
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	auto scene = view->scene();

	auto items = scene->items();
	QCOMPARE(items.size(), 31);

	auto startbutton = nApp.findChild<QPushButton *>("start");
	QVERIFY2(startbutton, "Could not find startbutton as child of nApp");
	auto stopbutton = nApp.findChild<QPushButton *>("stop");
	QVERIFY2(stopbutton, "Could not find stopbutton as child of nApp");
	auto resetbutton = nApp.findChild<QPushButton *>("reset");
	QVERIFY2(resetbutton, "Could not find resetbutton as child of nApp");
	
	stopbutton->click();
	
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
	
	items = scene->items();
	QCOMPARE(items.size(), 1);
	
	startbutton->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	//stop n reset
	stopbutton->click();
	resetbutton->click();

	program = "(f 2)";
	input->setPlainText(QString::fromStdString(program));
	QTest::qWait(200);
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);
	
	items = scene->items();
	qDebug() << "post reset size: "<< items.size();
	QCOMPARE(items.size(), 1);

}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

