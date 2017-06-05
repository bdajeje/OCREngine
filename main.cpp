#include "main_window.hpp"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
	tesseract::TessBaseAPI *tesseract = new tesseract::TessBaseAPI();

	// Initialize tesseract-ocr with English, without specifying tessdata path
	if(tesseract->Init(NULL, "eng") != 0)
	{
		std::cerr << "Could not initialize tesseract" << std::endl;
		return EXIT_FAILURE;
	}

  QApplication a(argc, argv);
  MainWindow w(tesseract);
  w.show();

  tesseract->End();
  delete tesseract;

  return a.exec();
}
