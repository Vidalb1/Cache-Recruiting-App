// CMSC 341 - Fall 25 - Project 4
//UMBC - CSEE - CMSC 341 - Fall 2025 - Proj4
/*****************************************
** File: cache.cpp
** Project: CMSC 341 - Project 4 - A Recruiting App
** Author: Vidal Bickersteth
** Date: 12/5/25
** E-mail: vidalb1@umbc.edu
**
** This program is the cache.cpp file that creates a cache for an app.
**
***********************************************/
#include "cache.h"
//Name: Region
//Precondition: Passed values must be valid
//Postcondition: Initializes member variables
Cache::Cache(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    //Checks for valid size
    if(!isPrime(size)){
        m_currentCap = findNextPrime(size);
    }
    if(size < MINPRIME){
        m_currentCap = MINPRIME;
    }else if(size > MAXPRIME){
        m_currentCap = MAXPRIME;
    }else{
        m_currentCap = size;
    }
    //Initializes variables
    m_currProbing = probing;
    m_hash = hash;
    m_currentTable = new Person*[m_currentCap];
    for(int i = 0; i < m_currentCap; i++){
        m_currentTable[i] = nullptr;
    }
    m_currNumDeleted = 0;
    m_currentSize = 0;
    m_newPolicy = DEFPOLCY;
    m_transferIndex = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = DEFPOLCY;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldTable = nullptr;

}

//Name: ~Cache
//Precondition: table must exist
//Postcondition: Deallocates all memory
Cache::~Cache(){
    //Deallocates current table
    if(m_currentTable != nullptr){
        for(int i = 0; i < m_currentCap; i++){
            Person* person = m_currentTable[i];
            if(person){
                delete m_currentTable[i];
                m_currentTable[i] = nullptr;
            }  
        }
        delete[] m_currentTable;
        m_currentTable = nullptr;
    }

    //Deallocates old table if conditions are met
    if(m_oldTable != nullptr){
         for(int i = 0; i < m_oldCap; i++){
            Person* person = m_oldTable[i];
            if(person){
                delete m_oldTable[i];
                m_oldTable[i] = nullptr;
            }  
        }
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }
    //member variables initialized to default values
    m_currentCap = 0;
    m_currProbing = DEFPOLCY;
    m_hash = nullptr;
    m_currNumDeleted = 0;
    m_currentSize = 0;
    m_newPolicy = DEFPOLCY;
    m_transferIndex = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = DEFPOLCY;
    m_oldCap = 0;
    m_oldSize = 0;
}

//Name: changeProbPolicy
//Precondition: policy must be valid
//Postcondition: sets new policy for rehashing purposes
void Cache::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}

//Name: insert
//Precondition: person must exist
//Postcondition: Adds a new person to the hash table and rehashes if conditions are met
bool Cache::insert(Person person){
    //Incrementally rehashes if needed and invalid checks
    if(m_oldTable != nullptr){
        incrementHash();
    }
    Person* people = new Person(person.getKey(), person.getID(), true);
    if(!noInvalidID(person.getID())){
        return false;
    }
    if(m_currentSize*2 == MAXPRIME){
        return false;
    }
    if(!noDup(people)){
        return false;
    }
    //Inserts 
    unsigned int index = m_hash(people->getKey()) % m_currentCap;
    if(m_currentSize < 1){
        m_currentTable[index] = people;
        m_currentSize++;
        if(m_currentTable[index]->getUsed() == false){
            m_currentTable[index]->setUsed(true);
            m_currNumDeleted--;
        }
        return true;
    }
    //Prevents collisions
    if(m_currentTable[index] != nullptr){
        if(m_currProbing == LINEAR){
            if(linearProbing(people, m_currentTable)){
                m_currentSize++;
            }
        }else if(m_currProbing == QUADRATIC){
            if(quadraticProbing(people, m_currentTable)){
                m_currentSize++;
            }
        }else if(m_currProbing == DOUBLEHASH){
            if(doubleHashingProbing(people, m_currentTable)){
                m_currentSize++;
            }
        }
    }else{
        m_currentTable[index] = people;
        m_currentSize++;
        if(!person.getUsed()){
            person.setUsed(true);
        }
    }
    //Rehash if needed
    if(lambda() > .50){
        incrementHash();
    }
    return true;
}

