#include "demosvgwidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

namespace  {

void collectIds(QDomNode node, const QSvgRenderer & renderer, QStringList & ids)
{
    auto element = node.toElement();
    if (element.hasAttribute("id")) {
        auto id = element.attribute("id");
        if (renderer.elementExists(id)) {
            ids << id;
        }
    }
    auto children = node.childNodes();
    for (int i = 0; i < children.count(); ++i) {
        collectIds(children.at(i), renderer, ids);
    }
}

QDomNode findNodeById(const QString & id, QDomNode node)
{
    auto element = node.toElement();
    if (element.hasAttribute("id")) {
        if (element.attribute("id") == id) {
            return node;
        }
    }
    auto children = node.childNodes();
    for (int i = 0; i < children.count(); ++i) {
        auto childNode = findNodeById(id, children.at(i));
        if (!childNode.isNull()) {
            return childNode;
        }
    }
    return QDomNode();
}

}

DemoSvgWidget::DemoSvgWidget(QWidget* parent)
    : QWidget(parent)
{
    _cache = QImage(size(), QImage::Format_ARGB32);

    connect(&_renderer, &QSvgRenderer::repaintNeeded, this, [this]() {
        repaint();
        update();
    });
}

QString DemoSvgWidget::load(const QString& filename)
{
    QString error;
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        if (!_document.setContent(f.readAll(), true, &error)) {
            qCritical() << error;
            return error;
        }
        if (!_renderer.load(_document.toByteArray())) {
            error = "Error loading data to QSvgRenderer";
            qCritical() << error;

        }
        _ids.clear();
        collectIds(_document, _renderer, _ids);
    }
    return error;
}

QStringList DemoSvgWidget::ids() const
{
    return _ids;
}

void DemoSvgWidget::select(const QStringList& ids)
{
    for (const auto & id : _ids) {
        auto node = findNodeById(id, _document);
        if (!node.isNull()) {
            auto element = node.toElement();
            if (ids.contains(id)) {
                element.setAttribute("style", "stroke:#ff0038;stroke-opacity:1;stroke-width:10;stroke-miterlimit:1.5;stroke-dasharray:none");
            } else {
                element.removeAttribute("style");
            }
        }
    }
    _renderer.load(_document.toByteArray());
    repaint();
    update();
}

void DemoSvgWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(rect(), _cache);
}

void DemoSvgWidget::resizeEvent(QResizeEvent* event)
{
    _cache = QImage(event->size(), QImage::Format_ARGB32);
    repaint();
}

void DemoSvgWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        auto viewBox = _renderer.viewBoxF();
        auto scale = QMatrix().scale(viewBox.width() / _cache.width(), viewBox.height() / _cache.height() );
        auto viewPoint = scale.map(event->pos());
        QString selectedId;
        qreal selectedRectArea = 0.0;
        for (const auto & id : _ids) {
            auto rect = _renderer.matrixForElement(id).mapRect(_renderer.boundsOnElement(id));
            auto rectArea = rect.width() * rect.height();
            if (selectedRectArea == 0.0 || (rect.contains(viewPoint) && rectArea < selectedRectArea)) {
                selectedId = id;
                selectedRectArea = rectArea;
            }
        }
        emit selected(selectedId);
    }
}

void DemoSvgWidget::repaint()
{
    _cache.fill(Qt::white);
    QPainter painter(&_cache);
    _renderer.render(&painter, _cache.rect());
}

