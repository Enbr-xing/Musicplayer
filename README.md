# sMusicPlayer_QT_3

## 介绍
用QT做的音乐播放器3，支持，随机播放，顺序播放，还有名字滚动功能。

排版比较简单。此处的思路参考了[CSDN的一个博主](https://blog.csdn.net/u012510215/article/details/128264941)。



## 效果预览

1. 打开文件夹所有音乐，列成目录
2. 点击播放
3. 播放，暂停，上下，定义模式模式
4. 控制音量
5. 滚动显示歌名

![音乐播放器3](assets/%E9%9F%B3%E4%B9%90%E6%92%AD%E6%94%BE%E5%99%A83.gif)



界面初步美化后的效果：

![音乐播放器3](assets/%E9%9F%B3%E4%B9%90%E6%92%AD%E6%94%BE%E5%99%A83-1679389738468.gif)



## 制作步骤

### 项目准备

+ 创建本地的项目。注意是创建 QWidget 项目
+ 在码云上准备好项目，复制链接。
+ 在本地项目那里，右键，get bush here,  克隆下来。
+ 显示隐藏文件，将文件夹中所有文件，包括 .git 文件夹复制到项目目录中。
+ 写笔记，commit    push 上传



### 配置pro工程文件

+ 添加代码：

```cpp
QT += multimedia
QT += multimediawidgets
```

+ 如果MSVC出现乱码问题，就在pro文件中添加：

并且在行为的文本编辑器中，默认编码选择UTF-8，将非此编码的删除。

```cpp
msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
```

+ 推荐使用WinGW编译。  不过这个编译的速度，确实是比MSVC要慢不少的。



### 在头文件中写好代码框架

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QWidget>
#include <QHBoxLayout>//水平布局
#include <QVBoxLayout>//垂直布局
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider> //进度条
#include <QComboBox>
#include <QListWidget> //显示所有歌曲
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
//    void openFile();//打开并选择文件
//    void playSong();//暂停/播放
//    void doubleClickPlay(QListWidgetItem *item);//双击选歌
//    void setTimeSlider(qint64 duration); //设置进度条范围
//    void updateTimeSlider(qint64 position); //进度条变化
//    void updateTime_NameShow();//显示名字及时间变化
//    void updateSingPos(int positoin);//更新歌曲进度条
//    void updateVolume(int positoin);//更新音量
//    void pre();//上一首
//    void next();//下一首

private:
    Ui::Widget *ui;
    QMediaPlayer *mediaPlayer;      //媒体播放
    QVBoxLayout *vbox;              //整体的垂直布局
    QHBoxLayout *hbox1;             //按钮水平布局
    QHBoxLayout *hbox2;             //标签水平布局
    QHBoxLayout *hbox3;             //进度条水平布局
    QPushButton *openBtn;           //打开文件
    QPushButton *upBtn;             //上一首
    QPushButton *downBtn;           //下一首
    QPushButton *playBtn;           //播放/暂停
    QComboBox *comboBox;            //存放播放顺序 //下拉框
    QSlider *volumeSlider;          //进度条
    QSlider *timeSlider;
    QListWidget *listWidget;        //显示所有文件的窗口
    QLabel *label;                  //显示播放时间/总时间
    QLabel *labName;                //线上歌曲名，支持滚动

    int n;                          //用n来辅助滚动
    QString songName;               //歌曲名
    QTimer *timer;                  //定时器
    QStringList nameList;           //存放所有的文件,依靠下标寻找位置
    int index;                      //下标
    int playMode=0;                 //播放模式，模式顺序播放
    QString filePath;               //文件夹路径
};
#endif // WIDGET_H

```



### 现在的 cpp 文件

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


```



### 布局排版

