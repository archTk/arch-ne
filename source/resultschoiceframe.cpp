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

#include "resultschoiceframe.h"

#include <QTextStream>
using namespace std;

QTextStream choiceOut(stdout);

ResultsChoiceFrame::ResultsChoiceFrame(QWidget *parent) :
    QFrame(parent)
{
    setupUi(this);
    resultsCombo->setCurrentIndex(0);

    connect(resultsCombo, SIGNAL(activated(int)), this, SLOT(changeDisplayedResult(int)));
}

void ResultsChoiceFrame::changeDisplayedResult(int newDisplayedResult)
{
    choiceOut << "ResultsChoiceFrame::changeDisplayedResults newResults= " << newDisplayedResult << endl;
    emit res2BeDisplayedChanged(newDisplayedResult);
}
