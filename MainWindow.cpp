#include "MainWindow.h"
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
    ui.setupUi(this);
	QApplication::instance()->installEventFilter(this);
    connect(ui.UserNameSearch,&QPushButton::pressed,this,&MainWindow::GetUsername);
	connect(ui.UserNameEdit,&QLineEdit::editingFinished,this,&MainWindow::GetUsername);
    Manager=new QNetworkAccessManager();
	RatingDisplayLabel=new QLabel(this);
	/*RatingX=new QValueAxis;
	RatingY=new QValueAxis;
	RatingChart=new QChart;
	RatingChart->setTitle("Rating");
	ui.RatingView->setChart(RatingChart);
	ui.RatingView->setRenderHint(QPainter::Antialiasing);
	RatingLineSeries=new QLineSeries;
	RatingPointSeries=new QScatterSeries;*/
	//connect(RatingPointSeries,&QScatterSeries::hovered,this,&MainWindow::RatingHistoryPointDisplay);
	setAutoFillBackground(true);
	RatingGraph=ui.RatingViewPlot->addGraph();
	ui.RatingViewPlot->legend->setVisible(false);
	//ui.RatingViewPlot->hide();
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
	ui.RatingViewPlot->hide();
	Data=Reply->readAll();
	qsizetype st=Data.indexOf("<script>var rating_history="),en=Data.indexOf("];</script>");
	st+=27;
	QByteArray RatingHistory_Origin=Data.mid(st,en-st+1);
	JsonDocument=QJsonDocument::fromJson(RatingHistory_Origin);
	JsonArray=JsonDocument.array();
	Contests.clear();
	ContestsHistoryID.clear();
	ContestsHistoryRating.clear();
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
		/*ContestRatingPoints.push_back(QPoint(i+1,TempRating));*/
		ContestsHistoryID.push_back(i+1);
		ContestsHistoryRating.push_back(TempRating);
		NowRating=TempRating;
	}
	RatingGraph->setData(ContestsHistoryID,ContestsHistoryRating);
	/*RatingLineSeries->replace(ContestRatingPoints);
	RatingPointSeries->replace(ContestRatingPoints);
	RatingX->setRange(0,JsonArray.size()+1);
	RatingX->setTitleText("Contest");
	RatingX->setTickCount(JsonArray.size()+2);
	RatingX->setLabelFormat("%d");*/
	int RatingLimitNumNow=std::upper_bound(RatingNum,RatingNum+11,NowRating)-RatingNum,RatingLimitNumMax=std::upper_bound(RatingNum,RatingNum+11,MaxRating)-RatingNum;
	RatingGraph->setPen(QPen(QColor(0,0,0)));
	RatingGraph->setLineStyle(QCPGraph::lsLine);
	RatingGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,10));
	ui.RatingViewPlot->yAxis->setRange(0,RatingNum[RatingLimitNumMax]);
	ui.RatingViewPlot->xAxis->setRange(0,JsonArray.size()+1);
	QSharedPointer<QCPAxisTickerFixed>TickerY(new QCPAxisTickerFixed),TickerX(new QCPAxisTickerFixed);
	TickerY->setTickStep(400);
	TickerX->setTickStep(1);
	ui.RatingViewPlot->yAxis->setTicker(TickerY);
	ui.RatingViewPlot->xAxis->setTicker(TickerX);
	ui.RatingViewPlot->yAxis->setSubTickLength(0,0);
	ui.RatingViewPlot->xAxis->setSubTickLength(0,0);
	ui.RatingViewPlot->xAxis->setVisible(false);
	ui.RatingViewPlot->setBackground(QPixmap(":/pics/icons/pic-for-yellow.png"));
	QLinearGradient plotGradient;
	plotGradient.setColorAt(0,QColor(255,255,255,0));
	ui.RatingViewPlot->setBackground(plotGradient);
	ui.RatingViewPlot->replot();
	ui.RatingViewPlot->show();
}
void MainWindow::GetUsername()
{
	UserName=ui.UserNameEdit->text();
	GetUserInformation();
}
void MainWindow::RatingHistoryPointDisplay(QMouseEvent * event)
{
	double MouseX=ui.RatingViewPlot->xAxis->pixelToCoord(event->pos().x());
	double MouseY=ui.RatingViewPlot->yAxis->pixelToCoord(event->pos().y());
	if(MouseX<ui.RatingViewPlot->xAxis->range().lower||MouseX>ui.RatingViewPlot->xAxis->range().upper||MouseY<ui.RatingViewPlot->yAxis->range().lower||MouseY>ui.RatingViewPlot->yAxis->range().upper) return;
	if(ContestsHistoryRating.empty()) return;
	/*auto it=std::lower_bound(ContestsHistoryRating.begin(),ContestsHistoryRating.end(),MouseY);
	int where=it-ContestsHistoryRating.begin();
	int TargetX=ContestsHistoryID.at(where);
	double TargetY=0;*/
	double RatioX=ui.RatingViewPlot->xAxis->axisRect()->width()/(ui.RatingViewPlot->xAxis->range().upper-ui.RatingViewPlot->xAxis->range().lower);
	double RatioY=ui.RatingViewPlot->yAxis->axisRect()->height()/(ui.RatingViewPlot->yAxis->range().upper-ui.RatingViewPlot->yAxis->range().lower);
	int get=0,TargetX=0;
	double TargetY=0,tmp=15;
	for(get=0;get<ContestsHistoryRating.size();get++)
	{
		int TempX=get+1;
		double TempY=ContestsHistoryRating.at(get);
		double tmp1=fabs(TempX-MouseX),tmp2=fabs(TempY-MouseY);
		if(tmp1*RatioX+tmp2*RatioY<tmp)
		{
			tmp=tmp1*RatioX+tmp2*RatioY;
			TargetX=TempX;
			TargetY=TempY;
		}
	}
	if(get==-1) return;
	if(tmp<15)
	{
		QString StrTip=QString("Rating:"+QString::asprintf("%d",(int)TargetY));
		int tmp=std::upper_bound(RatingNum,RatingNum+11,(int)TargetY)-RatingNum;
		switch(RatingNum[tmp])
		{
			case 400:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(128,128,128)}");
				break;
			}
			case 800:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(128,64,0)}");
				break;
			}
			case 1200:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(0,128,0)}");
				break;
			}
			case 1600:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(0,192,192)}");
				break;
			}
			case 2000:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(0,0,255)}");
				break;
			}
			case 2400:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(192,192,0)}");
				break;
			}
			case 2800:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(255,128,0)}");
				break;
			}
			default:
			{
				this->setStyleSheet("QToolTip{background-color:rgb(255,0,0)}");
				break;
			}
		}
		QToolTip::showText(event->globalPos(),StrTip,ui.RatingViewPlot);
	}
	else
		if(QToolTip::isVisible())
			QToolTip::hideText();
}
bool MainWindow::eventFilter(QObject * watched,QEvent * event)
{
	if(watched==ui.RatingViewPlot)
	{
		if(event->type()==QEvent::MouseMove)
		{
			QMouseEvent *pMouseEvent=(QMouseEvent*)event;
			RatingHistoryPointDisplay(pMouseEvent);
		}
	}
	return QObject::eventFilter(watched,event);
}
/*void MainWindow::RatingHistoryPointDisplay(const QPointF &point,bool state)
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
}*/