#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setSizes({400});


    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
        auto items = ui->listWidget->selectedItems();
        QStringList ids;
        std::transform(items.cbegin(), items.cend(), std::back_inserter(ids), [](const QListWidgetItem * item) {
            return item->text();
        });
        ui->widget->select(ids);
    });

    connect(ui->widget, &DemoSvgWidget::selected, this, [this](const QString & id) {
        ui->listWidget->clearSelection();
        auto items = ui->listWidget->findItems(id, Qt::MatchExactly);
        if (!items.isEmpty()) {
            items.first()->setSelected(true);
        }
    });

    load(":/space");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QSettings s;
    auto path = s.value("path").toString();
    if (path.isEmpty()) {
        path = QApplication::instance()->applicationDirPath();
    }
    auto filename = QFileDialog::getOpenFileName(this, "Select SVG file", path, "*svg");
    if (!filename.isEmpty()) {
        path = QFileInfo(filename).absolutePath();
        s.setValue("path", path);
        load(filename);
    }
}

void MainWindow::load(const QString& filename)
{
    ui->listWidget->clear();
    ui->statusBar->showMessage(ui->widget->load(filename));
    for (const auto & id : ui->widget->ids()) {
        ui->listWidget->addItem(id);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QApplication::aboutQt();
}
