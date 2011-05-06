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

#ifndef RESULTSGROUPBOX_H
#define RESULTSGROUPBOX_H

#include <QWidget>
#include "ui_resultsgroupbox.h"

#include "resultsview.h"

namespace Ui {
    class ResultsGroupBox;
}

class ResultsGroupBox : public QWidget, private Ui::ResultsGroupBox
{
    Q_OBJECT

public:
    explicit ResultsGroupBox(QWidget *parent = 0);

    void addResultsView(ResultsView* theResultsView, QPoint elementRequest);
    void clear();
    void removeAllResultsViewFromResultsDock();
    void removeResultsViewFromResultsDock();
    void setPageInResultsTab(QWidget *theResultsView);
    void resTabsContentChanged();

    QPoint currVListValue();
    QMap<QWidget*, QPoint> resultsVList();

signals:
    void result2BeDisaplyedChanged(int theResult);
    void visualizingEl2Ws(QPoint editingElement);

private:

    QMap<QWidget*, QPoint> resultsViewList;

};

#endif // RESULTSGROUPBOX_H
