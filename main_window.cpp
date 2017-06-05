#include "main_window.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>

#include <leptonica/allheaders.h>

#include <boost/algorithm/string.hpp>

#include <utils/files.hpp>
#include <utils/json.hpp>

namespace {
  constexpr int FILTER_LIMIT = 150;
  const char* CUT_IMAGE_FILEPATH_PREFIX = "/tmp/magic_cut_image_";
  const char* SETTING_FILEPATH = "./assets/values";

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

MainWindow::MainWindow(tesseract::TessBaseAPI* tesseract)
  : QMainWindow()
  , _tesseract {tesseract}
{
  auto central_widget = new QWidget;
  auto main_layout = new QVBoxLayout(central_widget);

  auto size_layout = new QHBoxLayout;
  _x_input = new QSpinBox;
  _x_input->setPrefix("x: ");
  _x_input->setRange(0, 99999);
  _y_input = new QSpinBox;
  _y_input->setPrefix("y: ");
  _y_input->setRange(0, 99999);
  size_layout->addWidget(_x_input);
  size_layout->addWidget(_y_input);

  auto positions_layout = new QHBoxLayout;
  _width_input = new QDoubleSpinBox;
  _width_input->setPrefix("w: ");
  _width_input->setRange(0, 1);
  _heigth_input = new QDoubleSpinBox;
  _heigth_input->setPrefix("h: ");
  _heigth_input->setRange(0, 1);
  positions_layout->addWidget(_width_input);
  positions_layout->addWidget(_heigth_input);

  _cut_image = new QLabel;
  _resulted_card_name = new QLabel;
  _selected_image_name = new QLabel;
  _selected_deck_name = new QLabel;
  _result_text = new QLabel;
  auto submit_btn = new QPushButton("Submit");
  auto select_file_btn = new QPushButton("Select Image");
  auto select_deckfile_btn = new QPushButton("Select Deck");

  connect(select_file_btn, SIGNAL(pressed()), this, SLOT(selectImageFile()));
  connect(select_deckfile_btn, SIGNAL(pressed()), this, SLOT(selectDeckFile()));
  connect(submit_btn, SIGNAL(pressed()), this, SLOT(submit()));

  main_layout->addLayout(size_layout);
  main_layout->addLayout(positions_layout);
  main_layout->addWidget(select_file_btn);
  main_layout->addWidget(_selected_image_name);
  main_layout->addWidget(select_deckfile_btn);
  main_layout->addWidget(_selected_deck_name);
  main_layout->addWidget(_cut_image);
  main_layout->addWidget(_resulted_card_name);
  main_layout->addWidget(_result_text);
  main_layout->addWidget(submit_btn);
  setCentralWidget(central_widget);

  loadPreviousValues();
}

void MainWindow::loadPreviousValues()
{
	json json_values = json::parse(files::read(SETTING_FILEPATH));
	_x_input->setValue(json_values["x"]);
	_y_input->setValue(json_values["y"]);
	_width_input->setValue(json_values["w"]);
	_heigth_input->setValue(json_values["h"]);
}

void MainWindow::saveValues()
{
	json values;
	values["x"] = _x_input->value();
	values["y"] = _y_input->value();
	values["w"] = _width_input->value();
	values["h"] = _heigth_input->value();

	if(!files::create(SETTING_FILEPATH, values.dump()))
		error("Can't save preferences", false);
}

void MainWindow::submit()
{
	if(_file_path.isEmpty())
	{
	  error("Select an image first");
	  return;
	}

	QFileInfo image_file(_file_path);
	if(!image_file.exists())
	{
	  error("Incorrect file");
	  return;
	}

	saveValues();

	QPixmap image (_file_path);
	auto image_size = image.size();
	int copy_width = image_size.width() * _width_input->value();
	int copy_height = image_size.height() * _heigth_input->value();
  //  _cut_image_pixmap = filterImage(image.copy(IMAGE_START_X, IMAGE_START_Y, copy_width, copy_height));
	_cut_image_pixmap = image.copy(_x_input->value(), _y_input->value(), copy_width, copy_height);
	_cut_image->setPixmap(_cut_image_pixmap);

	if(_cut_image_pixmap.isNull())
	{
	  error(QString("Invalid cuted image"));
	  return;
	}

	std::string cut_image_filepath = CUT_IMAGE_FILEPATH_PREFIX + image_file.fileName().toStdString();
	if(!_cut_image_pixmap.save(cut_image_filepath.c_str()))
	{
	  error(QString("Can't save cut image to: ") + cut_image_filepath.c_str());
	  return;
	}

	// Open input image with leptonica library
	Pix* leptonica_image = pixRead(cut_image_filepath.c_str());
	_tesseract->SetImage(leptonica_image);

	// Get OCR result
	char* outText = _tesseract->GetUTF8Text();
	_resulted_card_name->setText(filterResult(outText));

	foundCardName(outText);

	// Clean
	pixDestroy(&leptonica_image);
	delete[] outText;
}

void MainWindow::selectImageFile()
{
  _file_path = QFileDialog::getOpenFileName(this,
	  tr("Open Image"), "./images/nani", tr("Image Files (*.png *.jpg *.bmp)"));

  if(_file_path.isEmpty())
	return;

  _selected_image_name->setText(QFileInfo(_file_path).fileName());
}

void MainWindow::selectDeckFile()
{
  QString file_path = QFileDialog::getOpenFileName(this, tr("Open Deck File"));

  if(file_path.isEmpty())
	return;

  _selected_deck_name->setText(QFileInfo(file_path).fileName());

  std::string deck_content = files::read(file_path.toStdString());
  std::vector<std::string> splited_content;
  boost::algorithm::split(splited_content, deck_content, boost::is_any_of("\n"));

  for(const std::string& card_name : splited_content)
  {
	  if(card_name.empty())
		  continue;

	  auto found = _expected_cards.find(card_name);
	  if(found == _expected_cards.end())
		  _expected_cards[card_name] = 1;
	  else
		  ++(found->second);
  }
}

void MainWindow::error(const QString& error, bool reset)
{
  QMessageBox::warning(this, "Error", error);

  if(reset)
	_resulted_card_name->setText("");
}

bool MainWindow::foundCardName(const char* read_card_name)
{
	auto found = _expected_cards.find(read_card_name);

	if(found != _expected_cards.end() && found->second > 0)
	{
		--(found->second);
		acceptCard();
		return true;
	}

	refuseCard();
	return false;
}

void MainWindow::acceptCard()
{
	_result_text->setText("Accepted");
	_result_text->setStyleSheet("color: green;");
}

void MainWindow::refuseCard()
{
	_result_text->setText("Refused");
	_result_text->setStyleSheet("color: red;");
}