+ 一般采用的是UI布局，这里采用CSDN上面一个博主的布局方法：
+ 这里使用的是 Box 布局，先垂直布局，在放入水平布局。

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

        //先进行布局
        //先实例化两种布局
        this->vbox = new QVBoxLayout(this); //垂直布局
        this->setLayout(this->vbox);//设置整个窗口为垂直布局
        this->hbox1 = new QHBoxLayout(this); //按钮层水平布局
        this->hbox2 = new QHBoxLayout(this);//标签层水平布局
        this->hbox3 = new QHBoxLayout(this);//进度条布局
        //实例化对象、从上到下依次为媒体播放、listWidget、label、进度条、各种按钮
        this->mediaPlayer = new QMediaPlayer(this);

        this->listWidget = new QListWidget(this); //创建结构简单的列表


        this->label = new QLabel("00:00/00:00",this); //标签栏
        this->labName = new QLabel("暂无音乐",this);

        this->volumeSlider = new QSlider(this);//进度条
        this->volumeSlider->setMaximumWidth(200);
        this->timeSlider = new QSlider(this);

        this->comboBox = new QComboBox(this);
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
        
        
        //一会儿放connect槽函数的地方：
        
}

Widget::~Widget()
{
    delete ui;
}


```



### 打开文件功能：

+ 在头文件中，找到 void openFile(); 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数：

  ```cpp
  connect(this->openBtn,&QPushButton::clicked,this,&Widget::openFile);
  ```

+ 写相关槽参数的两种思路：

  

####       思路一：用户选文件

```cpp
// 思路一：用户自己选择文件
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
```

####       思路二：选择文件夹自动导入

```cpp
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
```

#### 如果想测试，记得cpp文件里加上头文件

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QTime>
#include <QDebug>
#include <QListWidgetItem>
#include <QUrl>
#include <QString>
```



### 播放和暂停功能

+ 在头文件中，找到 void playSong(); 取消注释，使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数：

  ```cpp
  connect(this->playBtn,&QPushButton::clicked,this,&Widget::playSong);
  ```

+ 写相关槽参数

  ```cpp
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
  ```

### 双击播放功能

+ 在头文件中，找到 void doubleClickPlay(); 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数：这里使用了老版写法，快速传参：

  ```cpp
  connect(this->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
          this,SLOT(doubleClickPlay(QListWidgetItem*)));//双击播放
  ```

+ 写相关槽参数的思路：

  ```cpp
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
      qInfo()<<filePath + "/" + nameList.at(index);
      this->mediaPlayer->setMedia(QUrl::fromLocalFile(filePath + "/" + nameList.at(index)));
      this->labName->setText(item->text());
      this->mediaPlayer->play();
      this->timer->start(1000);
      this->playBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
  }
  ```

  

### 播放音频的进度条

+ 在头文件中，找到 void setTimeSlider(); 和updateTimeSlider() 使用 Alt + enter 快捷键，添加定义。

+ 这里，一个是改变当前播放进度，一个是改变歌曲的总时长。都只针对滑块，不针对标签词条。

+ 添加槽函数：这里使用了老版写法，快速传参：

  ```cpp
  // 当歌曲进度改变的时候
  connect(this->mediaPlayer,&QMediaPlayer::positionChanged,this,&Widget::updateTimeSlider);
  // 当歌曲的持续时间改变的时候，其实就是换歌的时候：
  connect(this->mediaPlayer,&QMediaPlayer::durationChanged,this,&Widget::setTimeSlider);
  
  // 老版本写法：可以不用，参考即可：
  // connect(this->mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateTimeSlider(qint64))); //进度条变化
  // connect(this->mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(setTimeSlider(qint64))); // 持续时间改变，其实就是换歌了
  ```

+ 写相关槽参数的思路：

  ```cpp
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
  ```

  

### 拖动进度条实现改变进度

+ 在头文件中，找到 voidupdateSingPos(); 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数：也可以使用老版写法，快速传参：

  ```cpp
  // 绑定滑块和播放进度
  connect(this->timeSlider,&QSlider::sliderMoved,this,&Widget::updateSingPos);
  
  // connect(this->timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(updateSingPos(int)));
  ```

