/* ---------------------------------------------
Program 1: DNA Profiling
Course: CS 211, Fall 2022. 
System: MacOS using replit
Author: Yousef Habeeb
UIN: 652622525
---------------------------------------------
*/

#pragma once

#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

string cleanWords(string);
void history(vector <string>,vector <string>);


/* Function is responsible for cleaning up user input of any punctuations on either side of the string. It is implemented by using range loops to increment or decrement position values. Then it uses for loops to set those positions equal to NULL. Finally, one more loop is used to push back any part of the string that isnt NULL and return it.
*/
/*
The function was tested with the following code in main:
 string sentence;
 cin>>sentence;           (to test one word)
 getline(cin, sentence);  (to test full line)
 cout<<cleanToken(sentence);
*/
string cleanToken(string s) {
  bool checker = false;
  string str;
  int positions1 = 0, positions2 = s.size()-1;
  //checking for atleast one letter
  for(char i : s){
    if(isalpha(i)){
      checker = true;
    }
  }
  if(checker){
    // checking for punctuations before the word begins
    for(char i : s){
      if(ispunct(i) == false){
        break;
      }
      positions1++;
    }
    //setting punctuations before the word equal to NULL
    for(int i = 0; i < positions1; i++){
      s.at(i) = '\0';
    }
    // checking for punctuations after the word begins
    for(int i = s.size()-1; i < s.size(); i--){
      if(ispunct(s.at(i)) == false){
         break;
      }
      positions2--;
    }
    //setting punctuations after the word equal to NULL
    for(int i = s.size()-1; i > positions2; i--){
      s.at(i) = '\0';
    }
    //using another string to take in non-NULL characters 
    for(char i : s){
      if(i != '\0'){
        str.push_back(i);
      }
    }
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  }
  else{
    return "";
  }
}




/* Function is used for cleaning entire lines of input and storing it in a set. It is done so by using stringstream to read the text as an entire line and calling the above funtion to clean the token before inserting it into the set and returning the set.
*/
/*
The function was tested with the following code in main:
 string words;
 set<string> tokens;
 getline(cin,words);
 tokens = gatherTokens(words);
 for (auto it = tokens.begin(); it != tokens.end(); ++it){
   cout << ' ' << *it;
 }
*/
set <string> gatherTokens(const string& text) {
  set<string> tokens;

  stringstream s(text);
  string temp;
  string word;

  while (getline(s,word,' ')){
    temp = cleanToken(word);
    //only inserts if there are words inside
    if(!temp.empty()){
      tokens.insert(temp);
    }
  }
  return tokens;
}





/* Function is used to read through a file and store relative urls inside the set(inside the map). This is done so by using 2 getlines (1 for the url/ 1 for the text) and incrementing the number of times a url is counted to determine its amount. The GatherTokens function is called in order to clean the string before it can be insterted. This function will return the number of urls.
*/
/*
The function was tested with the following code in main:
 string filename;
 map< string, set<string>> index;
 cin>> filename;
 int count = buildIndex(filename,index);
 cout<<count;
*/
int buildIndex(const string& filename, map<string, set<string>> &index) {

  fstream myfile;
  string url, word;
  set<string> temp, urlCatcher;
  int urlCount = 0;

  //checking to see if the file opens
  myfile.open(filename);
  if (!myfile.is_open()) {
    cout << "Error: unable to open '" << filename<<"'"<<endl;
    exit(0);
  }

  while((!myfile.eof()) && myfile.good()){
    //gets the url 
    getline(myfile, url);
    urlCount++;
    //used to stop if the file isnt streamable
    if(!myfile.good()){
      break;
    }
    // gets the text under the url
    getline(myfile, word);
    //getting the words
    temp = gatherTokens(word);

    for (auto &itr : temp){
      // if the word is not found in the set then we insert it 
      if(index.find(itr) == index.end()){
        urlCatcher.insert(url);
        index[itr] = urlCatcher;
        urlCatcher.clear();
      }
      else{
        index[itr].insert(url);
      }
    }
  }
  temp.clear();
  return urlCount-1;
}




/* Function is used to allow the search using multiple words at once without having to connect them as a single string. It also allows different combinations of any number of words. This is done by using set_union, set_intersection, and set_difference to allow the program to search for both letters, 1 and not the other, or to search for urls without any of the words.
*/
/*
The function was tested with the following code in main:
  map<string, set<string>> index;
  set<string> urls;
  string test,words;
  cin>>test;

  int count = buildIndex(test,index);
  cin.ignore();
  getline(cin, words);
  urls = findQueryMatches(index, words);

  for (auto it = urls.begin(); it != urls.end(); ++it){
    cout << ' ' << *it;
  }
  cout<<endl<<"there are "<<urls.size()<<" url's";
*/
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
  set<string> allUrls, temp;
  stringstream s(sentence);
  string str;

  while(s >> str){
    
    transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);
    temp.clear();
    //checks to see if theres a character instead of a + or -
    if(isalpha(str[0])){
      set_union(begin(allUrls),
      end(allUrls),
      begin(index[cleanToken(str)]),
      end(index[cleanToken(str)]),
      inserter(temp, end(temp)));
      allUrls = temp;
    }
    // allows user to find websites containing both words 
    else if (str[0] == '+'){
      set_intersection(begin(allUrls),
      end(allUrls),
      begin(index[cleanToken(str)]),
      end(index[cleanToken(str)]),
      inserter(temp, end(temp)));
      allUrls = temp;
    }
    //allows user to find websites containing one, but not the other word
    else if (str[0] == '-'){
      set_difference(begin(allUrls),
      end(allUrls),
      begin(index[cleanToken(str)]),
      end(index[cleanToken(str)]),
      inserter(temp, end(temp)));
      allUrls = temp;
    }
  }
  return allUrls;
}