//Name: rehash
//Precondition: old table must exist
//Postcondition: Rehash into a bigger table
void Cache::rehash(){
    if(m_transferIndex == 0){
        m_oldSize = m_currentSize;
        m_oldCap = m_currentCap;
        m_oldTable = m_currentTable;
        m_oldProbing = m_currProbing;
        m_oldNumDeleted = m_currNumDeleted;
        int currentNum = m_currentSize - m_currNumDeleted;
        m_currentCap = findNextPrime(currentNum * 4);
        m_currentTable = new Person*[m_currentCap];
        for(int i = 0; i < m_currentCap; i++){
            m_currentTable[i] = nullptr;
        }
        m_currentSize = 0;
        m_currNumDeleted = 0;
    }
}

//Name: incrementHash
//Precondition: current table must exist
//Postcondition: Incrementally transfer 25% of the nodes to old table and creates a new table then resets
void Cache::incrementHash(){
    //Prepares the transfer 
    rehash();
    int limit = floor(m_transferIndex + ((m_oldCap/ 4)));
    if(limit > m_oldCap){
        limit = m_oldCap;
    }
    int limit2 = (3 * (m_oldCap/ 4));
    //Before the 75% transfer
    for(int i = m_transferIndex; i < limit; i++){
        if(m_newPolicy == QUADRATIC){
            if(m_oldTable[i] != nullptr && m_oldTable[i]->getUsed() == true){
                if(quadraticProbing(m_oldTable[i], m_currentTable)){
                    m_currentSize++;
                    m_oldTable[i] = nullptr;
                }
            }
        }else if(m_newPolicy == LINEAR){
            if(m_oldTable[i] != nullptr && m_oldTable[i]->getUsed() == true){
                if(linearProbing(m_oldTable[i], m_currentTable)){
                    m_currentSize++;
                    m_oldTable[i] = nullptr;
                }
            }
        }else if(m_newPolicy == DOUBLEHASH){
            if(m_oldTable[i] != nullptr && m_oldTable[i]->getUsed() == true){
                if(doubleHashingProbing(m_oldTable[i], m_currentTable)){
                    m_currentSize++;
                    m_oldTable[i] = nullptr;
                }
            }
        }
        
    }
    m_transferIndex = limit;
    if(m_transferIndex >= limit2){
        //Transfers the remaining data to new table
            for(int j = m_transferIndex; j < m_oldCap; j++){
                if(m_newPolicy == QUADRATIC){
                    if(m_oldTable[j] != nullptr && m_oldTable[j]->getUsed() == true){
                        if(quadraticProbing(m_oldTable[j], m_currentTable)){
                            m_currentSize++;
                            m_oldTable[j] = nullptr;
                        }
                    }
                }else if(m_newPolicy == LINEAR){
                if(m_oldTable[j] != nullptr && m_oldTable[j]->getUsed() == true){
                        if(linearProbing(m_oldTable[j], m_currentTable)){
                            m_currentSize++;
                            m_oldTable[j] = nullptr;
                        }
                    }
                }else if(m_newPolicy == DOUBLEHASH){
                    if(m_oldTable[j] != nullptr && m_oldTable[j]->getUsed() == true){
                        if(doubleHashingProbing(m_oldTable[j], m_currentTable)){
                            m_currentSize++;
                            m_oldTable[j] = nullptr;
                        }
                    }
                }
            }  
        m_transferIndex = m_oldCap;  
    }
    //Deletes oldTable once full rehash is complete
    if(m_transferIndex >= m_oldCap){
        if(m_oldTable != nullptr){
         for(int i = 0; i < m_oldCap; i++){
            Person* person = m_oldTable[i];
            if(person){
                delete m_oldTable[i];
                m_oldTable[i] = nullptr;
            }  
        }
        delete[] m_oldTable;
        m_oldTable = nullptr;
        }
        m_transferIndex = 0;
    }
}