+ 写相关槽参数的思路：

  ```cpp
  //修改歌曲播放的位置
  void Widget::updateSingPos(int positoin)
  {
      this->mediaPlayer->setPosition(positoin);
  }
  ```

  

### 滚动显示名字、实时修改播放进度

+ 在头文件中，找到 updateTime_NameShow(); 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数：

  ```cpp
  //滚动显示名字、显示歌曲已播放时间/总时间
  connect(this->timer,SIGNAL(timeout()),this,SLOT(updateTime_NameShow())); 
  
  // 其他写法：
  connect(this->timeSlider,&QSlider::sliderMoved,this,&Widget::updateSingPos);
  ```

+ 写相关槽参数的思路：

  ```cpp
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
      this->label->setText(posTime.addMSecs(pos).toString("mm:ss") + "/"
                           + durTime.addMSecs(dur).toString("mm:ss"));
  
      //////////////////////// 滚动显示歌曲名字 ///////////////////////////////
      if(++n>=songName.size()) n=0;
      QString playLableText = songName.mid(n);
      labName->setText(playLableText);
      
      ////////////////////// 处理当播放完了以后的操作 /////////////////
  	// 后面补充
  }
  ```



### 改变音量

+ 在头文件中，找到 updateVolume(); 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数，两个都行：

  ```cpp
  // 音量滑块
  // connect(this->volumeSlider,SIGNAL(sliderMoved(int)),this,SLOT(updateVolume(int)));
  connect(this->volumeSlider,&QSlider::sliderMoved,this,&Widget::updateVolume);
  ```

+ 写相关槽参数的思路：

  ```cpp
  //修改音量
  void Widget::updateVolume(int positoin)
  {
      this->mediaPlayer->setVolume(positoin);
  }
  ```



### 上一首和下一首功能

+ 在头文件中，找到 pre()  和 next()  ; 使用 Alt + enter 快捷键，添加定义。

+ 添加槽函数，两个都行：

  ```cpp
  // 上一首和下一首：
  // connect(this->upBtn,SIGNAL(clicked()),this,SLOT(pre()));
  // connect(this->downBtn,SIGNAL(clicked()),this,SLOT(next()));
  connect(this->upBtn,&QPushButton::clicked,this,&Widget::pre);
  connect(this->downBtn,&QPushButton::clicked,this,&Widget::next);
  ```

+ 写相关槽参数的思路：

  ```cpp
  
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
  
  ```

+ 完成以后，发现音乐显示名字文字没改，改一下，添加一个信号：

  ```cpp
  // 改变歌曲的时候，改变标签：
  connect(this->mediaPlayer,&QMediaPlayer::mediaChanged,[&](){
      songName = nameList.at(index);
  });
  ```

+ 完善之前的音乐播放完了以后的代码：并且还把滚动字幕完善了一下

  ```cpp
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
  ```

  

## 后期优化

### 导入css配置

+ 左上角，点击新建，新建选择QT，  资源 QT Resource File ，输入 res

+ 打开项目目录，新建css目录，加入css文件

+ 在构造函数中，添加 initStyle() 函数。

  ```cpp
  void Widget::initStyle()
  {
      QFile file;
      file.setFileName(":/css/gray.css");
      file.open(QIODevice::ReadOnly);
      QString strQss = file.readAll();
      this->setStyleSheet(strQss);
  }
  ```

  

### 打包

release 模式，编译运行一次，然后把exe文件复制到其他目录下，运行 windeployqt.exe   XXX.exe

```
找到QT自带的windeployqt.exe 

使用QT自带的cmd，管理员 如 Qt 5.14.2 (MinGW 7.3.0 64-bit)
然后 ： 	e:  进入到E盘
cd 路径	cd E:\QT_project\MyLovers1\result\
执行：		windeployqt XX.exe


打开Enigma Virtual Box
添加主进程，选择exe
添加文件递归，选择整个目录，封包
```


