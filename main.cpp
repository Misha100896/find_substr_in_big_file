#include <iostream>
#include <fstream>
#include <random>

using namespace std;

long long getLengthOfFile(ifstream &f) {
    f.seekg(0,std::ios_base::end);
    std::ios_base::streampos end_pos = f.tellg();

    return static_cast<long long>(end_pos);
}

int getNumberLength(long long n) {
    int ans = 0;
    while (n > 0) {
        ++ans;
        n /= 10;
    }

    return ans;
}

string getNumberFixLength(long long n, int length) {
    string s;
    while (s.length() < length) {
        s += '0' + n % 10;
        n /= 10;
    }

    reverse(s.begin(), s.end());
    return s;
}

long long stringToLongLong(string s) {
    long long ans = 0;

    for (int i = 0; i < s.length(); ++i) {
        ans = ans * 10 + (int)(s[i] - '0');
    }

    return ans;
}

long long getGroupByPos(long long pos, int numLength) {
    ifstream f("suffix_group.txt", std::ifstream::binary);
    f >> std::noskipws;
    f.seekg(pos * (numLength + 1) * sizeof(char), ios::beg);
    string s;

    for (int i = 0; i < numLength; ++i) {
        char c;
        f >> c;
        s += c;
    }

    return stringToLongLong(s);
}

long long getIndexSuffix(long long pos, int numLength) {
    ifstream f("sort_index.txt", std::ifstream::binary);
    f >> std::noskipws;
    f.seekg(pos * (numLength + 1) * sizeof(char), ios::beg);
    string s;

    for (int i = 0; i < numLength; ++i) {
        char c;
        f >> c;
        s += c;
    }

    return stringToLongLong(s);
}

void fillFilesForSortIndexes(ifstream &file, long long fileLength, int numLength) {
    ofstream out1("sort_index.txt");
    ofstream out2("suffix_group.txt");
    file.seekg(0, ios::beg);

    for (long long i = 0; i < fileLength; ++i) {
        char c;
        file >> c;

        if (i != 0) {
            out1 << ',';
            out2 << ',';
        }

        out1 << getNumberFixLength(i, numLength);
        out2 << getNumberFixLength((int)c - '0', numLength);
    }

    out1.close();
    out2.close();
}

void fillTempFileForGroups(int numberLength, long long fileLength) {
    ofstream out("temp_info_groups.txt");

    for (long long i = 0; i < fileLength; ++i) {
        if (i > 0) {
            out << ',';
        }

        out << getNumberFixLength(i, numberLength);
    }
}

bool comparator(long long pos1, long long pos2, long long p, int numLength, long long lengthFile) {
    long long group1Half1 = getGroupByPos(pos1, numLength);
    long long group2Half1 = getGroupByPos(pos2, numLength);

    if (pos1 == pos2) {
        return false;
    }

    if (pos1 == 0 && pos2 == 1) {
        cout << group1Half1 << ' ' << group2Half1 << '\n';
    }

    if (p == 1) {
        return group1Half1 < group2Half1;
    }

    if (group1Half1 < group2Half1) {
        return true;
    }

    if (group2Half1 < group1Half1) {
        return false;
    }

    if (pos1 + p / 2 >= lengthFile) {
        return group1Half1 <= group2Half1;
    }

    if (pos2 + p / 2 >= lengthFile) {
        return group1Half1 < group2Half1;
    }

    long long group1Half2 = getGroupByPos(pos1 + p / 2, numLength);
    long long group2Half2 = getGroupByPos(pos2 + p / 2, numLength);

    return group1Half2 < group2Half2;
}

void setSuffixIndex(long long pos, long long index, long long numLength) {
    fstream file("sort_index.txt", std::fstream::in | std::fstream::out);
    file.seekg(pos * (numLength + 1) * sizeof(char), ios::beg);
    file << getNumberFixLength(index, numLength);
    file.close();
}

