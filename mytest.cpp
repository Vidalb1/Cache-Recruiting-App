// CMSC 341 - Fall 25 - Project 4
//UMBC - CSEE - CMSC 341 - Fall 2025 - Proj4
/*****************************************
** File: cache.cpp
** Project: CMSC 341 - Project 4 - A Recruiting App
** Author: Vidal Bickersteth
** Date: 12/5/25
** E-mail: vidalb1@umbc.edu
**
** This program is the mytest.cpp file that tests cache.cpp.
**
***********************************************/
#include "cache.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;
const int MINSEARCH = 0;
const int MAXSEARCH = 7;
// the following array defines sample search strings for testing
string searchStr[MAXSEARCH+1]={"c++","python","java","scheme","prolog","c#","c","js"};
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(){}
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = mt19937(10);// 10 is the fixed seed value
            m_unidist = uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = mt19937(10);// 10 is the fixed seed value
            m_uniReal = uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = mt19937(seedNum);
    }
    void init(int min, int max){
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = mt19937(10);// 10 is the fixed seed value
        m_unidist = uniform_int_distribution<>(min,max);
    }
    void getShuffle(vector<int> & array){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    int getMin(){return m_min;}
    int getMax(){return m_max;}
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    random_device m_device;
    mt19937 m_generator;
    normal_distribution<> m_normdist;//normal distribution
    uniform_int_distribution<> m_unidist;//integer uniform distribution
    uniform_real_distribution<double> m_uniReal;//real uniform distribution
};

class Tester{
    public:
        bool testNormalInsertion();
        bool testCorrectIndex(Person person, Cache& cache);
        bool testCollision(Person person, Cache& cache);
        bool testCollisionInsertion();
        bool testCorrectCollisionIndex(Person person, Cache& cache);
        bool testErrorGetPerson();
        bool testNonCollisionGetPerson();
        bool testCollisionGetPerson();
        bool testNonCollisionRemove();
        bool testCollisionRemove();
        bool testRehashingInsert();
        bool testRehashingInsertCompletion();
        bool testRehashingRemove();
        bool testRehashingRemoveCompletion();
};


