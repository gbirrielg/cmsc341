// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    //Setting proper prime number table size.
    if (size < MINPRIME) {m_currentCap = MINPRIME;}
    if (size > MAXPRIME) {m_currentCap = MAXPRIME;}
    if (!isPrime(size)){
        m_currentCap = findNextPrime(size);
    } else {
        m_currentCap = size;
    }

    //Initalizing table and inserting EMPTY cars.
    m_currentTable = new Car[m_currentCap];
    for (int i = 0; i < m_currentCap; i++){
        m_currentTable[i] = EMPTY;
    }
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_currProbing = probing;

    m_hash = hash;
    m_newPolicy = NONE;

    //Initializing old table variables.
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = NONE;
}

CarDB::~CarDB(){
    if (m_oldTable) {delete[] m_oldTable;}
    if (m_currentTable) {delete[] m_currentTable;}
}

void CarDB::changeProbPolicy(prob_t policy){
    //Storing requested policy to be used when rehashing.
    m_newPolicy = policy;
}

bool CarDB::insert(Car car){
    //Using overloaded insert function that handles insertions and collisions.
    return insert(car, 0);
}

bool CarDB::insert(Car car, int i){
    //Ensuring car dealer's id is valid.
    if (car.m_dealer < MINID || car.m_dealer > MAXID) {return false;}

    //Calcuating index to insert car into.
    int index;
    if (m_currProbing == QUADRATIC){ //Quadratic probing
        index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
    }
    else{ //Double hash probing
        index = ((m_hash(car.m_model) % m_currentCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_currentCap;
    }

    //If identical car obj already exists, return false.
    if (m_currentTable[index] == car){
        checkIntegrity();
        transferCars();
        return false;
    }
    //If index after hashing lands on an EMPTY car, insert car.
    else if (m_currentTable[index] == EMPTY){
        m_currentTable[index] = car;
        m_currentSize++;
        checkIntegrity();
        transferCars();
        return true;
    }
    //If index is not EMPTY but is deleted object, insert car.
    else if (!m_currentTable[index].m_used){
        m_currentTable[index] = car;
        m_currNumDeleted--;
        checkIntegrity();
        transferCars();
        return true;
    }
    //Collision detected, recursively call insert with incremented i.
    else {
        return insert(car, i + 1);
    }
}

void CarDB::insertNoCheck(Car car, int i){
    /*Copy of insert function but without integrity check.
      Required due to the fact that if insert() calls transferCars() and transferCars
      utilizes insert, then an infinite loop occurs.
    */

    //Ensuring car dealer's id is valid.
    if (car.m_dealer < MINID || car.m_dealer > MAXID) {return;}

    //Calcuating index to insert car into.
    int index;
    if (m_currProbing == QUADRATIC){ //Quadratic probing
        index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
    }
    else{ //Double hash probing
        index = ((m_hash(car.m_model) % m_currentCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_currentCap;
    }

    //If identical car obj already exists, return.
    if (m_currentTable[index] == car){return;}
    //If index after hashing lands on an EMPTY car, insert car.
    else if (m_currentTable[index] == EMPTY){
        m_currentTable[index] = car;
        m_currentSize++;
        return;
    }
    //If index is not EMPTY but is deleted object, insert car.
    else if (!m_currentTable[index].m_used){
        m_currentTable[index] = car;
        m_currNumDeleted--;
        return;
    }
    //Collision detected, recursively call insert with incremented i.
    else {
        insertNoCheck(car, i + 1);
    }
}

void CarDB::checkIntegrity(){
    //Creating new table when load factor is > 0.5 or deleted ratio is > 0.8.
    if (lambda() > 0.5 || deletedRatio() > 0.8){
        int size = findNextPrime((m_currentSize - m_currNumDeleted) * 4);

        //Setting current data to be marked as old data.
        m_oldCap = m_currentCap;
        m_oldProbing = m_currProbing;
        m_oldTable = m_currentTable;

        //Setting oldSize to size of all live nodes and numDeleted to 0 for transferring purposes.
        m_oldSize = m_currentSize - m_currNumDeleted;
        m_oldNumDeleted = 0;

        //Allocating memeory for the new current table and setting members.
        m_currentTable = new Car[size];
        for (int i = 0; i < size; i++){
            m_currentTable[i] = EMPTY;
        }
        m_currentSize = 0;
        m_currentCap = size;
        m_currNumDeleted = 0;
        if (m_newPolicy == NONE){
            m_currProbing = m_oldProbing;
        } else {
            m_currProbing = m_newPolicy;
        }
        //Calling transfer cars to transfer 25% of the elements from oldTable to currentTable.
        transferCars();
    }
}

void CarDB::transferCars(){
    float flag;
    int i = 0;

    //Calculating up to what deletion ratio on our oldTable to hit when transferring from old to current.
    if (deletedRatioOld() < 0.25){flag = 0.25;}
    else if (deletedRatioOld() >= 0.25 && deletedRatioOld() < 0.5) {flag = 0.5;}
    else if (deletedRatioOld() >= 0.5 && deletedRatioOld() < 0.75) {flag = 0.75;}
    else {flag = 1;}

    //Transfer nodes from oldTable to currentTable up to specified deletion ratio flag.
    while (deletedRatioOld() < flag){
        if (m_oldTable[i].m_used){
            insertNoCheck(m_oldTable[i], 0);
            removeOldNoCheck(m_oldTable[i], 0);
            i++;
        } else {i++;}
    }

    //Delete oldTable if 
    if (deletedRatioOld() == 1){
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }
}

bool CarDB::remove(Car car){
    //If oldTable exists attempt to remove.
    if (m_oldTable){
        //If found in oldTable, remove and return true.
        if (removeOld(car, 0)) {return true;}
        //Else attempt to find it in currentTable and return whether a removal happened.
        else {return removeCurrent(car, 0);}
    }
    return removeCurrent(car, 0);
}

bool CarDB::removeCurrent(Car car, int i){
    int index;
    if (m_currProbing == QUADRATIC){ //Quadratic probing
        index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
    }
    else{ //Double hash probing
        index = ((m_hash(car.m_model) % m_currentCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_currentCap;
    }

    //If we reach an index with EMPTY, then car does not exist in table.
    if (m_currentTable[index] == EMPTY) {
        checkIntegrity();
        transferCars();
        return false;}
    //If we reach a deleted node, car may still be in our table.
    if (!m_currentTable[index].m_used){
        return removeCurrent(car, i + 1);
    }
    //If we find existing car but with different ID, car may still be in our table.
    if (m_currentTable[index].m_dealer != car.m_dealer){
        return removeCurrent(car, i + 1);
    }
    //Case where car is found.
    else{
        m_currentTable[index].m_used = false;
        m_currNumDeleted++;
        checkIntegrity();
        transferCars();
        return true;
    }
}

bool CarDB::removeOld(Car car, int i){
    int index;
    if (m_oldProbing == QUADRATIC){
        index = ((m_hash(car.m_model) % m_oldCap) + (i * i)) % m_oldCap;
    }
    else{
        index = ((m_hash(car.m_model) % m_oldCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_oldCap;
    }

    if (m_oldTable[index] == EMPTY) {
        checkIntegrity();
        transferCars();
        return false;
    }
    if (!m_oldTable[index].m_used){
        return removeOld(car, i + 1);
    }
    if (m_oldTable[index].m_dealer != car.m_dealer){
        return removeOld(car, i + 1);
    }
    else{
        m_oldTable[index].m_used = false;
        m_oldNumDeleted++;
        checkIntegrity();
        transferCars();
        return true;
    }
}

void CarDB::removeOldNoCheck(Car car, int i){
    int index;
    if (m_oldProbing == QUADRATIC){
        index = ((m_hash(car.m_model) % m_oldCap) + (i * i)) % m_oldCap;
    }
    else{
        index = ((m_hash(car.m_model) % m_oldCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_oldCap;
    }

    if (m_oldTable[index] == EMPTY) {return;}
    if (!m_oldTable[index].m_used){
        removeOldNoCheck(car, i + 1);
        return;
    }
    if (m_oldTable[index].m_dealer != car.m_dealer){
        removeOldNoCheck(car, i + 1);
        return;
    }
    else{
        m_oldTable[index].m_used = false;
        m_oldNumDeleted++;
        return;
    }
}

Car CarDB::getCar(string model, int dealer) const{
    //Checking if car is in current table.
    if (getCar(model, dealer, 0, m_currentTable) == EMPTY){
        //If not in current, check old (if it exists) and return result.
        if (m_oldTable){
            return getCar(model, dealer, 0, m_oldTable);
        }
        return EMPTY;
    }
    return getCar(model, dealer, 0, m_currentTable);
}

Car CarDB::getCar(string model, int dealer, int i, Car* table) const{
    int index;

    //Searching in current table if table passed in is the current table.
    if (table == m_currentTable){
        //Finding index.
        if (m_currProbing == QUADRATIC){
            index = ((m_hash(model) % m_currentCap) + (i * i)) % m_currentCap;
        }
        else{
            index = ((m_hash(model) % m_currentCap) + i * (11 - (m_hash(model) % 11))) % m_currentCap;
        }

        //If we reach an EMPTY spot, then car doesn't exist in table.
        if (table[index] == EMPTY) {return EMPTY;}
        //If we reach a deleted car, recursively call getCar.
        if (!table[index].m_used) {return getCar(model, dealer, i + 1, table);}
        //If car is found, return it.
        if (table[index].m_model == model && table[index].m_dealer == dealer) {return table[index];}
        //else continue searching.
        else{return getCar(model, dealer, i + 1, table);}
    }

    if (table == m_oldTable){
        //Finding index.
        if (m_oldProbing == QUADRATIC){
            index = ((m_hash(model) % m_oldCap) + (i * i)) % m_oldCap;
        }
        else{
            index = ((m_hash(model) % m_oldCap) + i * (11 - (m_hash(model) % 11))) % m_oldCap;
        }

        //If we reach an EMPTY spot, then car doesn't exist in table.
        if (table[index] == EMPTY) {return EMPTY;}
        //If we reach a deleted car, recursively call getCar.
        if (!table[index].m_used) {return getCar(model, dealer, i + 1, table);}
        //If car is found, return it.
        if (table[index].m_model == model && table[index].m_dealer == dealer) {return table[index];}
        //else continue searching.
        else{return getCar(model, dealer, i + 1, table);}
    }

    //Placed return here to make compiler happy, control never reaches here.
    return EMPTY;
}

float CarDB::lambda() const {
    return (float(m_currentSize) / m_currentCap);
}

float CarDB::deletedRatio() const {
    return (float(m_currNumDeleted) / m_currentSize);
}

float CarDB::deletedRatioOld() const {
    return (float(m_oldNumDeleted) / m_oldSize);
}

void CarDB::dump() const {
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

bool CarDB::updateQuantity(Car car, int quantity){
    int index;

    //If oldTable exists, check if car exists in table.
    if (m_oldTable){
        index = search(car, 0, m_oldTable);
        cout << index << endl;
        //If index != -1, then car was found and we update quantity.
        if (index != -1){
            m_oldTable[index].m_quantity = quantity;
            return true;
        }
    }
    
    //If index was -1, we search current table.
    index = search(car, 0, m_currentTable);
    //If index != -1, then car was found and we update quantity.
    if(index != -1){
        m_currentTable[index].m_quantity = quantity;
        return true;
    }
    return false;
}

int CarDB::search(Car car, int i, Car* table){
    int index;

    if (table == m_oldTable){
        if (m_oldProbing == QUADRATIC){
        index = ((m_hash(car.m_model) % m_oldCap) + (i * i)) % m_oldCap;
        } else {
        index = ((m_hash(car.m_model) % m_oldCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_oldCap;
        }

        if (m_oldTable[index] == EMPTY) {return -1;}
        if (!m_oldTable[index].m_used){
            return search(car, i + 1, m_oldTable);
        }
        if(car.m_model == m_oldTable[index].m_model && car.m_dealer == m_oldTable[index].m_dealer){
            return index;
        }
        else {return search(car, i + 1, m_oldTable);}
    }

    if (table == m_currentTable){
        if (m_currProbing == QUADRATIC){
        index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
        } else {
        index = ((m_hash(car.m_model) % m_currentCap) + i * (11 - (m_hash(car.m_model) % 11))) % m_currentCap;
        }

        if (m_currentTable[index] == EMPTY) {return -1;}
        if (!m_currentTable[index].m_used){
            return search(car, i + 1, m_currentTable);
        }
        if(car.m_model == m_currentTable[index].m_model && car.m_dealer == m_currentTable[index].m_dealer){
            return index;
        }
        else {return search(car, i + 1, m_currentTable);}
    }

    //Placed return here to make compiler happy, control never reaches here.
    return 0;
}

bool CarDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current){
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

ostream& operator<<(ostream& sout, const Car &car ) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity<< ")";
    else
        sout << "";
  return sout;
}

bool operator==(const Car& lhs, const Car& rhs){
    // since the uniqueness of an object is defined by model and dealer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}
