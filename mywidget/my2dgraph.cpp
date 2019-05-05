#include "my2dgraph.h"

#include <QPainter>

#include "common/printf.h"

My2DGraph::My2DGraph(QWidget *parent)
    : QWidget(parent)
{
    m_dataList.clear();
    m_minData = 0.0;
    m_maxData = 0.0;
}

My2DGraph::~My2DGraph()
{

}

void My2DGraph::PaintGraph(const QString &logBuf, const int sectionIndex, const QString &name, const QString &axisNum)
{
    PRINTF(LOG_DEBUG,"%s starts\n", __func__);

    m_dataList.clear();
    m_minData = 0.0;
    m_maxData = 0.0;

    QString suffixName;
    switch(sectionIndex){
    case 0://全过程
        suffixName = name;
        break;
    case  1://过渡段
        suffixName = name + "T";
        break;
    case 2://工作段
        suffixName = name + "W";
        break;
    default:
        PRINTF(LOG_WARNING, "%s: undefined sectionIndex(%d)\n", __func__, sectionIndex);
        return;
    }

    //axis从1开始 Logger中[0]是string的类型标记
    int axis = axisNum.toInt();
    if(name.startsWith("time")){
        axis = 1;
    }

    bool isFirstLineFlag = true;
    QStringList dataList = logBuf.split("\n");
    m_dataList.reserve(dataList.size());
    for(const QString& line : dataList){
        if(line.startsWith(suffixName)){
            QStringList lineList = line.split(",");
            const QString& content = lineList[axis];
            double value = content.toDouble();
            if(isFirstLineFlag){
                isFirstLineFlag = false;
                m_maxData = value;
                m_minData = value;
            }else{
                m_dataList.push_back(value);
                m_maxData = std::max(m_maxData, value);
                m_minData = std::min(m_minData, value);
			}
        }
	}
    if(fabs(m_maxData - m_minData) < 0.2) {//间距过小
        m_maxData += 0.1;
        m_minData -= 0.1;
    }
    if(suffixName.startsWith("time")){//时间的Y轴从0开始
        m_minData = 0.0;
    }

    this->update();
}


void My2DGraph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    painter.begin(this);
    painter.eraseRect(this->rect());
    painter.setBrush(Qt::NoBrush);
    DrawGrid(&painter);

    //红色数据折线
    painter.setPen(Qt::red);
    int xLast = -1;
    int yLast = -1;
    int i = 1;
    for(const double& value : m_dataList){
        int x = MapXToWindow(i++);
        int y = MapYToWindow(value);
        if(xLast != -1){
            QPoint startPoint(xLast, yLast);
            QPoint endPoint(x, y);
            painter.drawLine(startPoint, endPoint);
		}
        xLast = x;
        yLast = y;
	}
    painter.end();
}

int My2DGraph::MapXToWindow(double data)
{
    double width = this->width();
    double size = m_dataList.size();
    return data/size * width;
}

int My2DGraph::MapYToWindow(double data)
{
    double height = this->height();
    double range = m_maxData - m_minData;
    return (m_maxData-data)/range * height;
}

void My2DGraph::DrawGrid(QPainter *painter)
{
    painter->drawRect(0, 0, this->width()-1, this->height()-1);

    //黑色栅格线
    painter->setPen(Qt::black);
    constexpr int verticalNum = 6;
    for(int index=0; index<verticalNum; ++index){
        double data = m_minData + (m_maxData-m_minData)/verticalNum*index;
        int y = MapYToWindow(data);
        painter->drawLine(0, y, this->width()-2,y);
        painter->drawText(0, y, QString::number(data, 'f', 1));
	}
    constexpr int horizontalNum = 8;
    for(int index=1; index<horizontalNum; ++index){
        double data = m_dataList.size()/horizontalNum * index;
        int x = MapXToWindow(data);
        painter->drawLine(x, 0, x, this->height()-2);
        painter->drawText(x, this->height()-1, QString::number((int)data));
	}

    //蓝色X轴
    painter->setPen(Qt::blue);
    int y = MapYToWindow(0);
    painter->drawLine(0, y, this->width()-2, y);
}