unsigned int hashCode(const string str);
//Name: testNormalInsertion
//Precondition: person is valid
//Postcondition: Tests normal insertion
bool Tester::testNormalInsertion(){
    //Tests normal noncollision insertion
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    int size = 0;
    int p = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            if(!testCorrectIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    return true;
}

//Name: testCollsionInsertion
//Precondition: person is valid
//Postcondition: Tests normal insertion collision
bool Tester::testCollisionInsertion(){
//Tests normal collision insertion
    Random RndID(MINID,MAXID);
    Random RndStr(1,10, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    int size = 0;
    int p = 0;
    while(p < 50) {
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            if(!testCorrectCollisionIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    return true;
}
//Name: testCorrectIndex
//Precondition: person and cache is valid
//Postcondition: Tests correct index 
bool Tester::testCorrectIndex(Person person, Cache& cache){
    //Tests index
    unsigned int index = cache.m_hash(person.getKey()) % cache.m_currentCap;
    if(*cache.m_currentTable[index] == person){
        return true;
    }
    return false;
}

//Name: testCorrectCollisionIndex
//Precondition: person and cache is valid
//Postcondition: Tests normal insertion collision index
bool Tester::testCorrectCollisionIndex(Person person, Cache& cache){
    //Tests collision insertion 
   Person* people = cache.probingSearch(person.getKey(), person.getID(), cache.m_currentTable);
   if(people == nullptr){
        return false;
   }
    if(*people == person){
        return true;
    }
    return false;
}

//Name: testCollision
//Precondition: person is valid
//Postcondition: Tests collision
bool Tester::testCollision(Person person, Cache& cache){
    //Checks for collision
    unsigned int index = cache.m_hash(person.getKey()) % cache.m_currentCap;
    if(cache.m_currentTable[index] != nullptr){
        return true;
    }

    return false;

}

//Name: testErrorGetPerson
//Precondition: person is not valid
//Postcondition: Tests getPerson error case
bool Tester::testErrorGetPerson(){
    //Tests getPerson
    vector<Person> dataList;
    Random RndID(MINID,MAXID);
    Random RndStr(MINSEARCH,MAXSEARCH);
    Cache cache(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;
    Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
    dataList.push_back(dataObj);
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
    }
    return true;
}

//Name: testNonCollisionGetPerson
//Precondition: person is valid
//Postcondition: Tests getPerson non collision
bool Tester::testNonCollisionGetPerson(){
    //Test getPerson non collison
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    vector<Person> dataList;
    bool result = true;
    int size = 0;
    int p = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            dataList.push_back(dataObj);
            if(!testCorrectIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
    }
    return true;
}
//Name: testCollisionGetPerson
//Precondition: person is valid
//Postcondition: Tests getPerson collision case
bool Tester::testCollisionGetPerson(){
    //Tests getPerson collision case
    Random RndID(MINID,MAXID);
    Random RndStr(1,5, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    vector<Person> dataList;
    bool result = true;
    int size = 0;
    int p = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            dataList.push_back(dataObj);
            if(!testCorrectCollisionIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
    }
    return true;
}

//Name: testNonCollisionRemove()
//Precondition: person is valid
//Postcondition: Tests remove non collison
bool Tester::testNonCollisionRemove(){
    //Tests remove non collison
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    vector<Person> dataList;
    int size = 0;
    int p = 0;
    int l = 0;
    bool result = true;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            if(!testCorrectIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
        if(l < 4){
            if(!cache.remove(*it)){
                return false;
            }
        }
        l++;
    }
    return true;
}

//Name: testCollisionRemove()
//Precondition: person is valid
//Postcondition: Tests remove collison
bool Tester::testCollisionRemove(){
    //Tests remove collision
    Random RndID(MINID,MAXID);
    Random RndStr(1,5, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    vector<Person> dataList;
    bool result = true;
    int size = 0;
    int p = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(testCollision(dataObj, cache)){
            if(!cache.insert(dataObj)){
                return false;
            }
            dataList.push_back(dataObj);
            if(!testCorrectCollisionIndex(dataObj, cache)){
                return false;
            }
        }else{
            if(!cache.insert(dataObj)){
                return false;
            }
        }
        size = cache.m_currentSize;
        if(size != p+1){
            return false;
        }
        p++;
    }
    int l = 0;
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
        if(l < 4){
            if(!cache.remove(*it)){
                return false;
            }
        }
        l++;
    }
    return true;
}

//Name: testRehashingInsert()
//Precondition: table exists 
//Postcondition: Tests incremental rehashing
bool Tester::testRehashingInsert(){
    //Tests rehash for insert
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    cache.changeProbPolicy(QUADRATIC);
    int p = 0;
    while(p < 51){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!cache.insert(dataObj)){
            return false;
        }
        p++;
    }
    if(cache.m_transferIndex == 0 || !cache.m_oldTable){
        return false;
    }
    return true;
}

//Name: testRehashingInsertCompletion()
//Precondition: transfer must exist
//Postcondition: Tests rehash completion
bool Tester::testRehashingInsertCompletion(){
    //Tests full rehash for insert
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    cache.changeProbPolicy(QUADRATIC);
    int p = 0;
    while(p < 54){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!cache.insert(dataObj)){
            return false;
        }
        p++;
    }
    if(cache.m_transferIndex != 0 || cache.m_oldTable){
        return false;
    }
    return true;
}

//Name: testRehashingRemove()
//Precondition: table exists 
//Postcondition: Tests incremental rehashing
bool Tester::testRehashingRemove(){
    //Tests rehash for remove
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    cache.changeProbPolicy(QUADRATIC);
    vector<Person> dataList;
    bool result = true;
    int p = 0;
    int l = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!cache.insert(dataObj)){
            return false;
        }
        dataList.push_back(dataObj);
        p++;
    }
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
        if(l < 40){
            if(!cache.remove(*it)){
                return false;
            }
        }
        l++;
    }
    if(cache.m_transferIndex == 0 || !cache.m_oldTable){
        return false;
    }
    return true;
}