//Name: linearProbing
//Precondition: person and table must be valid
//Postcondition: Handles collisions linearly and finds the next valid bucket for insert
bool Cache::linearProbing(Person* person, Person** table){
    //Linear probing
    if(person == nullptr){
        return false;
    }
    unsigned int index = m_hash((person->getKey())) % m_currentCap;
    unsigned int probing = m_hash((person->getKey())) % m_currentCap;
    int i = 0;
    while(i < m_currentCap && m_currentTable[index] != nullptr){
        index = ((probing) + i) % m_currentCap;
        i++;
    }
    if(table[index] == nullptr || table[index]->getUsed() == false){
        table[index] = person;
        return true;
    }
    return false;
}

//Name: quadraticProbing
//Precondition: person and table must be valid
//Postcondition: Handles collisions quadratically and finds the next valid bucket for insert
bool Cache::quadraticProbing(Person* person, Person** table){
    //Quadratic probing
    if(person == nullptr){
        return false;
    }
    unsigned int index = m_hash((person->getKey())) % m_currentCap;
    unsigned int probing = m_hash((person->getKey())) % m_currentCap;
    int i = 0;
    while(i < m_currentCap && m_currentTable[index] != nullptr){
        index = ((probing) + (i * i)) % m_currentCap;
        i++;
    }
    if(table[index] == nullptr || table[index]->getUsed() == false){
        table[index] = person;
        table[index]->setUsed(true);
        return true;
    }
    return false;
}

//Name: doubleHashingProbing
//Precondition: person and table must be valid
//Postcondition: Handles collisions in double hashing and finds the next valid bucket for insert
bool Cache::doubleHashingProbing(Person* person, Person** table){
    //Double hash probing
    if(person == nullptr){
        return false;
    }
    unsigned int index = m_hash((person->getKey())) % m_currentCap;
    unsigned int probing = m_hash((person->getKey())) % m_currentCap;
    int i = 0;
    while(i < m_currentCap && m_currentTable[index] != nullptr){
        index = ((probing) + (i * (11-(m_hash(person->getKey()) % 11)))) % m_currentCap;
        i++;
    }
    if(table[index] == nullptr || !table[index]->getUsed()){
        table[index] = person;
        table[index]->setUsed(true);
        return true;
    }
    return false;
}

//Name: probingSearch
//Precondition: key, ID and table must be valid
//Postcondition: searches for person in the most time effective fashion using probing techniques
Person* Cache::probingSearch(string key, int ID, Person** table) const{
    //Searches for person in probing iteratively
    unsigned int index = m_hash(key) % m_currentCap;
    unsigned int probing = m_hash(key) % m_currentCap;
    int i = 0;
    if(table == nullptr){
        return nullptr;
    }
    while(i < m_currentCap && table[index] != nullptr){
        if(m_currProbing == LINEAR){
                index = ((probing) + i) % m_currentCap;
                i++;
        }else if(m_currProbing == QUADRATIC){
                index = ((probing) + (i * i)) % m_currentCap;
                i++;
        }else if(m_currProbing == DOUBLEHASH){
                index = ((probing) + (i * (11-(m_hash(key) % 11)))) % m_currentCap;
                i++;
        }
        if(table[index] == nullptr){
            return nullptr;
        }
        if(table[index]->getKey() == key && table[index]->getID() == ID && table[index]->getUsed() == true){
            return table[index];
        }
    }
    return nullptr;
}
//Name: noDup
//Precondition: person must be valid
//Postcondition: Ensures no duplicate persons
bool Cache::noDup(Person* &person){
    //searches for person in the most time effective fashion using probing techniques to ensure no duplicates
    if(person->getKey() == "" || m_currentSize == 0){
        return true;
    }
    unsigned int index = m_hash((person->getKey())) % m_currentCap;
    unsigned int probing = m_hash((person->getKey())) % m_currentCap;
    int i = 0;
    if(m_currentTable[index] == nullptr){
        return true;
    }
   while(i < m_currentCap && m_currentTable[index] != nullptr){
        if(m_currProbing == LINEAR){
                index = ((probing) + i) % m_currentCap;
                i++;
        }else if(m_currProbing == QUADRATIC){
                index = ((probing) + (i * i)) % m_currentCap;
                i++;
        }else if(m_currProbing == DOUBLEHASH){
                index = ((probing) + (i * (11-(m_hash(person->getKey()) % 11)))) % m_currentCap;
                i++;
        }
        if(m_currentTable[index] == person){
            return false;
        }
    }
    //If not found then no dup is found
    if(m_currentTable[index] == nullptr){
        return true;
    }
    return true;
}

