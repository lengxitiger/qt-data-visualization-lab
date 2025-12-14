#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QFont>
#include <QVector>
#include <algorithm>
#include <cmath>

struct AccountItem {
    QString name;
    double amount;        // 金额（万元）
    double ratio;         // 占比
    QString trend;        // 趋势：↑增长 ↓下降 →平稳
    QColor color;         // 专属颜色
};

class FinanceAnalysisViz : public QWidget {
public:
    FinanceAnalysisViz(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("(C++QT版)财务会计科目可视化分析图表(作者-冷溪虎山)");
        resize(1100, 750);

        // 初始化数据 - 财务费用主要科目
        initData();

        // 按金额排序
        std::sort(m_data.begin(), m_data.end(), [](const AccountItem& a, const AccountItem& b) {
            return a.amount > b.amount;
        });

        // 计算总金额和占比
        double total = 0;
        for (auto& item : m_data) total += item.amount;
        for (auto& item : m_data) item.ratio = item.amount / total * 100;
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 1. 专业金融背景渐变
        drawGradientBackground(p);

        // 2. 添加网格线
        drawGrid(p);

        // 3. 绘制标题和装饰
        drawTitle(p);

        // 4. 绘制各个图表
        drawBarChart(p, QRect(60, 100, 450, 320));      // 柱状图
        drawPieChart(p, QRect(550, 100, 500, 320));     // 饼图（带图例）
        drawTable(p, QRect(60, 450, 990, 260));         // 数据表格
        drawSummary(p, QRect(60, 720, 990, 20));        // 底部总结
    }

private:
    QVector<AccountItem> m_data;

    void initData() {
        // 财务费用主要科目数据（单位：万元）
        m_data = {
                {"利息支出", 115.6, 0, "↑", QColor(231, 76, 60)},     // 红色
                {"汇兑损失", 82.3, 0, "↑", QColor(230, 126, 34)},    // 橙色
                {"手续费", 45.8, 0, "→", QColor(241, 196, 15)},      // 黄色
                {"现金折扣", 28.4, 0, "↓", QColor(46, 204, 113)},    // 绿色
                {"其他财务费用", 15.2, 0, "→", QColor(52, 152, 219)} // 蓝色
        };
    }

    void drawGradientBackground(QPainter& p) {
        // 深蓝色渐变背景，金融风格
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0.0, QColor(13, 27, 42));    // 深蓝黑
        gradient.setColorAt(0.5, QColor(22, 44, 69));    // 金融蓝
        gradient.setColorAt(1.0, QColor(31, 58, 88));    // 稍浅蓝

        p.fillRect(rect(), gradient);

