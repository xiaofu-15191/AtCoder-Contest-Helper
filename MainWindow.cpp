#include "MainWindow.h"
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent)
{
	ui.setupUi(this);
	QApplication::instance()->installEventFilter(this);
	QPixmapCache::setCacheLimit(1);
	connect(ui.UserNameSearch,&QPushButton::pressed,this,&MainWindow::GetUsername);
	connect(ui.UserNameEdit,&QLineEdit::editingFinished,this,&MainWindow::GetUsername);
	Manager=new QNetworkAccessManager();
	RatingDisplayLabel=new QLabel(this);
	setAutoFillBackground(true);
	RatingGraph=ui.RatingViewPlot->addGraph();
	ui.RatingViewPlot->legend->setVisible(false);
	ui.RatingViewPlot->xAxis->setVisible(false);
	ui.RatingViewPlot->yAxis->setVisible(false);
	ui.NowRatingLabel1->setFont(QFont("Lato",10,600));
	ui.MaxmiumRatingLabel1->setFont(QFont("Lato",10,600));
	ui.RatedMatchesLabel1->setFont(QFont("Lato",10,600));
	ui.RankingLabel1->setFont(QFont("Lato",10,600));
	ui.UserNameEdit->setFont(QFont("Lato",10,600));
	ui.NowRatingLabel2->setFont(QFont("Lato",10,600));
	ui.MaxmiumRatingLabel2->setFont(QFont("Lato",10,600));
	ui.RatedMatchesLabel2->setFont(QFont("Lato",10,600));
	ui.RankingLabel2->setFont(QFont("Lato",10,600));
	ui.UserNameLabel->setFont(QFont("Lato",20,QFont::Bold));
	QToolTip::setFont(QFont("Lato",9));
	ColorPreset();
	ui.SideBar->setCurrentRow(0);
	Painter=new QPainter(this);
}
MainWindow::~MainWindow()
{
}
void MainWindow::ColorPreset()
{
	QLinearGradient plotGradient;
	plotGradient.setColorAt(0,QColor(255,255,255,0));
	ui.RatingViewPlot->setBackground(plotGradient);
	if(QApplication::palette().color(QPalette::Window).lightness()<128)
	{
		ui.SideBar->setStyleSheet(DarkStyleSheet);
		ui.RatingViewPlot->yAxis->setTickLabelColor(QColor(235,238,245));
	}
	else
	{
		ui.SideBar->setStyleSheet(LightStyleSheet);
		ui.RatingViewPlot->yAxis->setTickLabelColor(QColor(40,44,52));
	}
}
void MainWindow::GetUserInformation()
{
	UserInformationReply=Manager->get(QNetworkRequest(QUrl("https://atcoder.jp/users/"+UserName+"?graph=rating")));
	connect(UserInformationReply,&QNetworkReply::readyRead,this,&MainWindow::GetRating);
	connect(UserInformationReply,&QNetworkReply::finished,[&]{UserInformationReply->deleteLater();});
}
void MainWindow::GetRating()
{
	ui.RatingViewPlot->hide();
	Data=UserInformationReply->readAll();
	if(int(Data.indexOf("<title>404 Not Found - AtCoder</title>"))!=-1)
	{
		ui.statusbar->showMessage("用户不存在。",1500);
		return;
	}
	qsizetype st=Data.indexOf("<script>var rating_history="),en=Data.indexOf("];</script>");
	st+=27;
	QByteArray RatingHistoryOrigin=Data.mid(st,en-st+1);
	JsonDocument=QJsonDocument::fromJson(RatingHistoryOrigin);
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
		ContestsHistoryID.push_back(i+1);
		ContestsHistoryRating.push_back(TempRating);
		NowRating=TempRating;
	}
	RatingGraph->setData(ContestsHistoryID,ContestsHistoryRating);
	int RatingLimitNumNow=std::upper_bound(RatingNum,RatingNum+11,NowRating)-RatingNum,RatingLimitNumMax=std::upper_bound(RatingNum,RatingNum+11,MaxRating)-RatingNum;
	GottenRating=1;
	RatingGraph->setPen(QPen(QColor(20,20,20)));
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
	ui.RatingViewPlot->yAxis->setVisible(true);
	ui.RatingViewPlot->replot();
	ui.RatingViewPlot->show();
	ui.NowRatingLabel2->setText(QString::asprintf("%d",NowRating));
	ui.NowRatingLabel2->setStyleSheet(RatingLabelColorStyleSheet[std::min(8,RatingLimitNumNow)]);
	ui.MaxmiumRatingLabel2->setText(QString::asprintf("%d",MaxRating));
	ui.MaxmiumRatingLabel2->setStyleSheet(RatingLabelColorStyleSheet[std::min(8,RatingLimitNumMax)]);
	ui.RatedMatchesLabel2->setText(QString::asprintf("%d",JsonArray.size()));
	st=Data.indexOf("<tr><th class=\"no-break\">Rank</th><td>"),en=Data.mid(st,75).indexOf("</td></tr>")+st;
	QByteArray RankOrigin;
	if(st<0||en<0)
	{
		RankOrigin=" -";
		ui.RankingLabel2->setWordWrap(true);
	}
	else
	{
		st+=38;
		RankOrigin=Data.mid(st,en-st);
	}
	ui.RankingLabel2->setText(QString(""+RankOrigin));
	st=Data.indexOf("<img class='avatar' src='"),en=Data.mid(st,200).indexOf("width='128' height='128'")+st;
	st+=25;
	ui.UserNameLabel->setText(UserName);
	ui.UserNameLabel->setStyleSheet(RatingLabelColorStyleSheet[std::min(8,RatingLimitNumNow)]);
	ui.statusbar->showMessage("加载完毕。",1000);
}
void MainWindow::GetUsername()
{
	UserName=ui.UserNameEdit->text();
	ui.statusbar->showMessage("加载用户信息中...");
	GetUserInformation();
}
void MainWindow::RatingHistoryPointDisplay(QMouseEvent * event)
{
	double MouseX=ui.RatingViewPlot->xAxis->pixelToCoord(event->pos().x());
	double MouseY=ui.RatingViewPlot->yAxis->pixelToCoord(event->pos().y());
	if(MouseX<ui.RatingViewPlot->xAxis->range().lower||MouseX>ui.RatingViewPlot->xAxis->range().upper||MouseY<ui.RatingViewPlot->yAxis->range().lower||MouseY>ui.RatingViewPlot->yAxis->range().upper) return;
	if(ContestsHistoryRating.empty()) return;
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
		QString StrTip=QString(Contests[(int)(TargetX-1)]+" Rating:"+QString::asprintf("%d",(int)TargetY));
		int tmp=std::upper_bound(RatingNum,RatingNum+11,(int)TargetY)-RatingNum;
		this->setStyleSheet(RatingTipColorStyleSheet[std::min(7,tmp-1)]);
		QToolTip::showText(event->globalPos(),StrTip,ui.RatingViewPlot);
	}
	else
		if(QToolTip::isVisible())
			QToolTip::hideText();
}
void MainWindow::paintEvent(QPaintEvent * event)
{
	if(GottenRating==0) return;
	Painter->begin(this);
	double PlotWidth=ui.RatingViewPlot->width()-60;
	double PlotHeight=ui.RatingViewPlot->height()-32;
	if(ui.SideBar->currentRow()==0)
	{
		int end_at=(MaxRating)/400;
		QPolygonF RatingBackRect;
		for(int i=0;i<=end_at;i++)
		{
			Painter->setPen(QPen(RatingBackGroundColor[i]));
			Painter->setRenderHint(QPainter::Antialiasing,true);
			QPointF TmpPoint=QPointF(175,63);
			RatingBackRect.clear();
			RatingBackRect.append(QPointF(TmpPoint.x(),TmpPoint.y()));
			RatingBackRect.append(QPointF(TmpPoint.x(),TmpPoint.y()+PlotHeight-PlotHeight/(end_at+1)*i));
			RatingBackRect.append(QPointF(TmpPoint.x()+PlotWidth,TmpPoint.y()+PlotHeight-PlotHeight/(end_at+1)*i));
			RatingBackRect.append(QPointF(TmpPoint.x()+PlotWidth,TmpPoint.y()));
			QBrush Filler;
			Filler.setColor(QColor(RatingBackGroundColor[i]));
			Filler.setStyle(Qt::SolidPattern);
			Painter->setBrush(Filler);
			Painter->drawPolygon(RatingBackRect);
		}
	}
	else
	{
		Painter->eraseRect(175,63,175+width(),63+height());
	}
	Painter->end();
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