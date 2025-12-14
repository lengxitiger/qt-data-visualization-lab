#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QFont>
#include <QVector>
#include <algorithm>

struct Item {
    QString name;
    double price;
    QString spec;
};

class MedicalPricingViz : public QWidget {
public:
    MedicalPricingViz(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("C++QT可视化图表医疗耗材价格对比 - 输液器测试(作者-冷溪虎山)");
        resize(1000, 750);

        // 加载背景图
        bool bgLoaded = m_background.load("D:/ad/c/pic/background1.jpg");

        if (!bgLoaded) {
            qDebug() << "背景图未找到！路径: D:/ad/c/pic/background1.jpg";
            qDebug() << "将使用纯色背景";
            // 创建默认渐变背景
            m_useGradientBg = true;
        }

        // 提取的数据
        m_data = {
                {"一次性使用袋式输液器 带针", 6.65, "FV3-250mm 0.55"},
                {"一次性使用输液器 带针", 6.60, "BV4 0.7*25TWLB*25支"},
                {"一次性使用无菌溶药注射器 带针", 5.82, "RY50ml 1.6*30TWX"},
                {"一次性使用无菌注射器 带针", 3.16, "1ml 0.45*15RWSB"},
                {"一次性使用静脉输液针", 1.5, "0.55"},
                {"一次性使用无菌注射针", 1.66, "0.45-0.7"}
        };

        // 按价格从高到低排序
        std::sort(m_data.begin(), m_data.end(), [](const Item& a, const Item& b) {
            return a.price > b.price;
        });
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 1. 绘制背景
        if (!m_background.isNull()) {
            // 如果有背景图，缩放绘制
            p.save();  // 保存状态
            p.setOpacity(0.6);  // 透明度
            p.drawPixmap(rect(), m_background, m_background.rect());
            p.restore();  // 恢复状态
        } else if (m_useGradientBg) {
            // 使用渐变色背景
            QLinearGradient gradient(0, 0, width(), height());
            gradient.setColorAt(0, QColor(20, 30, 48));     // 深蓝
            gradient.setColorAt(1, QColor(36, 59, 85));     // 蓝灰
            p.fillRect(rect(), gradient);
        } else {
            p.fillRect(rect(), QColor(15, 15, 35)); // 纯色深蓝背景
        }

        // 2. 添加半透明遮罩，让前景内容更清晰
        p.setBrush(QColor(0, 0, 0, 100)); // 半透明黑色
        p.setPen(Qt::NoPen);
        p.drawRect(rect());

        // 3. 绘制各个图表组件
        drawTitle(p);
        drawBarChart(p, QRect(50, 80, 400, 300));      // 调整位置
        drawPieChart(p, QRect(500, 80, 450, 300));     // 调整位置和大小
        drawTable(p, QRect(50, 410, 900, 300));        // 调整位置和大小
    }

private:
    QPixmap m_background;
    bool m_useGradientBg = false;
    QVector<Item> m_data;  // 使用m_前缀避免重复

