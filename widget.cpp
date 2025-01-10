#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QTime>
#include <QDebug>
#include <QListWidgetItem>
#include <QUrl>
#include <QString>
#include <QTime>
#include <QString>
#include <QRandomGenerator>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //先进行布局
    //先实例化两种布局
    this->setWindowTitle("我的音乐播放器");
    this->vbox = new QVBoxLayout(this);             //垂直布局
    this->setLayout(this->vbox);                    //设置整个窗口为垂直布局
    this->hbox1 = new QHBoxLayout(this);            //按钮层水平布局
    this->hbox2 = new QHBoxLayout(this);            //标签层水平布局
    this->hbox3 = new QHBoxLayout(this);            //进度条布局

    //实例化对象、从上到下依次为媒体播放、listWidget、label、进度条、各种按钮
    this->mediaPlayer = new QMediaPlayer(this);

    this->listWidget = new QListWidget(this);       //创建结构简单的列表


    this->label = new QLabel("00:00/00:00",this);   //标签栏
    this->labName = new QLabel("暂无音乐",this);

    this->volumeSlider = new QSlider(this);         //进度条
    this->volumeSlider->setMaximumWidth(200);
    this->timeSlider = new QSlider(this);

    this->comboBox = new QComboBox(this);           // 选择框
    this->comboBox->addItem("顺序播放");
    this->comboBox->addItem("随机播放");
    this->comboBox->addItem("循环播放");

    //设置成水平的
    this->volumeSlider->setOrientation(Qt::Horizontal);
    this->timeSlider->setOrientation(Qt::Horizontal);
    this->volumeSlider->setMinimumWidth(100);
    this->volumeSlider->setRange(0,100);
    this->volumeSlider->setValue(50);

    this->openBtn = new QPushButton("打开",this);
    this->upBtn = new QPushButton("Previous",this);
    this->downBtn = new QPushButton("Next",this);
    this->playBtn = new QPushButton("Play",this);
    this->playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));

    this->timer = new QTimer(this);
    this->n = 0;
    this->index = 0;
    //给水平布局增加控件
    hbox1->addWidget(this->openBtn);
    hbox1->addWidget(this->upBtn);
    hbox1->addWidget(this->downBtn);
    hbox1->addWidget(this->playBtn);
    hbox1->addWidget(this->comboBox);

    hbox2->addWidget(this->label);
    hbox2->addWidget(this->labName);

    hbox3->addWidget(this->timeSlider);
    hbox3->addWidget(this->volumeSlider);
    //垂直布局
    vbox->addWidget(this->listWidget);
    vbox->addLayout(this->hbox2);
    vbox->addLayout(this->hbox3);
    vbox->addLayout(this->hbox1);

    // 设置文件样式
    initStyle();

    // 关联槽函数
    connect(this->openBtn,&QPushButton::clicked,this,&Widget::openFile);
    connect(this->playBtn,&QPushButton::clicked,this,&Widget::playSong);

    //双击播放
    connect(this->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleClickPlay(QListWidgetItem*)));

    //进度条变化
    // connect(this->mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateTimeSlider(qint64))); //进度条变化
    connect(this->mediaPlayer,&QMediaPlayer::positionChanged,this,&Widget::updateTimeSlider);

    // 持续时间改变，其实就是换歌了
    // connect(this->mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(setTimeSlider(qint64)));
    connect(this->mediaPlayer,&QMediaPlayer::durationChanged,this,&Widget::setTimeSlider);

    //滚动显示名字、显示歌曲已播放时间/总时间
    // connect(this->timer,SIGNAL(timeout()),this,SLOT(updateTime_NameShow()));
    connect(this->timer,&QTimer::timeout,this,&Widget::updateTime_NameShow);

    // 滑块改变进度：
    // connect(this->timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(updateSingPos(int)));
    connect(this->timeSlider,&QSlider::sliderMoved,this,&Widget::updateSingPos);

    // 音量滑块
    // connect(this->volumeSlider,SIGNAL(sliderMoved(int)),this,SLOT(updateVolume(int)));
    connect(this->volumeSlider,&QSlider::sliderMoved,this,&Widget::updateVolume);

    // 上一首和下一首：
    // connect(this->upBtn,SIGNAL(clicked()),this,SLOT(pre()));
    // connect(this->downBtn,SIGNAL(clicked()),this,SLOT(next()));
    connect(this->upBtn,&QPushButton::clicked,this,&Widget::pre);
    connect(this->downBtn,&QPushButton::clicked,this,&Widget::next);

    // 改变歌曲的时候，改变标签：
    connect(this->mediaPlayer,&QMediaPlayer::mediaChanged,[&](){
        songName = nameList.at(index);
    });

}

Widget::~Widget()
{
    delete ui;
}


// 思路一：用户自己选择文件
#if 0
void Widget::openFile()
{
    // 清空列表
    nameList.clear();
    listWidget->clear();


    // 获取文件目录：
    nameList = QFileDialog::getOpenFileNames(this,"选择文件",".","Mp3(*.mp3)");     // 标题，默认目录，选择格式

    // 如果为空，就跳过
    if(nameList.isEmpty()) return;

    // 将歌曲放入listWidget中
    for (auto songName : nameList) {
        // qInfo()<<songName;       // 已经是完整路径了
        QFileInfo info(songName);
        QListWidgetItem *item = new QListWidgetItem(info.fileName(),listWidget);
        listWidget->addItem(item);
        item->setToolTip(songName);
    }

    // 默认显示第一个：
    QString fileName = nameList.at(0);
    QFileInfo info(fileName);
    songName = info.fileName();
    labName->setText(songName);

    // 播放
    mediaPlayer->setMedia(QUrl(fileName));      // 设置音频文件
    mediaPlayer->setVolume(50);                 // 音量默认50
    mediaPlayer->play();

    // 定时器，控制滚动：
    timer->start(1000);

    // 设置播放图标为暂停：
    playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));

}

