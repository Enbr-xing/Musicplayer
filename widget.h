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

    void initStyle();

public slots:
    void openFile();//打开并选择文件
    void playSong();//暂停/播放
    void doubleClickPlay(QListWidgetItem *item);//双击选歌
    void setTimeSlider(qint64 duration); //设置进度条范围
    void updateTimeSlider(qint64 position); //进度条变化
    void updateTime_NameShow();//显示名字及时间变化
    void updateSingPos(int positoin);//更新歌曲进度条
    void updateVolume(int positoin);//更新音量
    void pre();//上一首
    void next();//下一首


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

    int n=0;                        //用n来辅助滚动
    QString songName;               //歌曲名
    QTimer *timer;                  //定时器
    QStringList nameList;           //存放所有的文件,依靠下标寻找位置
    int index;                      //下标
    int playMode=0;                 //播放模式，模式顺序播放
    QString filePath;               //文件夹路径
};
#endif // WIDGET_H
