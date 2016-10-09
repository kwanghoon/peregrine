// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma

#include <QString>
#include <QGlobal.h>
#include <algorithm>
#include <array>

namespace SuggestionAlgorithm
{
    // determine whether |keyword| is matched with |words|
    std::pair<bool, QString> matchKeyword(const QStringList& words, const QString& keyword)
    {
        if (words.empty())
        {
            return{ false, {} };
        }

        // collect indices of capital letter
        std::array<int, 10> indices;
        int cnt = 0;

        for (int i = 0; i < keyword.length(); i++)
        {
            if (keyword[i].isUpper())
            {
                indices[cnt++] = i;
            }
        }
        if (cnt == 0)
        {
            indices[cnt++] = 0;
        }

        // extract all ranges that is matched one of |words|
        // if a word that has no matched part is found, returns false.
        std::array<std::pair<int, int>, 32> matchedRanges;
        int numOfMatchedRanges = 0;
        Q_FOREACH(const QString& aWord, words)
        {
            bool matchFound = false;
            for (int i = 0; i < cnt; i++)
            {
                QStringRef part(&keyword, indices[i], keyword.length() - i);
                if (part.startsWith(aWord, Qt::CaseInsensitive))
                {
                    matchFound = true;
                    matchedRanges[numOfMatchedRanges++] = { indices[i], indices[i] + aWord.length() };
                }
            }
            if (!matchFound)
            {
                return{ false, {} };
            }
        }
        matchedRanges[numOfMatchedRanges++] = { 10000, 10000 };

        // emphasize matched substrings with 'b' tag.
        std::sort(matchedRanges.begin(), matchedRanges.begin() + numOfMatchedRanges - 1);
        QString s;
        int pastEnd = 0;
        for (auto it = matchedRanges.cbegin(); it != matchedRanges.cbegin() + numOfMatchedRanges - 1; it++)
        {
            int farest = it->second;
            auto it2 = it + 1;
            while (it2->first <= it->second)
            {
                farest = std::max(farest, it2->second);
                it2++;
            }
            it2--;
            it = it2;

            s += QStringRef(&keyword, pastEnd, it->first - pastEnd);
            s.append("<b>");
            s += QStringRef(&keyword, it->first, farest - it->first);
            s.append("</b>");
            pastEnd = farest;
        }
        s += QStringRef(&keyword, pastEnd, keyword.length());
        return{ true, s };
    }
}