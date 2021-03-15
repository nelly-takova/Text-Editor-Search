#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <utility>
using namespace std;

// Structure to store information of a suffix
struct Suffix
{
    int index = 0; // To store original index
    pair<int, int> suffRank; // To store ranks and next rank pair
};

// A comparison function used by sort() for suffixes
bool isSmaller(Suffix& a, Suffix& b)
{
    if (a.suffRank.first == b.suffRank.first)
    {
        return (a.suffRank.second < b.suffRank.second);
    }
    else
    {
        return (a.suffRank.first < b.suffRank.first);
    }
}

vector<int> makeSuffixArray(string text)
{
    string::size_type N = text.size();
    // A structure to store suffixes and their indexes
    Suffix* suffixes = new Suffix[N];

    // Store suffixes and their indexes in an array of structures. 
    for (string::size_type i = 0; i < N; i++)
    {
        suffixes[i].index = i;
        suffixes[i].suffRank.first = text[i] - 'a';
        suffixes[i].suffRank.second = ((i + 1) < N) ? (text[i + 1] - 'a') : -1;
    }
        
    sort(suffixes, suffixes + N, isSmaller);

    // At this point, all suffixes are sorted according to first 2 characters. Now we have to sort suffixes according to first
    // 4 characters, then first 8 and so on
    int* indexes = new int[N];  
    // This array is needed to get the index in suffixes[] from original index.  This mapping is needed to get next suffix.

    for (string::size_type k = 4; k < 2 * N; k = k * 2)
    {
        // Assigning rank and index values to first suffix
        int rank = 0;
        int prevRank = suffixes[0].suffRank.first;
        suffixes[0].suffRank.first = rank;
        indexes[suffixes[0].index] = 0;

        // Assigning rank to suffixes
        for (string::size_type i = 1; i < N; i++)
        {
            // If first rank and next ranks are same as that of previous
            // suffix in array, assign the same new rank to this suffix
            if (suffixes[i].suffRank.first == prevRank && suffixes[i].suffRank.second == suffixes[i - 1].suffRank.second)
            {
                prevRank = suffixes[i].suffRank.first;
                suffixes[i].suffRank.first = rank;
            }
            else // Otherwise increment rank and assign
            {
                prevRank = suffixes[i].suffRank.first;
                suffixes[i].suffRank.first = ++rank;
            }
            indexes[suffixes[i].index] = i;
        }

        // Assign next rank to every suffix
        for (string::size_type i = 0; i < N; i++)
        {
            int nextIndex = suffixes[i].index + k / 2;
            suffixes[i].suffRank.second = (nextIndex < (int)N) ? suffixes[indexes[nextIndex]].suffRank.first : -1;
        }

        // Sort the suffixes according to first k characters
        sort(suffixes, suffixes + N, isSmaller);
    }

    // Store indexes of all sorted suffixes in the suffix array
    vector<int> suffixArr;
    for (string::size_type i = 0; i < N; i++)
    {
        suffixArr.push_back(suffixes[i].index);
    }
    // Return the suffix array
    return  suffixArr;
}

//Utility function to the function "find", which finds all indexes that match the phrase 
set<int> findAllIndexes(string phrase, string text, vector<int>& suffArr, int phraseLength, int indexFound)
{           
    set<int> result;
    int isMatching = phrase.compare(0, phraseLength, text, suffArr[indexFound], phraseLength);
    int decreasingIndex = indexFound;
    //A loop to check for the next indexes
    while (isMatching == 0)
    {
        result.insert(suffArr[indexFound]);
        indexFound++;
        if (indexFound > (int)text.size() - 1)
        {
            break;
        }
        isMatching = phrase.compare(0, phraseLength, text, suffArr[indexFound], phraseLength);
    }
    isMatching = phrase.compare(0, phraseLength, text, suffArr[decreasingIndex], phraseLength);
    //A loop to check for the previous indexes
    while (isMatching == 0)
    {
        result.insert(suffArr[decreasingIndex]);
        decreasingIndex--;
        if (decreasingIndex < 0)
        {
            break;
        }
        isMatching = phrase.compare(0, phraseLength, text, suffArr[decreasingIndex], phraseLength);
    }
    return result;
}

// A suffix array based search function  
set<int> find(string phrase, string text, vector<int>& suffArr)
{
    int fullLength = text.size();
    int phraseLength = phrase.size();  
    set<int> result;
    // Do binary search for the phrase in the text using the built suffix array 

    int left = 0, right = fullLength - 1; 
    while (left <= right)
    {
        // See if the phrase is prefix of middle suffix in suffix array 
        int mid = (left + right) / 2;
        int isMatching = phrase.compare(0,phraseLength,text,suffArr[mid],phraseLength);

        // If match found at the middle, insert in the set 
        if (isMatching == 0)
        {
            result = findAllIndexes(phrase, text, suffArr, phraseLength, mid);
            break;
        }
        else if (isMatching < 0)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }    
    return result;
}

int main()
{
    string text;
    cout << "Please insert the text you want to search in: ";
    getline(cin, text);
    if (!cin)
    {
        cout << "Error! Wrong input.";
        return -1;
    }
    vector<int> suffixArr = makeSuffixArray(text);
    int RequestsNumber = 0;
    cout << "\n Please enter the number of phrases you want to search for: ";
    cin >> RequestsNumber;
    if (!cin || RequestsNumber < 1)
    {
        cout << "Error! Wrong input.";
        return -2;
    }
    for (int i = 0; i < RequestsNumber; i++)
    {
        string phrase;
        cout << "\n Insert a word or phrase: ";
        getline(cin, phrase);
        if (!cin || phrase.size() > text.size())
        {
            cout << "Error! Wrong input.";
            return -3;
        }
        if (phrase.size() == 0)
        {
            getline(cin, phrase);
        }
        set<int> indexes = find(phrase, text, suffixArr);
        if (indexes.size() == 0)
        {
            cout << "\n Sorry! This phrase doesn't appear in the text.";        
        }
        else
        {
            cout << "\n The phrase appears in text " << indexes.size() << " time/s at the position/s: ";
            for (int i : indexes)
            {
                cout << i << " ";
            }
        }
    }
    return 0;
}