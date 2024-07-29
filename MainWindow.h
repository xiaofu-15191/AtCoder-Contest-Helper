#pragma once
#include<QtWidgets/QMainWindow>
#include<QtCore/QCoreApplication>
#include<QtWidgets>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonValue>
#include<QLabel>
#include<QPushButton>
#include<QAction>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QNetworkAccessManager>
#include<QtCharts/QChartView>
#include<QtCharts/QChart>
#include<QtCharts/QLineSeries>
#include<QtCharts/QScatterSeries>
#include<QtCharts/QValueAxis>
#include<QPalette>
#include<QVector>
#include<algorithm>
#define QT_CHARTS_USE_NAMESPACE
using namespace Qt;
#include "ui_MainWindow.h"
const int RatingNum[]={0,400,800,1200,1600,2000,2400,2800,3200,3600,4000,4400};
//const QColor RatingColor[]={QColor(0,0,0),QColor(128,128,128),QColor(128,64,0),QColor(0,128,0),QColor(0,192,192),QColor(0,0,255),QColor(192,192,0),QColor(255,128,0),QColor(255,0,0)};
const QColor RatingBackGroundColor[]={QColor(217,217,217),QColor(217,197,178),QColor(178,217,178),QColor(178,236,236),QColor(178,178,255),QColor(236,236,178),QColor(255,217,178),QColor(255,178,178),QColor(255,178,178),QColor(255,178,178),QColor(255,178,178)};
const QString DarkStyleSheet=R"(QWidget:focus{
		outline: none;
}
QListWidget{
	border-radius: 5px;
}
QListView::item{
height: 50px;
	border-radius: 5px;
}
QListView::Item:hover{
background: rgb(80,80,80);
}

QListView::Item:selected{
background: rgb(63,63,63);
})";
const QString LightStyleSheet=R"(QWidget:focus{
		outline: none;
}
QListWidget{
border-radius: 5px;
}
QListView::item{
height: 50px;
	border-radius: 5px;
}
QListView::Item:hover{
background: rgb(238,238,238);
}
QListView::Item:selected{
background: rgb(214,214,214);
})";
const QString RatingTipColorStyleSheet[]={"QToolTip{color:rgb(255,255,255);background-color:rgb(128,128,128)}",
"QToolTip{color:rgb(255,255,255);background-color:rgb(128,64,0)}",
"QToolTip{color:rgb(255,255,255);background-color:rgb(0,128,0)}",
"QToolTip{color:rgb(255,255,255);background-color:rgb(0,192,192)}",
"QToolTip{color:rgb(255,255,255);background-color:rgb(0,0,255)}",
"QToolTip{color:rgb(0,0,0);background-color:rgb(192,192,0)}",
"QToolTip{color:rgb(0,0,0);background-color:rgb(255,128,0)}",
"QToolTip{color:rgb(255,255,255);background-color:rgb(255,0,0)}"};
const QString RatingLabelColorStyleSheet[]={"QLabel{color:rgb(255,255,255)}",
"QLabel{color:rgb(171,160,144)}",
"QLabel{color:rgb(128,64,0)}",
"QLabel{color:rgb(0,128,0)}",
"QLabel{color:rgb(94,255,255)}",
"QLabel{color:rgb(0,0,255)}",
"QLabel{color:rgb(192,192,0)}",
"QLabel{color:rgb(255,128,0)}",
"QLabel{color:rgb(255,0,0)}"};
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow ui;
	QJsonDocument JsonDocument;
	QJsonArray JsonArray;
	QNetworkAccessManager *Manager;
	QNetworkReply *UserInformationReply;
	QString UserName;
	QPainter *Painter;
	QCPGraph *RatingGraph;
	QByteArray Data;
	QVector<QString>Contests;
	QVector<double>ContestsHistoryID,ContestsHistoryRating;
	QLabel *RatingDisplayLabel; 
	int NowRating=0,MaxRating=0;
	bool GottenRating=0;
	void ColorPreset();
	void GetUserInformation();
	void GetRating();
	void GetUsername();
	void RatingHistoryPointDisplay(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *watched,QEvent *event);
};
