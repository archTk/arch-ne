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

#ifndef RESULTSCHOICEFRAME_H
#define RESULTSCHOICEFRAME_H

#include <QFrame>
#include "ui_resultschoiceframe.h"

namespace Ui {
    class ResultsChoiceFrame;
}

class ResultsChoiceFrame : public QFrame, private Ui::ResultsChoiceFrame
{
    Q_OBJECT

public:
    explicit ResultsChoiceFrame(QWidget *parent = 0);

signals:
    void res2BeDisplayedChanged(int theResults);

public slots:
    void changeDisplayedResult(int newDisplayedResult);

private:

};

#endif // RESULTSCHOICEFRAME_H
