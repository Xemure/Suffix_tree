// STComplete.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>  //Нужно для работы с файлами.
#include <list>
#include "SufTree.h"
#include <chrono>

/// <summary>
/// Строит случайную строку заполненную латинскими символами
/// </summary>
/// <param name="ab_size">размер алфавита</param>
/// <param name="len">длина строки</param>
/// <returns>случаную строку из латинских символов</returns>
string get_rnd_str(int ab_size, int len)
{
    string s(len,0);
    for (int i = 0; i < len; i++)
    {
        int rch = rand() % ab_size + 'a';
        s [i]= char(rch);
    }
    return s;
}


#define FROM_FILE
int main()
{
#ifdef FROM_FILE
    cout << "Enter a file name (empty for 'rfc 1951.txt'): ";

    string fname;
    char fName[128]{0};
    cin.getline(fName, sizeof(fName));
    if (strlen(fName) == 0)
        strcpy_s(fName, sizeof(fName), "rfc 1951.txt");
    cout << fName << endl;

    ifstream ifs(fName, ios_base::in | ios_base::binary);
    char input[128];

    streampos begin, end;
    begin = ifs.tellg();
    ifs.seekg(0, ios::end);
    end = ifs.tellg();
    cout << "size is: " << (end - begin) << " bytes.\n";
    ifs.seekg(0, ios::beg);

    char* inp = new char[(end - begin) * 2];
    SufTree st(inp);
    auto start = chrono::steady_clock::now();

    int maxRemaind = 0, iMax = 0;
    char c = ifs.get();
    for (int i = 0; ifs.good(); i++)
    {
        inp[i] = c;
        st.extend(i);
        c = ifs.get();
        iMax = i;
    }
    for (int i = iMax + 1; i < iMax * 2 + 2; i++)
    {
        inp[i] = inp[iMax - (i - iMax - 1)];
        st.extend(i);
    }
    st.finalize();
    auto stop = chrono::steady_clock::now();
    ifs.close();
    cout << "Time difference = " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << "[ms]" << endl;
    st.FillLeaves();
    st.FillPath();
    st.FillNodes();
    list<int> palindromes;
    int len_for_get_pal = st.MaxPalindromeLen();
    cout << "Max Palindrome: " << len_for_get_pal << endl;
    cout << "Palindromes of max length: " << endl;
    st.GetPalindromes(palindromes, len_for_get_pal);
    for (auto& p : palindromes)
        cout << string(inp + p, len_for_get_pal) << endl;
    delete[] inp;
#else
    //string s = get_rnd_str(2, 7);
    string s = "cbbeaaa";
    cout << s << endl;
    s += string(s.rbegin(), s.rend());
    cout << s << endl;
    auto start = chrono::steady_clock::now();

    SufTree st(s.c_str());
    for (int i = 0; i < s.length(); i++)
        st.extend(i);
    // и завершаем построение, делая все суффиксы явными!
    st.finalize();
    
    auto stop = chrono::steady_clock::now();

    st.FillLeaves();    // Можно и без него, но не будут заполнены индексы суффиксов.
    st.FillPath();
    st.FillNodes();

    st.ShowTree();
    cout << endl << endl;
    cout << st.MaxPalindromeLen() << endl;

    // Все палиндромы заданной длины
    list<int> palindromes;
    int len_for_get_pal = st.MaxPalindromeLen();
    st.GetPalindromes(palindromes, len_for_get_pal);
    for (auto& p : palindromes)
        cout << string(s.c_str() + p, len_for_get_pal) << endl;
    cout << "Time difference = " << chrono::duration_cast<chrono::microseconds>(stop - start).count() << "[mks]" << endl;
#endif

}