    void drawBarChart(QPainter& p, const QRect& area) {
        // 绘制背景框
        p.setBrush(QColor(30, 30, 50, 200));
        p.setPen(QColor(100, 150, 255, 150));
        p.drawRoundedRect(area, 10, 10);

        // 标题
        p.setPen(Qt::white);
        p.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
        p.drawText(area.left(), area.top() - 5, area.width(), 30,
                   Qt::AlignCenter, "💰 单价对比（元）");

        if (m_data.empty()) return;

        double maxPrice = m_data.front().price;
        int barWidth = 30;
        int spacing = 15;  // 增加间距
        int left = area.left() + 40;
        int bottom = area.bottom() - 40;
        int chartHeight = area.height() - 80;

        p.setPen(Qt::NoPen);
        for (int i = 0; i < m_data.size(); ++i) {
            double ratio = m_data[i].price / maxPrice;
            int height = ratio * chartHeight;

            // 柱状图渐变效果
            QLinearGradient grad(left + i * (barWidth + spacing), bottom - height,
                                 left + i * (barWidth + spacing), bottom);
            if (m_data[i].price > 5) {
                grad.setColorAt(0, QColor(255, 100, 100));   // 顶部：亮红
                grad.setColorAt(1, QColor(180, 60, 60));     // 底部：暗红
            } else if (m_data[i].price < 2) {
                grad.setColorAt(0, QColor(100, 180, 255));   // 顶部：亮蓝
                grad.setColorAt(1, QColor(60, 120, 180));    // 底部：暗蓝
            } else {
                grad.setColorAt(0, QColor(255, 200, 100));   // 顶部：亮黄
                grad.setColorAt(1, QColor(200, 150, 60));    // 底部：暗黄
            }

            p.setBrush(grad);

            // 绘制柱状图（带圆角）
            QRect barRect(left + i * (barWidth + spacing), bottom - height, barWidth, height);
            p.drawRoundedRect(barRect, 5, 5);

            // 柱顶数值标签
            p.setPen(Qt::white);
            p.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
            p.drawText(barRect.left(), barRect.top() - 20, barWidth, 15,
                       Qt::AlignCenter, QString::number(m_data[i].price, 'f', 2));

            // 底部名称标签（旋转显示）
            p.save();
            p.translate(barRect.left() + barWidth/2, bottom + 10);
            p.rotate(-45);  // 旋转45度避免重叠
            p.setFont(QFont("Microsoft YaHei", 8));
            QString label = m_data[i].name;
            if (label.length() > 10) label = label.left(8) + "...";
            p.drawText(-50, 0, 100, 20, Qt::AlignCenter, label);
            p.restore();
        }

        // Y轴刻度
        p.setPen(QColor(200, 200, 200, 150));
        p.setFont(QFont("Microsoft YaHei", 9));
        for (int i = 0; i <= 5; i++) {
            double value = maxPrice * i / 5.0;
            int y = bottom - chartHeight * i / 5.0;
            p.drawLine(left - 5, y, left, y);
            p.drawText(left - 40, y - 10, 35, 20, Qt::AlignRight | Qt::AlignVCenter,
                       QString::number(value, 'f', 1));
        }
    }

    void drawPieChart(QPainter& p, const QRect& area) {
        // 绘制背景框
        p.setBrush(QColor(30, 30, 50, 200));
        p.setPen(QColor(100, 150, 255, 150));
        p.drawRoundedRect(area, 10, 10);

        // 标题
        p.setPen(Qt::white);
        p.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
        p.drawText(area.left(), area.top() - 5, area.width(), 30,
                   Qt::AlignCenter, "📊 价格区间分布");

        int low = 0, mid = 0, high = 0;
        for (const auto& item : m_data) {
            if (item.price > 5) {
                high++;
            } else if (item.price < 2) {
                low++;
            } else {
                mid++;
            }
        }

        int total = low + mid + high;
        if (total == 0) return;

        // 饼图中心
        int cx = area.center().x();
        int cy = area.center().y();
        int radius = qMin(area.width(), area.height()) / 3 - 20;

        // 绘制饼图（带阴影效果）
        int startAngle = 0;
        QVector<int> slices = {low, mid, high};
        QVector<QColor> colors = {
                QColor(80, 180, 255),   // 低价 - 蓝
                QColor(255, 200, 100),  // 中价 - 黄
                QColor(255, 100, 100)   // 高价 - 红
        };

        for (int i = 0; i < 3; ++i) {
            if (slices[i] == 0) continue;

            int spanAngle = 360 * slices[i] / total;

            // 阴影效果
            p.save();
            p.translate(3, 3);
            p.setBrush(QColor(0, 0, 0, 100));
            p.setPen(Qt::NoPen);
            p.drawPie(cx - radius, cy - radius, radius * 2, radius * 2,
                      startAngle * 16, spanAngle * 16);
            p.restore();

            // 实际饼图
            p.setBrush(colors[i]);
            p.setPen(Qt::white);
            p.drawPie(cx - radius, cy - radius, radius * 2, radius * 2,
                      startAngle * 16, spanAngle * 16);

            // 在扇形中间显示百分比
            if (spanAngle > 30) {
                double midAngle = startAngle + spanAngle / 2.0;
                double rad = midAngle * 3.14159 / 180.0;
                int labelX = cx + (radius * 0.6) * cos(rad);
                int labelY = cy - (radius * 0.6) * sin(rad);

                p.setPen(Qt::white);
                p.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
                QString percent = QString::number(slices[i] * 100.0 / total, 'f', 0) + "%";
                p.drawText(labelX - 20, labelY - 10, 40, 20, Qt::AlignCenter, percent);
            }

            startAngle += spanAngle;
        }

        // 图例（在饼图右侧）
        int y = area.top() + 40;
        QVector<QString> labels = {
                QString("低价 (<2元): %1项").arg(low),
                QString("中价 (2~5元): %1项").arg(mid),
                QString("高价 (>5元): %1项").arg(high)
        };

        p.setFont(QFont("Microsoft YaHei", 10));
        for (int i = 0; i < 3; ++i) {
            p.setBrush(colors[i]);
            p.drawRect(area.right() - 150, y, 15, 15);
            p.setPen(Qt::white);
            p.drawText(area.right() - 130, y, 140, 15, Qt::AlignLeft, labels[i]);
            y += 25;
        }
    }

