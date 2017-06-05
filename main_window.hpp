#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QDoubleSpinBox>

class MainWindow final : public QMainWindow
{
  Q_OBJECT

  public:

	MainWindow(QWidget *parent = 0);

  private:

	void error(const QString& error, bool reset = true);
	void loadPreviousValues();
	void saveValues();

  private slots:

	void selectImageFile();
	void readResultFile();
	void submit();

  private:

	QLabel* _cut_image;
	QLabel* _resulted_card_name;
	QLabel* _selected_image_name;
	QSpinBox* _x_input;
	QSpinBox* _y_input;
	QDoubleSpinBox* _width_input;
	QDoubleSpinBox* _heigth_input;
	QPixmap _cut_image_pixmap;
	QString _file_path;
};

#endif // MAIN_WINDOW_HPP
