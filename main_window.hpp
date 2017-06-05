#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <set>

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QDoubleSpinBox>

#include <tesseract/baseapi.h>

class MainWindow final : public QMainWindow
{
  Q_OBJECT

  public:

	MainWindow(tesseract::TessBaseAPI* tesseract);

  private:

	void error(const QString& error, bool reset = true);
	void loadPreviousValues();
	void saveValues();

  private slots:

	void selectImageFile();
	void selectDeckFile();
	void submit();
	bool foundCardName(const char* read_card_name);
	void acceptCard();
	void refuseCard();

  private:

	tesseract::TessBaseAPI* _tesseract;
	QLabel* _cut_image;
	QLabel* _resulted_card_name;
	QLabel* _selected_image_name;
	QLabel* _selected_deck_name;
	QLabel* _result_text;
	QSpinBox* _x_input;
	QSpinBox* _y_input;
	QDoubleSpinBox* _width_input;
	QDoubleSpinBox* _heigth_input;
	QPixmap _cut_image_pixmap;
	QString _file_path;

	/* Map card name to number of expected cards with this name */
	std::map<std::string, size_t> _expected_cards;
};

#endif // MAIN_WINDOW_HPP