    void drawTable(QPainter& p, const QRect& area) {
        // 表格背景
        p.setBrush(QColor(30, 30, 50, 220));
        p.setPen(QColor(100, 150, 255, 150));
        p.drawRoundedRect(area, 10, 10);

        // 标题
        p.setPen(QColor(100, 200, 255));
        p.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
        p.drawText(area.left(), area.top() - 5, area.width(), 30,
                   Qt::AlignCenter, "📋 耗材详细清单");

        int rowHeight = 35;
        int y = area.top() + 30;
        QStringList headers = {"序号", "器械名称", "规格", "单价（元）"};
        int widths[] = {60, 400, 300, 100};

        // 表头（带背景色）
        p.setBrush(QColor(60, 80, 120, 200));
        p.setPen(Qt::NoPen);
        p.drawRect(area.left(), y, area.width(), rowHeight);

        p.setPen(QColor(220, 240, 255));
        p.setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
        int x = area.left() + 10;
        for (int i = 0; i < 4; ++i) {
            p.drawText(x, y, widths[i], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, headers[i]);
            x += widths[i];
        }

        // 数据行
        p.setFont(QFont("Microsoft YaHei", 10));
        for (int i = 0; i < m_data.size(); ++i) {
            y += rowHeight;

            // 交替行背景色
            if (i % 2 == 0) {
                p.setBrush(QColor(40, 45, 70, 150));
            } else {
                p.setBrush(QColor(50, 55, 80, 150));
            }
            p.setPen(Qt::NoPen);
            p.drawRect(area.left(), y, area.width(), rowHeight);

            // 绘制单元格内容
            x = area.left() + 10;
            p.setPen(i % 2 ? QColor(220, 220, 220) : QColor(240, 240, 240));

            // 序号
            p.drawText(x, y, widths[0], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, QString::number(i+1));
            x += widths[0];

            // 名称
            p.drawText(x, y, widths[1], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, m_data[i].name);
            x += widths[1];

            // 规格
            p.drawText(x, y, widths[2], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, m_data[i].spec);
            x += widths[2];

            // 价格（特殊颜色）
            if (m_data[i].price > 5) {
                p.setPen(QColor(255, 120, 120));  // 高价红色
            } else if (m_data[i].price < 2) {
                p.setPen(QColor(120, 200, 255));  // 低价蓝色
            }
            p.drawText(x, y, widths[3], rowHeight,
                       Qt::AlignRight | Qt::AlignVCenter,
                       "¥" + QString::number(m_data[i].price, 'f', 2));
        }
    }

    void drawTitle(QPainter& p) {
        // 标题背景
        p.setBrush(QColor(20, 40, 80, 200));
        p.setPen(QColor(100, 180, 255, 100));
        p.drawRoundedRect(0, 0, width(), 60, 0, 0);

        // 主标题
        p.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
        QLinearGradient titleGrad(0, 0, width(), 0);
        titleGrad.setColorAt(0, QColor(100, 200, 255));
        titleGrad.setColorAt(1, QColor(200, 150, 255));
        p.setPen(QPen(titleGrad, 2));
        p.drawText(0, 0, width(), 60, Qt::AlignCenter,
                   "🏥 医疗耗材数据可视化分析");

        // 副标题
        p.setFont(QFont("Microsoft YaHei", 10));
        p.setPen(QColor(200, 220, 255));
        p.drawText(0, 40, width(), 30, Qt::AlignCenter,
                   "免责声明:数据均为虚构演示，不涉及任何企业和单位商业机密");
    }
};

// 注意：由于没有Q_OBJECT，不需要.moc文件
// #include "medical_pricing_viz.moc"  // 删除这行

// ============ 主函数 ============
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 设置应用字体（确保中文显示）
    QFont font("Microsoft YaHei");
    app.setFont(font);

    MedicalPricingViz w;
    w.show();

    return app.exec();
}