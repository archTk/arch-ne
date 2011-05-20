////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 Orobix Srl & Mario Negri Institute
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

namespace Ui {
    class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = 0);
    ~InfoDialog();

    void initWithMessage(QString message);
    void changeDisplayedMessage(QString theMessage);
    void closeEvent(QCloseEvent* event);

signals:
    void abortSimulation();
    void minimizeApp();
    void maximizeApp();
    void mouseEnteredInInfoDialog();
    void mouseLeftInfoDialog();

private:
    Ui::InfoDialog *ui;
    void changeEvent(QEvent* event);

public slots:
    void abortButtonReleased();
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
};

#endif // INFODIALOG_H