//Name: testRehashingRemoveCompletion()
//Precondition: person is valid
//Postcondition: Tests rehash completion
bool Tester::testRehashingRemoveCompletion(){
    //Test full rehash for remove
    Random RndID(MINID,MAXID);
    Random RndStr(1, 100, UNIFORMINT);
    Cache cache(MINPRIME, hashCode, LINEAR);
    cache.changeProbPolicy(QUADRATIC);
    vector<Person> dataList;
    bool result = true;
    int p = 0;
    int l = 0;
    while(p < 50){
        // generating random data
        Person dataObj = Person(RndStr.getRandString(RndStr.getRandNum()), RndID.getRandNum(), true);
        if(!cache.insert(dataObj)){
            return false;
        }
        dataList.push_back(dataObj);
        p++;
    }
    for(vector<Person>::iterator it = dataList.begin(); it != dataList.end(); it++){
        Person anObj = cache.getPerson((*it).getKey(), (*it).getID());
        bool foundIt = (*it == anObj);
        result = result && foundIt;
        if(!foundIt){
            return false;
        }
        if(l < 43){
            if(!cache.remove(*it)){
                return false;
            }
        }
        l++;
    }
    if(cache.m_transferIndex != 0 || cache.m_oldTable){
        return false;
    }
    return true;
}
int main(){
    Tester tester;
    //Testing normal insert
    if(tester.testNormalInsertion()){
        cout << "TEST CASE PASSED insert(): Nodes are inserted at proper index" << endl;
    }else{
        cout << "TEST CASE FAILED insert(): Nodes are not inserted at proper index" << endl;
    }
    //Testing normal insert collision
    if(tester.testCollisionInsertion()){
        cout << "TEST CASE PASSED insert(): Nodes are inserted at proper index and collsions are handled" << endl;
    }else{
        cout << "TEST CASE FAILED insert(): Nodes are not inserted at proper index nor collisions handled" << endl;
    }
    //Testing error getPerson
    if(!tester.testErrorGetPerson()){
        cout << "TEST CASE PASSED getPerson(): Person does not exist in the database" << endl;
    }else{
        cout << "TEST CASE FAILED getPerson(): Person does exist in the database" << endl;
    }
    //Testing non collision getPerson
    if(tester.testNonCollisionGetPerson()){
        cout << "TEST CASE PASSED getPerson(): Person does exist in the database with non collision" << endl;
    }else{
        cout << "TEST CASE FAILED getPerson(): Person does not exist in the database with non collision" << endl;
    }
    //Testing collision getPerson
    if(tester.testCollisionGetPerson()){
        cout << "TEST CASE PASSED getPerson(): Person does exist in the database with collision handled" << endl;
    }else{
        cout << "TEST CASE FAILED getPerson(): Person does not exist in the database with collision handled" << endl;
    }
    //Testing non collision remove
    if(tester.testNonCollisionRemove()){
        cout << "TEST CASE PASSED remove(): Persons got removed in the database with no collision" << endl;
    }else{
        cout << "TEST CASE FAILED remove(): Persons not removed in the database with no collision" << endl;
    }
    //Testing collision remove
    if(tester.testCollisionRemove()){
        cout << "TEST CASE PASSED remove(): Persons got removed in the database with collision" << endl;
    }else{
        cout << "TEST CASE FAILED remove(): Persons not removed in the database with collision" << endl;
    }
    //Testing Rehash
    if(tester.testRehashingInsert()){
        cout << "TEST CASE PASSED insert(): Nodes are inserted at proper index and rehashing after trigger" << endl;
    }else{
        cout << "TEST CASE FAILED insert(): Nodes are not inserted at proper index nor rehashing after trigger" << endl;
    }

    //Testing Rehash Completion
    if(tester.testRehashingInsertCompletion()){
        cout << "TEST CASE PASSED insert(): Nodes are inserted and complete rehash" << endl;
    }else{
        cout << "TEST CASE FAILED insert(): Nodes are not inserted nor complete rehash" << endl;
    }

    //Testing Rehash Remove
    if(tester.testRehashingRemove()){
        cout << "TEST CASE PASSED remove(): Nodes are removed and rehashing after trigger" << endl;
    }else{
        cout << "TEST CASE FAILED remove(): Nodes are not remove nor rehashing after trigger" << endl;
    }

    //Testing Rehash Remove Completion
    if(tester.testRehashingRemoveCompletion()){
        cout << "TEST CASE PASSED remove(): Nodes are inserted at proper index and complete rehash" << endl;
    }else{
        cout << "TEST CASE FAILED remove(): Nodes are not inserted at proper index nor complete rehash" << endl;
    }
    return 0;
}

unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for ( int i = 0 ; i < (int)(str.length()); i++)
       val = val * thirtyThree + str[i] ;
    return val ;
 }