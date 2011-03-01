////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 Mario Negri Institute & Orobix Srl
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

#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

#include <QWidget>

namespace Ui {
    class ResultsView;
}

class ResultsView : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsView(QString cookie, QPixmap pressurePic, QPixmap flowPic, QString PMean,
                         QString FMean, QWidget *parent = 0);
    ~ResultsView();

private:
    Ui::ResultsView *ui;
    void setResultsCookie(QString theCookie);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

signals:
    void okButtonClicked(QString cookie);
    void deleteItself();

private slots:
    void okButtClicked();
    void resizeLabels();

private:
    QString resultsCookie;
    QPixmap pressPix, flPix;
};

#endif // RESULTSVIEW_H
