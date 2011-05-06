////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2011 Orobix Srl & Mario Negri Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include "resultsgroupbox.h"

#include <QTextStream>
using namespace std;

QTextStream resGrOut(stdout);

ResultsGroupBox::ResultsGroupBox(QWidget *parent) :
    QWidget(parent)
{
    //Q_INIT_RESOURCE(ResultsGroupBox);
    setupUi(this);
    connect(resultsChoiceFrame, SIGNAL(res2BeDisplayedChanged(int)), this, SIGNAL(result2BeDisaplyedChanged(int)));

    resTabs->hide();
}

void ResultsGroupBox::addResultsView(ResultsView* theResultsView, QPoint elementRequest)
{
       resGrOut << "ResultsGroupBox::addResultsView" << endl;
       QString element;

       /*if (elementRequest.x() == 1) {
           element = "node ";
       } else if (elementRequest.x() == 2) {
           element = "edge ";
       }*/

       element = "MeshNode ";

       QString idString;
       idString.setNum(elementRequest.y());
       element += idString;

       resultsViewList.insert(theResultsView, elementRequest);

       resTabs->insertTab(0, theResultsView, element);
       resTabs->setCurrentWidget(theResultsView);
       ////emit editingEl2Ws(elementRequest);
       //resultsDock->setWidget(resultsTabs);
       resTabs->show();
}

void ResultsGroupBox::clear()
{
    resultsViewList.clear();
    resTabs->clear();
}

void ResultsGroupBox::removeAllResultsViewFromResultsDock()
{
    resultsViewList.clear();
    resTabs->clear();
    QPoint temp(-1, -1);
    emit visualizingEl2Ws(temp);
}

void ResultsGroupBox::removeResultsViewFromResultsDock()
{
    resultsViewList.remove(resTabs->currentWidget());
    resTabs->removeTab(resTabs->currentIndex());
    if (resTabs->count() == 0) {
        resTabs->hide();
    }
}

void ResultsGroupBox::setPageInResultsTab(QWidget *theResultsView)
{
    resTabs->setCurrentWidget(theResultsView);
    emit visualizingEl2Ws(resultsViewList.value(resTabs->currentWidget()));
}

QPoint ResultsGroupBox::currVListValue()
{
    return resultsViewList.value(resTabs->currentWidget());
}

QMap<QWidget*, QPoint> ResultsGroupBox::resultsVList()
{
    return resultsViewList;
}
