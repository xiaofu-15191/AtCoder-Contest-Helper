#include "MainWindow.h"
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.UserNameSearch,&QPushButton::pressed,this,&MainWindow::GetUsername);
	connect(ui.UserNameEdit,&QLineEdit::editingFinished,this,&MainWindow::GetUsername);
    Manager=new QNetworkAccessManager();
	RatingDisplayLabel=new QLabel(this);
	RatingX=new QValueAxis;
	RatingY=new QValueAxis;
	RatingChart=new QChart;
	RatingChart->setTitle("Rating");
	ui.RatingView->setChart(RatingChart);
	ui.RatingView->setRenderHint(QPainter::Antialiasing);
	RatingLineSeries=new QLineSeries;
	RatingPointSeries=new QScatterSeries;
	connect(RatingPointSeries,&QScatterSeries::hovered,this,&MainWindow::RatingHistoryPointDisplay);
	setAutoFillBackground(true);
}
MainWindow::~MainWindow()
{
}
void MainWindow::GetUserInformation()
{
	Request=QNetworkRequest(QUrl("https://atcoder.jp/users/"+UserName));
	Reply=Manager->get(Request);
	connect(Reply,&QNetworkReply::readyRead,this,&MainWindow::GetRating);
	connect(Reply,&QNetworkReply::finished,[&]{Reply->deleteLater();});
}
void MainWindow::GetRating()
{
	ui.RatingView->hide();
	Data=Reply->readAll();
	qsizetype st=Data.indexOf("<script>var rating_history="),en=Data.indexOf("];</script>");
	st+=27;
	QByteArray RatingHistory_Origin=Data.mid(st,en-st+1);
	JsonDocument=QJsonDocument::fromJson(RatingHistory_Origin);
	JsonArray=JsonDocument.array();
	Contests.clear();
	ContestRatingPoints.clear();
	NowRating=0;MaxRating=0;
	for(int i=0;i<JsonArray.size();i++)
	{
		QString ContestName=JsonDocument[i]["StandingsUrl"].toString();
		for(int j=0;!ContestName.isEmpty()&&j<10;j++) ContestName.erase(ContestName.begin());
		while(!ContestName.isEmpty()&&ContestName.back()!='/') ContestName.erase(ContestName.end()-1);
		ContestName.erase(ContestName.end()-1);
		int TempRating=JsonDocument[i]["NewRating"].toInt();
		Contests.push_back(ContestName);
		MaxRating=std::max(MaxRating,TempRating);
		ContestRatingPoints.push_back(QPoint(i+1,TempRating));
		NowRating=TempRating;
	}
	RatingLineSeries->replace(ContestRatingPoints);
	RatingPointSeries->replace(ContestRatingPoints);
	RatingX->setRange(0,JsonArray.size()+1);
	RatingX->setTitleText("Contest");
	RatingX->setTickCount(JsonArray.size()+2);
	RatingX->setLabelFormat("%d");
	int RatingLimitNumNow=std::upper_bound(RatingNum,RatingNum+8,NowRating)-RatingNum,RatingLimitNumMax=std::upper_bound(RatingNum,RatingNum+8,MaxRating)-RatingNum;
	RatingY->setRange(0,RatingNum[RatingLimitNumMax]);
	RatingY->setTitleText("Rating");
	RatingY->setLabelFormat("%d");
	RatingY->setTickCount(RatingNum[RatingLimitNumMax]/400+1);
	//RatingLineSeries->setColor(RatingColor[RatingLimitNumNow]);
	//RatingPointSeries->setColor(RatingColor[RatingLimitNumNow]);
	RatingLineSeries->setColor(QColor(10,10,10));
	RatingPointSeries->setColor(QColor(20,20,20));
	RatingLineSeries->setMarkerSize(10);
	RatingPointSeries->setMarkerSize(10);
	RatingChart->removeSeries(RatingLineSeries);
	RatingChart->removeSeries(RatingPointSeries);
	RatingChart->addSeries(RatingLineSeries);
	RatingChart->addSeries(RatingPointSeries);
	RatingChart->setAxisX(RatingX,RatingLineSeries);
	RatingChart->setAxisY(RatingY,RatingLineSeries);
	RatingChart->setAxisX(RatingX,RatingPointSeries);
	RatingChart->setAxisY(RatingY,RatingPointSeries);
	RatingChart->setAnimationOptions(QChart::SeriesAnimations);
	//RatingChart->setBackgroundVisible(false);
	RatingChart->legend()->hide();
	ui.RatingView->show();
}
void MainWindow::GetUsername()
{
	UserName=ui.UserNameEdit->text();
	GetUserInformation();
}
void MainWindow::RatingHistoryPointDisplay(const QPointF &point,bool state)
{
	if(state)
	{
		int tmp=std::upper_bound(RatingNum,RatingNum+8,(int)(point.y()))-RatingNum;
		RatingDisplayLabel->setText((QString(Contests[(int)(point.x())-1])+" Rating:"+QString::asprintf("%d",(int)(point.y()))));
		QPoint CurrentPos=mapFromGlobal(QCursor::pos());
		RatingDisplayLabel->move(CurrentPos.x()-RatingDisplayLabel->width()/2,CurrentPos.y()-RatingDisplayLabel->height()*1.5);
		RatingDisplayLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
		RatingDisplayLabel->setMinimumHeight(60);
		RatingDisplayLabel->setAlignment(AlignCenter);
		RatingDisplayLabel->setWordWrap(true);
		switch(RatingNum[tmp])
		{
			case 400:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(128,128,128)");
				break;
			}
			case 800:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(128,64,0)");
				break;
			}
			case 1200:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(0,128,0)");
				break;
			}
			case 1600:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(0,192,192)");
				break;
			}
			case 2000:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(0,0,255)");
				break;
			}
			case 2400:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(192,192,0)");
				break;
			}
			case 2800:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(255,128,0)");
				break;
			}
			default:
			{
				RatingDisplayLabel->setStyleSheet("border-radius: 6px;background-color:rgb(255,0,0)");
				break;
			}
		}
		RatingDisplayLabel->show();
	}
	else
		RatingDisplayLabel->hide();
}