        // 添加微弱的网格纹理
        p.setPen(QColor(255, 255, 255, 8));
        for (int x = 0; x < width(); x += 20) {
            p.drawLine(x, 0, x, height());
        }
        for (int y = 0; y < height(); y += 20) {
            p.drawLine(0, y, width(), y);
        }
    }

    void drawGrid(QPainter& p) {
        p.setPen(QColor(255, 255, 255, 15));

        // 主要网格线
        for (int x = 50; x < width(); x += 100) {
            p.drawLine(x, 0, x, height());
        }
        for (int y = 50; y < height(); y += 50) {
            p.drawLine(0, y, width(), y);
        }
    }

    void drawTitle(QPainter& p) {
        // 主标题
        QLinearGradient titleGrad(0, 0, width(), 0);
        titleGrad.setColorAt(0.0, QColor(64, 224, 208));   // 青色
        titleGrad.setColorAt(0.5, QColor(138, 43, 226));   // 紫色
        titleGrad.setColorAt(1.0, QColor(255, 105, 180));  // 粉色

        p.setFont(QFont("Microsoft YaHei", 24, QFont::Bold));
        p.setPen(QPen(titleGrad, 2));
        p.drawText(0, 0, width(), 70, Qt::AlignCenter,
                   "💰 财务会计科目对比分析");

        // 副标题
        p.setFont(QFont("Microsoft YaHei", 12));
        p.setPen(QColor(200, 220, 255, 200));
        p.drawText(0, 45, width(), 30, Qt::AlignCenter,
                   "财务费用构成分析 | 数据期间: 2025年9-12月 | 单位: 万元");

        // 装饰线
        p.setPen(QPen(QColor(100, 150, 255, 80), 1));
        p.drawLine(100, 65, width() - 100, 65);
        p.drawLine(100, 67, width() - 100, 67);
    }

    void drawBarChart(QPainter& p, const QRect& area) {
        // 图表背景
        drawChartBackground(p, area, "📈 财务费用科目金额对比");

        if (m_data.empty()) return;

        double maxAmount = m_data.front().amount;
        int barWidth = 50;
        int spacing = 30;
        int left = area.left() + 40;
        int bottom = area.bottom() - 40;
        int chartHeight = area.height() - 65;

        p.setPen(Qt::NoPen);

        for (int i = 0; i < m_data.size(); ++i) {
            double ratio = m_data[i].amount / maxAmount;
            int height = ratio * chartHeight;
            int x = left + i * (barWidth + spacing);

            // 柱状图3D效果（顶部高光 + 主体 + 底部阴影）
            QColor baseColor = m_data[i].color;

            // 主体柱状（带渐变）
            QLinearGradient barGrad(x, bottom - height, x, bottom);
            barGrad.setColorAt(0.0, baseColor.lighter(130));  // 顶部亮
            barGrad.setColorAt(0.7, baseColor);               // 中部原色
            barGrad.setColorAt(1.0, baseColor.darker(130));   // 底部暗

            p.setBrush(barGrad);
            p.drawRoundedRect(x, bottom - height, barWidth, height, 5, 5);

            // 顶部高光条
            p.setBrush(baseColor.lighter(180));
            p.drawRect(x + 2, bottom - height, barWidth - 4, 8);

            // 金额标签（柱顶）
            p.setPen(Qt::white);
            p.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
            QString amountStr = QString::number(m_data[i].amount, 'f', 1);
            p.drawText(x - 10, bottom - height - 25, barWidth + 20, 20,
                       Qt::AlignCenter, amountStr + "万");

            // 科目名称（底部）
            p.setFont(QFont("Microsoft YaHei", 9));
            QString name = m_data[i].name;
            p.drawText(x - 15, bottom + 5, barWidth + 30, 40,
                       Qt::AlignCenter | Qt::TextWordWrap, name);

            // 趋势箭头
            p.setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
            QColor trendColor = Qt::white;
            if (m_data[i].trend == "↑") trendColor = QColor(231, 76, 60);
            else if (m_data[i].trend == "↓") trendColor = QColor(46, 204, 113);

            p.setPen(trendColor);
            p.drawText(x + barWidth/2 - 5, bottom - height - 45, 20, 20,
                       Qt::AlignCenter, m_data[i].trend);
        }

        // Y轴刻度和标签
        p.setPen(QColor(200, 200, 255, 180));
        p.setFont(QFont("Microsoft YaHei", 9));
        for (int i = 0; i <= 5; i++) {
            double value = maxAmount * i / 5.0;
            int y = bottom - chartHeight * i / 5.0;
            p.drawLine(left - 8, y, left, y);
            p.drawText(left - 55, y - 10, 45, 20,
                       Qt::AlignRight | Qt::AlignVCenter,
                       QString::number(value, 'f', 0));
        }

        // 轴线
        p.setPen(QPen(QColor(255, 255, 255, 120), 1.5));
        p.drawLine(left, area.top() + 30, left, bottom);
        p.drawLine(left, bottom, area.right() - 20, bottom);
    }

    void drawPieChart(QPainter& p, const QRect& area) {
        drawChartBackground(p, area, "📊 费用构成占比分析");

        int totalItems = m_data.size();
        if (totalItems == 0) return;

        // 饼图中心
        int cx = area.left() + 140;
        int cy = area.center().y();
        int radius = 100;

        int startAngle = 0;

        // 先绘制阴影层
        for (int i = 0; i < totalItems; i++) {
            int spanAngle = 360 * m_data[i].ratio / 100;
            if (spanAngle <= 0) continue;

            p.save();
            p.translate(5, 5);
            p.setBrush(QColor(0, 0, 0, 80));
            p.setPen(Qt::NoPen);
            p.drawPie(cx - radius, cy - radius, radius * 2, radius * 2,
                      startAngle * 16, spanAngle * 16);
            p.restore();

            startAngle += spanAngle;
        }

        // 绘制实际饼图
        startAngle = 0;
        for (int i = 0; i < totalItems; i++) {
            int spanAngle = 360 * m_data[i].ratio / 100;
            if (spanAngle <= 0) continue;

            // 扇形渐变
            QConicalGradient conicGrad(cx, cy, -startAngle - spanAngle/2);
            conicGrad.setColorAt(0.0, m_data[i].color.lighter(150));
            conicGrad.setColorAt(0.5, m_data[i].color);
            conicGrad.setColorAt(1.0, m_data[i].color.darker(150));

            p.setBrush(conicGrad);
            p.setPen(QPen(Qt::white, 1));
            p.drawPie(cx - radius, cy - radius, radius * 2, radius * 2,
                      startAngle * 16, spanAngle * 16);

            // 在扇形中间显示百分比
            if (spanAngle > 20) {
                double midAngle = startAngle + spanAngle / 2.0;
                double rad = midAngle * 3.14159 / 180.0;
                int labelX = cx + (radius * 0.65) * cos(rad);
                int labelY = cy - (radius * 0.65) * sin(rad);

                p.setPen(Qt::white);
                p.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
                QString percent = QString::number(m_data[i].ratio, 'f', 1) + "%";
                p.drawText(labelX - 25, labelY - 10, 50, 20,
                           Qt::AlignCenter, percent);
            }

            startAngle += spanAngle;
        }

        // 饼图中间的圆（挖空效果）
        p.setBrush(QColor(13, 27, 42));
        p.setPen(Qt::NoPen);
        p.drawEllipse(cx - radius/2, cy - radius/2, radius, radius);

        // 图例（右侧）
        int legendX = area.left() + 280;
        int legendY = area.top() + 60;

        p.setFont(QFont("Microsoft YaHei", 10));
        for (int i = 0; i < totalItems; i++) {
            // 颜色方块
            p.setBrush(m_data[i].color);
            p.setPen(QColor(255, 255, 255, 100));
            p.drawRect(legendX, legendY, 15, 15);

            // 文本
            p.setPen(QColor(240, 240, 255));
            QString legendText = QString("%1 %2% (%3万)")
                    .arg(m_data[i].name)
                    .arg(m_data[i].ratio, 0, 'f', 1)
                    .arg(m_data[i].amount, 0, 'f', 1);

            p.drawText(legendX + 25, legendY, 180, 15,
                       Qt::AlignLeft | Qt::AlignVCenter, legendText);

            // 趋势
            p.setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
            QColor trendColor = (m_data[i].trend == "↑") ?
                                QColor(231, 76, 60) : QColor(46, 204, 113);
            p.setPen(trendColor);
            p.drawText(legendX + 190, legendY, 20, 15,
                       Qt::AlignCenter, m_data[i].trend);

            p.setFont(QFont("Microsoft YaHei", 10));
            legendY += 25;
        }

        // 中心标题
        p.setPen(QColor(200, 220, 255));
        p.setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
        p.drawText(cx - 40, cy - 5, 80, 20, Qt::AlignCenter, "构成比");
    }

    void drawTable(QPainter& p, const QRect& area) {
        drawChartBackground(p, area, "📋 财务费用明细分析表");

        int rowHeight = 40;
        int headerHeight = 35;
        int y = area.top() + 20;

        // 表头背景
        QLinearGradient headerGrad(area.left(), y, area.left(), y + headerHeight);
        headerGrad.setColorAt(0.0, QColor(52, 152, 219, 200));
        headerGrad.setColorAt(1.0, QColor(41, 128, 185, 200));

        p.setBrush(headerGrad);
        p.setPen(Qt::NoPen);
        p.drawRect(area.left(), y, area.width(), headerHeight);

        // 表头文字
        p.setPen(QColor(255, 255, 255));
        p.setFont(QFont("Microsoft YaHei", 12, QFont::Bold));

        QStringList headers = {"序号", "会计科目", "金额(万元)", "占比(%)", "趋势", "分析说明"};
        int widths[] = {60, 250, 120, 100, 80, 400};

        int x = area.left() + 10;
        for (int i = 0; i < headers.size(); i++) {
            Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter;

            // 根据不同列设置不同对齐方式
            switch(i) {
                case 0: // 序号 - 居中
                    align = Qt::AlignCenter | Qt::AlignVCenter;
                    break;
                case 2: // 金额 - 右对齐
                    align = Qt::AlignRight | Qt::AlignVCenter;
                    break;
                case 3: // 占比 - 居中
                    align = Qt::AlignCenter | Qt::AlignVCenter;
                    break;
                case 4: // 趋势 - 居中
                    align = Qt::AlignCenter | Qt::AlignVCenter;
                    break;
                default: // 其他列左对齐
                    align = Qt::AlignLeft | Qt::AlignVCenter;
            }

            p.drawText(x, y, widths[i], headerHeight, align, headers[i]);
            x += widths[i];
        }

        // 数据行
        y += headerHeight;
        p.setFont(QFont("Microsoft YaHei", 10));

        for (int i = 0; i < m_data.size(); i++) {
            // 交替行背景
            if (i % 2 == 0) {
                p.setBrush(QColor(255, 255, 255, 20));
            } else {
                p.setBrush(QColor(255, 255, 255, 8));
            }
            p.setPen(Qt::NoPen);
            p.drawRect(area.left(), y, area.width(), rowHeight);

            x = area.left() + 10;

            // 序号
            p.setPen(QColor(200, 220, 255));
            p.drawText(x, y, widths[0], rowHeight,
                       Qt::AlignCenter | Qt::AlignVCenter, QString::number(i + 1));
            x += widths[0];

            // 科目名称
            p.setPen(Qt::white);
            p.drawText(x, y, widths[1], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, m_data[i].name);
            x += widths[1];

            // 金额（颜色根据数值大小）
            double amount = m_data[i].amount;
            if (amount > 100) p.setPen(QColor(231, 76, 60));     // 红色
            else if (amount > 50) p.setPen(QColor(230, 126, 34)); // 橙色
            else p.setPen(QColor(46, 204, 113));                // 绿色

            QString amountStr = QString::number(amount, 'f', 1);
            p.drawText(x, y, widths[2], rowHeight,
                       Qt::AlignRight | Qt::AlignVCenter, amountStr);
            x += widths[2];

            // 占比
            p.setPen(QColor(174, 214, 241));
            p.drawText(x, y, widths[3], rowHeight,
                       Qt::AlignCenter | Qt::AlignVCenter,
                       QString::number(m_data[i].ratio, 'f', 1) + "%");
            x += widths[3];

            // 趋势（带箭头）
            QColor trendColor = (m_data[i].trend == "↑") ?
                                QColor(231, 76, 60) : QColor(46, 204, 113);
            p.setPen(trendColor);
            p.setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
            p.drawText(x, y, widths[4], rowHeight,
                       Qt::AlignCenter | Qt::AlignVCenter, m_data[i].trend);
            x += widths[4];

            // 分析说明（根据数据生成）
            p.setFont(QFont("Microsoft YaHei", 9));
            p.setPen(QColor(220, 220, 220));
            QString analysis = generateAnalysis(i);
            p.drawText(x, y, widths[5], rowHeight,
                       Qt::AlignLeft | Qt::AlignVCenter, analysis);

            y += rowHeight;
        }
    }

    void drawSummary(QPainter& p, const QRect& area) {
        // 计算总计
        double total = 0;
        for (const auto& item : m_data) total += item.amount;

        QString summary = QString("📊 分析总结: 本期财务费用总额 %1 万元，其中利息支出占比最高，建议优化融资结构。")
                .arg(total, 0, 'f', 1);

        p.setPen(QColor(255, 255, 255, 180));
        p.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        p.drawText(area, Qt::AlignLeft | Qt::AlignVCenter, summary);
    }

    QString generateAnalysis(int index) {
        switch(index) {
            case 0: return "主要融资成本，受利率政策影响";
            case 1: return "汇率波动导致，需加强外汇风险管理";
            case 2: return "银行手续费等，相对稳定";
            case 3: return "供应商现金折扣，有所减少";
            case 4: return "其他零星费用，占比最小";
            default: return "正常业务发生";
        }
    }

    void drawChartBackground(QPainter& p, const QRect& area, const QString& title) {
        // 1. 外阴影（向右下偏移）
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 25));
        p.drawRoundedRect(area.translated(2, 2), 12, 12);

        // 2. 主背景
        QLinearGradient bgGrad(area.topLeft(), area.bottomRight());
        bgGrad.setColorAt(0.0, QColor(255, 255, 255, 10));
        bgGrad.setColorAt(1.0, QColor(255, 255, 255, 25));
        p.setBrush(bgGrad);
        p.setPen(QPen(QColor(100, 150, 255, 80), 1.5));
        p.drawRoundedRect(area, 12, 12);

        // 3. 内边框（高光效果）
        p.setPen(QPen(QColor(255, 255, 255, 40), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(area.adjusted(1, 1, -1, -1), 11, 11);

        // 标题
        p.setPen(QColor(220, 240, 255));
        p.setFont(QFont("Microsoft YaHei", 13, QFont::Bold));
        p.drawText(area.left(), area.top() - 5, area.width(), 30,
                   Qt::AlignCenter, title);
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 设置中文字体
    QFont font("Microsoft YaHei");
    app.setFont(font);

    FinanceAnalysisViz w;
    w.show();

    return app.exec();
}