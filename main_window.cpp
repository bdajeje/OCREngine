#include "main_window.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>

namespace {
  constexpr int FILTER_LIMIT = 150;
  constexpr int IMAGE_START_X = 20;
  constexpr int IMAGE_START_Y = 20;
  constexpr float IMAGE_WIDTH_RATIO = 0.65;
  constexpr float IMAGE_HEIGHT_RATIO = 0.07;
  const char* CUT_IMAGE_FILEPATH_PREFIX = "/tmp/magic_cut_image_";
  const char* IMAGE_NAME_OUTPUT_FILEPATH = "/tmp/magic_image_name";

  QString filterResult(QString input)
  {
    QString result;
std::cout << input.toStdString() << std::endl;
std::cout << "-------------------------------" << std::endl;
    for(char character : input.toStdString())
    { std::cout << character << std::endl;
      if((character >= 48 && character <= 57) ||
         (character >= 97 && character <= 122) ||
         (character >= 65 && character <= 90) ||
         character == ' ') { std::cout <<  "YES"<< std::endl;
        result.append(character); }
    }
std::cout << "-------------------------------" << std::endl;
std::cout << result.toStdString() << std::endl;
    return result;
  }

  QPixmap filterImage(const QPixmap& input)
  {
    QImage image = input.toImage();
    int width = image.width();
    int height = image.height();
    QColor null_color {255, 255, 255};
    QRgb null_rgb = null_color.rgb();

    for(int y = 0; y < height; ++y)
    {
      for(int x = 0; x < width; ++x)
      {
        QColor color (image.pixel(x, y));
        if(color.red() > FILTER_LIMIT || color.green() > FILTER_LIMIT || color.blue() > FILTER_LIMIT)
          image.setPixel(x, y, null_rgb);
      }
    }

    return QPixmap::fromImage(image);
  }
}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  auto central_widget = new QWidget;
  auto main_layout = new QVBoxLayout(central_widget);

  _cut_image = new QLabel;
  _resulted_card_name = new QLabel;

  auto select_file_btn = new QPushButton("Select Image");
  connect(select_file_btn, SIGNAL(pressed()), this, SLOT(selectImageFile()));

  main_layout->addWidget(select_file_btn);
  main_layout->addWidget(_cut_image);
  main_layout->addWidget(_resulted_card_name);
  setCentralWidget(central_widget);
}

void MainWindow::selectImageFile()
{
  QString file_path = QFileDialog::getOpenFileName(this,
      tr("Open Image"), "./images", tr("Image Files (*.png *.jpg *.bmp)"));

  if(file_path.isEmpty())
    return;

  QFile image_file(file_path);
  if(!image_file.exists())
  {
    error("Incorrect file");
    return;
  }

  QPixmap image (file_path);
  auto image_size = image.size();
  int copy_width = image_size.width() * IMAGE_WIDTH_RATIO;
  int copy_height = image_size.height() * IMAGE_HEIGHT_RATIO;
  _cut_image_pixmap = filterImage(image.copy(IMAGE_START_X, IMAGE_START_Y, copy_width, copy_height));
  _cut_image->setPixmap(_cut_image_pixmap);

  if(_cut_image_pixmap.isNull())
    error(QString("para "));

  std::string cut_image_filepath = CUT_IMAGE_FILEPATH_PREFIX + QFileInfo(image_file).fileName().toStdString();
  if(!_cut_image_pixmap.save(cut_image_filepath.c_str()))
  {
    error(QString("Can't save cut image to: ") + cut_image_filepath.c_str());
    return;
  }

  system((std::string("tesseract -l eng ") + cut_image_filepath +" "+ std::string(IMAGE_NAME_OUTPUT_FILEPATH)).c_str());

  QTimer::singleShot(2000, this, SLOT(readResultFile()));
  _resulted_card_name->setText("Processing...");
}

void MainWindow::readResultFile()
{
  QFile result_file(QString(IMAGE_NAME_OUTPUT_FILEPATH) + ".txt");

  if(!result_file.exists() || !result_file.open(QIODevice::ReadOnly))
  {
    error("Incorrect result file not found: " + result_file.fileName());
    return;
  }

  _resulted_card_name->setText(filterResult(result_file.readAll()));
}

void MainWindow::error(const QString& error)
{
  QMessageBox::warning(this, "Error", error);
  _resulted_card_name->setText("");
}
