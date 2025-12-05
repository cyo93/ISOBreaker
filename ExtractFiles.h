#ifndef EXTRACT_FILES_H
#define EXTRACT_FILES_H
#include <QtWidgets/QDialog>
#include <QComboBox>
#include "ui_ExtractFiles.h"

class ExtractFiles : public QDialog {
	Q_OBJECT
public:
	void updateProgressBar(size_t type,size_t value);

public slots:
	void cancelExtraction();

private:
	
};

#endif