/* Function is used to combine all other functions and display the webpages that contain the given words. This is done by implementing a fixed version of the test code for findQueryMatches function. In order to keep the query search going, a while loop is used to continue inputting words. BuildIndex gives us the number of urls while findQueryMatches gives us the set of urls which are both used to display them.
*/
/* The test code was implemented in this function and not in main as this function will need to be called in order to run the program.
*/
void searchEngine(const string& filename) {
  //used for user input
  string words;
  map<string, set<string>> index;
  set<string> urls;
  // vetcors are used only for the cretive assesment
  vector<string> allHistory,releventHistory;
  //recieves the number of urls
  int count = buildIndex(filename,index);

  cout<<"Stand by while building index..."<<endl;
  cout<<"Indexed "<< count <<" pages containing "<< index.size() <<" unique terms"<<endl;
  cout<<"\n";
  cout<<"Enter query sentence (press enter to quit): ";

  getline(cin, words);
  //stores searches for creative assessment 
  allHistory.push_back(words);
  releventHistory.push_back(words);

  while(!words.empty()){
    //erases these symbols before being read into the findQueryMatches
    if(words[0] == '-' || words[0] == '+'){
      words.erase(0,1);
    }
    urls = findQueryMatches(index, words);
    cout<<"Found "<< urls.size() <<" matching pages"<<endl;

    //deletes the last search if it does not match (for the creative assessment)
    if(urls.empty()){
      releventHistory.erase(releventHistory.end());
    }
    // prints the urls
    for (const auto & url : urls){
      cout <<url<<endl;
    }

    cout<<endl<<"Enter query sentence (press enter to quit): ";
    getline(cin, words);
    //stores searches for creative assessment 
    allHistory.push_back(words);
    releventHistory.push_back(words);
  }

  cout<<"Thank you for searching!"<<endl<<"Would you like to view recent searches? ";

  string select;
  cin>>select;
  if(select == "yes"){
    history(allHistory,releventHistory);
  }
  else{
    exit(0);
  }
}




/*
These functions are the creative assessment. The cleanWords function is similar to cleanTokens as it removes whitespaces before the words begin. This function is used to make a organized list of the recent searches
*/
/*
There is no need to test this code as it is the same code that was tested in cleanTokens and can simply be applied to the History function.
*/

string cleanWords(string search){
  bool checker = false;
  string str;
  int positions1 = 0;
  //checking for alteast a single character 
  for(char i : search){
    if(isalpha(i)){
      checker = true;
    }
  }
  if(checker){
    // searches for whitespaces before the word begins
    for(char i : search){
      if(i != ' '){
        break;
      }
    positions1++;
    }
    //sets those whitespaces equal to NULL
    for(int i = 0; i < positions1; i++){
      search.at(i) = '\0';
    }
    //using another string to take in non-NULL characters 
    for(char i : search){
      if(i != '\0'){
        str.push_back(i);
      }
    }
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  }
  else{
    return "";
  }
}



/*
These functions are the creative assessment. as we are building a search engine, I added a recently searched history. The history function can print everything the user has searched, only the searches that found matching urls, or can simply exit the program. allHistory contains every word the user searches while releventHistory contains words that found matching urls.
*/
/*
To test this code, simply search for any words using the search engine. After the user is done searching, there will be prompt to either view the history, or simply end the program.
To view all searches, select "1".
to view searches that have matching urls, select "2".

*/
void history(vector<string> allHistory, vector<string> releventHistory){
  int choice;
  string extra = allHistory[0];
  bool checker = false;

  //checks to see if theres alteast one character
  for(auto & i: extra){
    if(isalpha(i)){
      checker = true;
    }
  }
  cout<<"Select 1 for all searches \n"<<"Or select 2 to view searches with matching urls: ";

  cin.ignore();
  cin>>choice;
  cout<<"\n";
  // views all recent searches if there is any 
  if(choice == 1){
    cout<<"Recent searches:\n";
    if(!checker){
      cout<<"No history";
      exit(0);
    }
    else{
      for(auto & i: allHistory){
        i = cleanWords(i);
        cout<<i<<endl;
      }
      exit(0);
    }
  }
  // views all relevent searches if there is any 
  else if(choice == 2){
    cout<<"Recent searches:"<<endl;
    if(!checker){
      cout<<"No relevent history";
      exit(0);
    }
    else{
      for(auto & i : releventHistory){
        i = cleanWords(i);
        cout<<i<<endl;
      }
      exit(0);
    }
  }
}