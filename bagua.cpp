#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <cmath>
#include <vector>

class BaguaDiagram : public QWidget {
private:
    double rotation = 0.0;
    bool animate = true;
    QTimer *timer;

    std::vector<std::vector<int>> trigrams = {
            {1,1,1}, {0,0,0}, {1,0,0}, {0,1,0},
            {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1}
    };

    std::vector<QString> trigramNames = {
            "乾 天 西北", "坤 地 西南", "震 雷 东", "坎 水 北",
            "艮 山 东北", "巽 风 东南", "离 火 南", "兑 泽 西"
    };

public:
    BaguaDiagram(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("太极八卦图");
        resize(700, 750);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            rotation += 0.5;
            if (rotation >= 360) rotation = 0;
            update();
        });

        if (animate) timer->start(16);

        setStyleSheet("background: #0c2461;");
    }

    void toggleAnimation() {
        animate = !animate;
        animate ? timer->start(16) : timer->stop();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int cx = width() / 2;
        int cy = height() / 2;
        int r = qMin(width(), height()) / 3;

        // 绘制太极
        painter.save();
        if (animate) {
            painter.translate(cx, cy);
            painter.rotate(rotation);
            painter.translate(-cx, -cy);
        }

        // 外圆
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(Qt::black);
        painter.drawEllipse(cx - r, cy - r, r * 2, r * 2);

        // 阴阳鱼 (简化版：两个半圆)
        painter.setBrush(Qt::white);
        painter.drawChord(cx - r, cy - r, r * 2, r * 2, 0, 180 * 16);

        // 阴眼阳眼
        painter.setBrush(Qt::black);
        painter.drawEllipse(cx + r/2 - r/8, cy - r/8, r/4, r/4);
        painter.setBrush(Qt::white);
        painter.drawEllipse(cx - r/2 - r/8, cy - r/8, r/4, r/4);

        painter.restore();

        // 绘制八卦符号
        painter.setPen(QPen(QColor(255,215,0), 2));
        for (int i = 0; i < 8; i++) {
            double angle = i * M_PI / 4 - M_PI/2;
            int tx = cx + r * 1.2 * cos(angle);
            int ty = cy + r * 1.2 * sin(angle);

            // 简单绘制三条线
            painter.save();
            painter.translate(tx, ty);
            painter.rotate(angle * 180 / M_PI + 90);

            for (int j = 0; j < 3; j++) {
                int y = -30 + j * 20;
                if (trigrams[i][j] == 1) {
                    painter.drawLine(-20, y, 20, y);  // 实线
                } else {
                    painter.drawLine(-20, y, -5, y);  // 虚线1
                    painter.drawLine(5, y, 20, y);    // 虚线2
                }
            }

            // 名称
            QFont font = painter.font();
            font.setPointSize(8);
            painter.setFont(font);
            painter.setPen(Qt::white);
            painter.drawText(-40, 40, 80, 40, Qt::AlignCenter, trigramNames[i]);

            painter.restore();
        }

        // 标题
        painter.setPen(Qt::yellow);
        QFont titleFont = painter.font();
        titleFont.setPointSize(20);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, "太极八卦图");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("易经八卦图演示-作者(冷溪虎山)");
    window.resize(720, 770);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    BaguaDiagram *bagua = new BaguaDiagram();
    layout->addWidget(bagua);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *toggleBtn = new QPushButton("切换动画");
    QPushButton *infoBtn = new QPushButton("64爻卦说明");

    QObject::connect(toggleBtn, &QPushButton::clicked,
                     [bagua]() { bagua->toggleAnimation(); });
    QObject::connect(infoBtn, &QPushButton::clicked, []() {
        QMessageBox::information(nullptr, "说明", "太极八卦 - Qt绘制");
    });

    btnLayout->addWidget(toggleBtn);
    btnLayout->addWidget(infoBtn);
    btnLayout->addStretch();

    layout->addLayout(btnLayout);

    window.show();
    return app.exec();
}