void setNewGroup(long long pos, long long index, long long numLength) {
    fstream file("temp_info_groups.txt", std::fstream::in | std::fstream::out);
    file.seekg(pos * (numLength + 1) * sizeof(char), ios::beg);
    file << getNumberFixLength(index, numLength);
    file.close();
}

// Меняет местами два индекса в файле sort_index.txt
void swapperSuffix(long long pos1, long long pos2, int numLength) {
    long long temp = getIndexSuffix(pos1, numLength);
    setSuffixIndex(pos1, getIndexSuffix(pos2, numLength), numLength);
    setSuffixIndex(pos2, temp, numLength);
}

// Сортировка суффиксов
void sortSuffixesInFile(long long l, long long r, long long p, int numLength, long long lengthFile) {
    long long i = l, j = r;
    mt19937 mt_rand(time(0));
    long long md = mt_rand() % (r - l + 1) + l;
    long long pivot = getIndexSuffix(md, numLength);

    while (i <= j) {
        while (comparator(getIndexSuffix(i, numLength), pivot, p, numLength, lengthFile)) {
            ++i;
        }

        while (comparator(pivot, getIndexSuffix(j, numLength), p, numLength, lengthFile)) {
            --j;
        }

        if (i <= j) {
            swapperSuffix(i, j, numLength);
            ++i;
            --j;
        }
    }

    if (j > l) {
        sortSuffixesInFile(l, j, p, numLength, lengthFile);
    }

    if (i < r) {
        sortSuffixesInFile(i, r, p, numLength, lengthFile);
    }
}

int main() {
    ifstream file("../a.txt", std::ifstream::binary);
    file >> std::noskipws;

    long long fileLength = getLengthOfFile(file);

    // Пишем в формате 0000128 чтобы легко менять индексы в файле
    int numberLength = max(getNumberLength(fileLength), 3);
    fillFilesForSortIndexes(file, fileLength, numberLength);

    // Идем по степеням двойки
    for (long long p = 1; p < fileLength; p *= 2) {
        cout << p << endl;

        // Сортируем суффиксы равные степени двойки
        sortSuffixesInFile(0, fileLength - 1, p, numberLength, fileLength);
        fillTempFileForGroups(numberLength, fileLength);

        // Пересчитываем новые полученные группы
        long long numGroup = 0;
        for (long long i = 0; i < fileLength; ++i) {
            long long currentIndex = getIndexSuffix(i, numberLength);

            if (i > 0) {
                long long lastIndex = getIndexSuffix(i - 1, numberLength);
                if (comparator(lastIndex, currentIndex, p, numberLength, fileLength)) {
                    ++numGroup;
                }
            }

            setNewGroup(currentIndex, numGroup, numberLength);
        }

        remove("suffix_group.txt");
        rename("temp_info_groups.txt", "suffix_group.txt");
    }

    cout << "Write queries:\n";

    string query;
    getline(cin, query);

    while (query != "exit") {
        if (query.length() > fileLength) {
            cout << "NO\n";
        }

        long long l = 0, r = fileLength - 1;
        long long ans = -1;

        // Бинарный поиск для нахождения подстроки
        while (l <= r) {
            long long md = (l + r) / 2;
            long long indSuff = getIndexSuffix(md, numberLength);

            file.clear();
            file.seekg(indSuff * sizeof(char), ios::beg);

            bool good = true;
            for (int i = 0; i < query.length(); ++i) {
                if (indSuff + i >= fileLength) {
                    l = md + 1;
                    break;
                }

                char c;
                file >> c;

                if (c != query[i]) {
                    good = false;
                    if (c < query[i]) {
                        l = md + 1;
                    } else {
                        r = md - 1;
                    }

                    break;
                }
            }

            if (good) {
                ans = indSuff;
                break;
            }
        }

        if (ans == -1) {
            cout << "NO\n";
        } else {
            cout << "YES: " << ans << '\n';
        }

        getline(cin, query);
    }

    return 0;
}