//Name: noInvalidID
//Precondition: personID must be valid
//Postcondition: Ensures no invalid ID
bool Cache::noInvalidID(int personID){
    //Must be in range
    if(personID >= MINID && personID <= MAXID){
        return true;
    }
    return false;
}

//Name: remove
//Precondition: person must be valid
//Postcondition: removes person from hash table
bool Cache::remove(Person person){
    //Rehash if needed
    if(m_oldTable != nullptr){
        incrementHash();
    }
    //Checks for invalid scenarios
    if(m_currentSize < 1 && m_oldSize < 1){
        return false;
    }
    //Probing for must effective search and tag the object as deleted if found
    Person* oldPerson = probingSearch(person.getKey(), person.getID(), m_oldTable);
    Person* currPerson = probingSearch(person.getKey(), person.getID(), m_currentTable);
    if(!oldPerson && !currPerson){
        return false;
    }else if(!oldPerson && currPerson){
        currPerson->setUsed(false);
        m_currNumDeleted++;
    }else if(!currPerson && oldPerson){
        oldPerson->setUsed(false);
        m_oldNumDeleted++;
    }
    //Rehash if needed
    if(deletedRatio() > .80){
        incrementHash();
    }
    return true;

}

//Name: people
//Precondition: person must be valid
//Postcondition: returns person object
const Person Cache::people(Person*& person) const{
    Person ppl;
    if(person == nullptr){
        return ppl;
    }
    ppl = *person;
    return ppl;
}

//Name: getPerson
//Precondition: key and ID must be valid
//Postcondition: gets person from database
const Person Cache::getPerson(string key, int ID) const{
    //Probing techniques for most effective searching and if found returns person
    Person person;
    Person* oldPerson = probingSearch(key, ID, m_oldTable);
    Person newOldPerson = people(oldPerson);
    Person* currPerson = probingSearch(key, ID, m_currentTable);
    Person newCurrPerson = people(currPerson);
    if(newOldPerson.getKey().empty() && newCurrPerson.getKey().empty()){
        return person;
    }else if(newOldPerson.getKey().empty() && !newCurrPerson.getKey().empty()){
        return newCurrPerson;
    }else if(newCurrPerson.getKey().empty() && !newOldPerson.getKey().empty()){
        return newOldPerson;
    }
    return person;

}

//Name: updateID
//Precondition: person and ID must be valid
//Postcondition: Updates a person's ID
bool Cache::updateID(Person person, int ID){
    Person* people = probingSearch(person.getKey(), person.getID(), m_currentTable);
    Person* people2 = probingSearch(person.getKey(), person.getID(), m_oldTable);
    if(!noInvalidID(ID)){
        return false;
    }
    if(!people && !people2){
        return false;
    }else if(!people && people2){
        people2->setID(ID);
        return true;
    }else if(people && !people2){
        people->setID(ID);
        return true;
    }
    return false;
}

//Name: lambda
//Precondition: current size and cap must be valid
//Postcondition: Captures the load factor of current table
float Cache::lambda() const {
    int numOfOcc = m_currentSize;
    float loadFactor = (float) numOfOcc / m_currentCap;
    return loadFactor;
}

//Name: deletedRatio
//Precondition: current size and deleted buckets must be valid
//Postcondition: Captures the deletedRatio of current table
float Cache::deletedRatio() const {
    int numOfOcc = m_currentSize;
    float deletedRatio = (float) m_currNumDeleted / numOfOcc;
    return deletedRatio;

}

//Name: dump
//Precondition: current table must exist
//Postcondition: prints the hash table 
void Cache::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

//Name: isPrime
//Precondition: number must be valid
//Postcondition: Checks if passed number is a prime number
bool Cache::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

//Name: findNextPrime
//Precondition: current must be valid
//Postcondition: finds the next prime number
int Cache::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}