#endif


// 思路二：选择文件夹自动导入
void Widget::openFile()
{
    // 清空列表
    nameList.clear();
    listWidget->clear();

    // 获取文件夹目录：
    filePath =  QFileDialog::getExistingDirectory(this,"选择文件夹",".");
    QDir dir(filePath);
    QStringList list = dir.entryList(QStringList()<<"*.mp3"<<"*.wav");
    for(auto file : list)
    {
        nameList.append(file);
        listWidget->addItem(file);
    }

    // 默认显示第一个：
    QString fileName = nameList.at(0);
    QFileInfo info(fileName);
    songName = info.fileName();
    labName->setText(songName);

    // 播放
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath + "/" + fileName));      // 设置音频文件
    mediaPlayer->setVolume(50);                 // 音量默认50
    mediaPlayer->play();


    // 定时器，控制滚动：
    timer->start(1000);

    // 设置播放图标为暂停：
    playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));

}

void Widget::playSong()
{
    // 获取播放状态
    int state = this->mediaPlayer->state();
    if(state == QMediaPlayer::PlayingState)
    {
        //正在播放,暂停播放，图标修改成播放
        this->mediaPlayer->pause();
        this->playBtn->setText("播放");
        this->playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));

    }
    else if (state == QMediaPlayer::PausedState) {
        //暂停,继续播放，图标修改为暂停
        this->mediaPlayer->play();
        this->playBtn->setText("暂停");
        this->playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
    }
}

void Widget::doubleClickPlay(QListWidgetItem *item)//参数为双击的目标信息
{
    /*
    1、获取listWidget中下标位置
    2、设置媒体源、播放选中的歌曲
    3、修改名字
    4、修改图标
    5、定时器启动
    */
    this->index = this->listWidget->currentRow();        //获取当前下标，有利于后面的顺序播放的上一首，下一首
    //qInfo()<<filePath + "/" + nameList.at(index);
    this->mediaPlayer->setMedia(QUrl::fromLocalFile(filePath + "/" + nameList.at(index)));
    this->labName->setText(item->text());
    this->mediaPlayer->play();
    this->timer->start(1000);
    this->playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
}

//给进度条设置范围
void Widget::setTimeSlider(qint64 duration)
{
    this->timeSlider->setRange(0,int(duration));
}

//根据歌曲播放的位置修改进度条
void Widget::updateTimeSlider(qint64 position)
{
    this->timeSlider->setValue(int(position));
}

//滚动显示名字、修改文本时间
//实现播放模式
void Widget::updateTime_NameShow()
{
    //////////////////////// 实时修改时间进度 ///////////////////////////////
    //获取总时间跟当前播放时长
    qint64 dur = this->mediaPlayer->duration();
    qint64 pos = this->mediaPlayer->position();
    //实例化两个对象，用来显示
    QTime durTime(0,0,0);
    QTime posTime(0,0,0);
    this->label->setText(posTime.addMSecs(pos).toString("mm:ss") + "/" + durTime.addMSecs(dur).toString("mm:ss"));

    //////////////////////// 滚动显示歌曲名字 ///////////////////////////////
    if(++n>=songName.size()) n=0;
    QString playLableText = songName.mid(n) + "   " + songName.mid(0,n);
    labName->setText(playLableText );


    ////////////////////// 处理当播放完了以后的操作 /////////////////
    if(dur==pos) next();

}

//修改歌曲播放的位置
void Widget::updateSingPos(int positoin)
{
    this->mediaPlayer->setPosition(positoin);
}

// 音量滑块功能
void Widget::updateVolume(int positoin)
{
    this->mediaPlayer->setVolume(positoin);
}

void Widget::pre()
{
    // 首先要判断播放模式
    if(this->comboBox->currentText()=="顺序播放")
    {
        //再判断index范围
        if(--index < 0)
        {
            index = nameList.size()-1;
        }
    }
    else if(this->comboBox->currentText()=="随机播放")
    {
        index = qrand() % this->nameList.size();
    }
    else    // 循环模式
    {
        // index = index;
        // 不变
    }

    // 设置最新的播放资源
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath + "/" + nameList.at(index)));
    mediaPlayer->play();

    // 设置最新的播放资源名字
    listWidget->setCurrentRow(index);
    songName = nameList.at(index);

}

void Widget::next()
{
    // 首先要判断播放模式
    if(this->comboBox->currentText()=="顺序播放")
    {
        //再判断index范围
        if(++index >= nameList.size())
        {
            index = 0;
        }
    }
    else if(this->comboBox->currentText()=="随机播放")
    {
        index = qrand() % this->nameList.size();
    }
    else    // 循环模式
    {
        // index = index;
        // 不变
    }

    // 设置最新的播放资源
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath + "/" + nameList.at(index)));
    mediaPlayer->play();

    // 设置最新的播放资源名字
    listWidget->setCurrentRow(index);
    songName = nameList.at(index);
}


void Widget::initStyle()
{
    QFile file;
    file.setFileName(":/css/gray.css");
    file.open(QIODevice::ReadOnly);
    QString strQss = file.readAll();
    this->setStyleSheet(strQss);
}


