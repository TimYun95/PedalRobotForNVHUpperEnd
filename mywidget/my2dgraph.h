#ifndef MY2DGRAPH_H
#define MY2DGRAPH_H

#include <vector>

#include <QWidget>
#include <QString>
#include <QPaintEvent>

class My2DGraph : public QWidget
{
	Q_OBJECT
public:
    My2DGraph(QWidget* parent=0);
    virtual ~My2DGraph();

    void PaintGraph(const QString& logBuf, const int sectionIndex, const QString& name, const QString& axisNum);

protected:
    void paintEvent(QPaintEvent* event);

private:
    int MapXToWindow(double data);
    int MapYToWindow(double data);
    void DrawGrid(QPainter* painter);

private:
    std::vector<double> m_dataList;
    double m_minData;
    double m_maxData;
};

#endif
