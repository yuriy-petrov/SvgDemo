#pragma once

#include "QDomDocument"
#include "QSvgRenderer"
#include <QWidget>

class DemoSvgWidget : public QWidget
{
    Q_OBJECT

public:
    DemoSvgWidget(QWidget * parent = nullptr);

    QString load(const QString & filename);

    QStringList ids() const;

    void select(const QStringList & ids);

signals:
    void selected(const QString & id);

protected:
    // QWidget interface
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void repaint();
    void updateCache();

private:
    QDomDocument _document;
    QSvgRenderer _renderer;
    QImage _cache;
    QStringList _ids;
};

