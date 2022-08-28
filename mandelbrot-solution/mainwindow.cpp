#include "mainwindow.h"

#include "config.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowTitle("Mandelrot Fractal");
    setWindowIcon(QIcon(":/icon.png"));

    ui->setupUi(this);

    // Performance Options setup
    ui->iterationsLimitSpinBox->setValue(config::defaultValues::iterationsLimit);
    connect(ui->iterationsLimitSpinBox,
            &QSpinBox::valueChanged,
            ui->mandelbrotWidget,
            &MandelbrotWidget::iterationsLimitChanged);

    // Colors Options setup
    ui->redSpinBox->setValue(config::defaultValues::redIntensity);
    connect(ui->redSpinBox,
            &QSpinBox::valueChanged,
            ui->mandelbrotWidget,
            &MandelbrotWidget::redIntensityChanged);

    ui->greenSpinBox->setValue(config::defaultValues::greenIntensity);
    connect(ui->greenSpinBox,
            &QSpinBox::valueChanged,
            ui->mandelbrotWidget,
            &MandelbrotWidget::greenIntensityChanged);

    ui->blueSpinBox->setValue(config::defaultValues::blueIntensity);
    connect(ui->blueSpinBox,
            &QSpinBox::valueChanged,
            ui->mandelbrotWidget,
            &MandelbrotWidget::blueIntensityChanged);

    // "Save Screenshot" button setup
    connect(ui->saveScreenshotButton,
            &QPushButton::clicked,
            ui->mandelbrotWidget,
            &MandelbrotWidget::saveScreenshot);
}

MainWindow::~MainWindow()
{
    delete ui;
}
