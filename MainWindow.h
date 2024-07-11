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
const int RatingNum[]={0,400,800,1200,1600,2000,2400,2800,3200,3600,4000};
const QColor RatingColor[]={QColor(0,0,0),QColor(128,128,128),QColor(128,64,0),QColor(0,128,0),QColor(0,192,192),QColor(0,0,255),QColor(192,192,0),QColor(255,128,0),QColor(255,0,0)};
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindowClass ui;
	QJsonDocument JsonDocument;
	QJsonArray JsonArray;
	QNetworkAccessManager *Manager;
	QNetworkRequest Request;
	QNetworkReply *Reply;
	QString UserName;
	/*QChart *RatingChart;
	QLineSeries *RatingLineSeries;
	QScatterSeries *RatingPointSeries;
	QValueAxis *RatingX,*RatingY;*/
	QCPGraph *RatingGraph;
	QByteArray Data;
	QVector<QString>Contests;
	QVector<double>ContestsHistoryID,ContestsHistoryRating;
	QLabel *RatingDisplayLabel; 
	int NowRating=0,MaxRating=0;
	void GetUserInformation();
	void GetRating();
	void GetUsername();
	void RatingHistoryPointDisplay(QMouseEvent *event);
	bool eventFilter(QObject *watched,QEvent *event);
};
