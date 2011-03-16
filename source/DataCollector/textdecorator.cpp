////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2011 Mario Negri Institute & Orobix Srl
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

#include "textdecorator.h"

TextDecorator::TextDecorator(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    styleSet styleset;

    elementStyle.setForeground(QColor("#2244ee"));
    elementStyle.setFontWeight(QFont::Bold);
    styleset.regexp = QRegExp("(<[A-Za-z0-9:_]+\\b|<\\?[A-Za-z0-9:_]+\\b|\\?>|>|/>|</[A-Za-z0-9:_]+>)");
    styleset.format = elementStyle;
    styleSettings.append(styleset);

    attNameStyle.setForeground(QColor("#22aa44"));
    attNameStyle.setFontWeight(QFont::Bold);
    styleset.regexp = QRegExp("[A-Za-z0-9:_]+=");
    styleset.format = attNameStyle;
    styleSettings.append(styleset);

    attValStyle.setForeground(QColor("#ff2244"));
    styleset.regexp = QRegExp("(\"[^\"]*\"|'[^']*')");
    styleset.format = attValStyle;
    styleSettings.append(styleset);
}

void TextDecorator::highlightBlock(const QString &text)
{
     foreach (const styleSet &styleset, styleSettings) {
         QRegExp expression(styleset.regexp);
         int index = text.indexOf(expression);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, styleset.format);
             index = text.indexOf(expression, index + length);
         }
     }
}
