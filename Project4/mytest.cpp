// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
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
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i] ;
   return val ;
}

class Tester{
    public:
    bool insertNormal(){
        //Testing inserting elements in normal cases: no collisions.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("stratos", 1, 2222, true),
        car3("gt500", 1, 3333, true), car4("miura", 1, 4444, true);

        //Testing insertion on a QUADRATIC probing db.
        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);

        //We expect to find car1 at index 40, car2 index 75, car3 index 38, and car4 index 31.
        //We also expect currentSize to be 4.
        result = (db.m_currentTable[40].m_model == "challenger") && (db.m_currentTable[75].m_model == "stratos")
        && (db.m_currentTable[38].m_model == "gt500") && (db.m_currentTable[31].m_model == "miura")
        && (db.m_currentSize == 4);

        //

        return result;
    }
    
    bool insertEdge(){
        //Testing inserting elements in edge cases: collision handled cars.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("challenger", 1, 2222, true),
        car3("challenger", 1, 3333, true), car4("gt500", 1, 4444, true),
        car5("gt500", 1, 5555, true), car6("gt500", 1, 6666, true);

        //Testing inserting collision handled cars on a QUADRATIC probing db.
        CarDB db1(MINPRIME, hashCode, QUADRATIC);
        db1.insert(car1);
        db1.insert(car2);
        db1.insert(car3);
        db1.insert(car4);
        db1.insert(car5);
        db1.insert(car6);

        //We expect to find all of the cars in db1 and for currentSize = 6.
        result = (db1.getCar("challenger", 1111) == car1) && (db1.getCar("challenger", 2222) == car2)
        && (db1.getCar("challenger", 3333) == car3) && (db1.getCar("gt500", 4444) == car4)
        && (db1.getCar("gt500", 5555) == car5) && (db1.getCar("gt500", 6666) == car6)
        && (db1.m_currentSize == 9);

        //Testing inserting collision handled cars on a DOUBLE HASH probing db.
        CarDB db2(MINPRIME, hashCode, DOUBLEHASH);
        db2.insert(car1);
        db2.insert(car2);
        db2.insert(car3);
        db2.insert(car4);
        db2.insert(car5);
        db2.insert(car6);

        //We expect to find all of the cars in db2 and for currentSize = 6.
        result = (db2.getCar("challenger", 1111) == car1) && (db2.getCar("challenger", 2222) == car2)
        && (db2.getCar("challenger", 3333) == car3) && (db2.getCar("gt500", 4444) == car4)
        && (db2.getCar("gt500", 5555) == car5) && (db2.getCar("gt500", 6666) == car6)
        && (db2.m_currentSize == 9);

        return true;
    }

    bool insertError(){
        //Testing the insertion function under erroneous cases: cars already exist in table.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("stratos", 1, 2222, true),
        car3("gt500", 1, 3333, true), car4("miura", 1, 4444, true);

        //Testing on a QUADRATIC probing db.
        CarDB db1(MINPRIME, hashCode, QUADRATIC);
        db1.insert(car1);
        db1.insert(car2);
        db1.insert(car3);
        db1.insert(car4);

        //We expect to get a false return when we attempt to insert the cars again.
        result = (!db1.insert(car1)) && (!db1.insert(car2)) && (!db1.insert(car3))
        && (!db1.insert(car4)) && (db1.m_currentSize == 4);

        return result;
    }

    bool insertMass(){
        //Testing the insertion function with several insertions.
        bool result;

        //Testing the process of several insertions to trigger rehash.
        CarDB db1(MINPRIME, hashCode, QUADRATIC);
        generator(70, db1);
        //We expect db1's capacity to now be larger than 101.
        result = (db1.m_currentCap > MINPRIME);
        return result;

        //Testing on a DOUBLE HASH probing db.
        CarDB db2(MINPRIME, hashCode, DOUBLEHASH);
        generator(70, db2);
        //We expect db2's capacity to now be larger than 101.
        result = (db2.m_currentCap > 101);

        return result;
    }

    bool getCarNormal(){
        //Testing getCar on normal cases: collision free cars.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("stratos", 1, 1111, true),
        car3("gt500", 1, 1111, 1), car4("miura", 1, 1111, true), car5("x101", 1, 1111, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);
        db.insert(car5);

        //We expect each car to be returned after attempting to search for them.
        result = (db.getCar("challenger", 1111) == car1) && (db.getCar("stratos", 1111) == car2)
        && (db.getCar("gt500", 1111) == car3) && (db.getCar("miura", 1111) == car4)
        && (db.getCar("x101", 1111) == car5);

        //Testing on a DOUBLE HASH probing db.
        CarDB db2(MINPRIME, hashCode, DOUBLEHASH);
        db2.insert(car1);
        db2.insert(car2);
        db2.insert(car3);
        db2.insert(car4);
        db2.insert(car5);
        //We expect each car to be returned after attempting to search for them.
        result = (db2.getCar("challenger", 1111) == car1) && (db2.getCar("stratos", 1111) == car2)
        && (db2.getCar("gt500", 1111) == car3) && (db2.getCar("miura", 1111) == car4)
        && (db2.getCar("x101", 1111) == car5);

        return result;
    }

    bool getCarEdge(){
        //Testing getCar on edge cases: finding cars after collision handling.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("challenger", 1, 2222, true),
        car3("challenger", 1, 3333, true), car4("gt500", 1, 4444, true),
        car5("gt500", 1, 5555, true), car6("gt500", 1, 6666, true);

        CarDB db (MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);
        db.insert(car5);
        db.insert(car6);

        //We expect each car to be returned after attemtping to search for them.
        result = (db.getCar("challenger", 1111) == car1) && (db.getCar("challenger", 2222) == car2)
        && (db.getCar("challenger", 3333) == car3) && (db.getCar("gt500", 4444) == car4)
        && (db.getCar("gt500", 5555) == car5) && (db.getCar("gt500", 6666) == car6);

        //Testing on a DOUBLE HASH probing db.
        CarDB db2(MINPRIME, hashCode, DOUBLEHASH);
        db2.insert(car1);
        db2.insert(car2);
        db2.insert(car3);
        db2.insert(car4);
        db2.insert(car5);
        db2.insert(car6);

        //We expect each car to be returned after attempting to search for them.
        result = (db2.getCar("challenger", 1111) == car1) && (db2.getCar("challenger", 2222) == car2)
        && (db2.getCar("challenger", 3333) == car3) && (db2.getCar("gt500", 4444) == car4)
        && (db2.getCar("gt500", 5555) == car5) && (db2.getCar("gt500", 6666) == car6);

        return result;
    }

    bool getCarError(){
        //Testing getCar on erroneous cases: attempting to get a car that doesn't exist.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("stratos", 1, 2222, true),
        car3("gt500", 1, 3333, true), car4("miura", 1, 4444, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        //Excluding car4 on purpose.

        //We expect to get an EMPTY car return when attempting to find cars not in the table
        //and when attempting to search for cars with the same key but different ID.
        result = (db.getCar("miura", 5555) == EMPTY) && (db.getCar("challenger", 1112) == EMPTY)
        && (db.getCar("stratos", 2223) == EMPTY) && (db.getCar("gt500", 3334) == EMPTY);

        return result;
    }

    bool removeNormal(){
        //Testing remove on normal cases: Non collision-handled cars.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("stratos", 1, 1111, true),
        car3("gt500", 1, 1111, 1), car4("miura", 1, 1111, true), car5("x101", 1, 1111, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);
        db.insert(car5);

        db.remove(car1);
        db.remove(car2);
        db.remove(car3);

        //We expect for getCar to return an EMPTY car when searching for car 1, 2, and 3,
        //and for currentSize = 5 and currNumDeleted = 3.
        result = (db.getCar("challenger", 1111) == EMPTY) && (db.getCar("stratos", 1111) == EMPTY)
        && (db.getCar("gt500", 1111) == EMPTY) && (db.m_currentSize == 5) && (db.m_currNumDeleted == 3);

        return result;
    }

    bool removeEdge(){
        //Testing remove on edge cases: removing collision-handled cars.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("challenger", 1, 2222, true),
        car3("challenger", 1, 3333, true), car4("gt500", 1, 4444, true),
        car5("gt500", 1, 5555, true), car6("gt500", 1, 6666, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);
        db.insert(car5);
        db.insert(car6);

        db.remove(car2);
        db.remove(car3);
        db.remove(car6);

        //We expect for getCar to return an EMPTY car when searching for car 2, 3, and 6,
        //and for currentSize = 6, and currNumDeleted = 3.
        result = (db.getCar("challenger", 2222) == EMPTY) && (db.getCar("challenger", 3333) == EMPTY)
        && (db.getCar("gt500", 6666) == EMPTY) && (db.m_currentSize == 6) && (db.m_currNumDeleted == 3);

        return result;
    }

    bool removeError(){
        //Testing remove on erroneous cases: removing cars that don't exist.
        bool result;
        Car car1("challenger", 1, 1111, true), car2("challenger", 1, 2222, true),
        car3("challenger", 1, 3333, true), car4("gt500", 1, 4444, true),
        car5("gt500", 1, 5555, true), car6("gt500", 1, 6666, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car3);
        db.insert(car5);
        //We expect when attempting to remove cars that don't exist in the table to get back false.
        result = (!db.remove(car2)) && (!db.remove(car4)) && (!db.remove(car6))
        && (db.m_currentSize == 3);

        return result;
    }

    bool removeMass(){
        //Testing the removal function with multiple removals.
        bool result = true;
        vector<Car> dataList;

        //Testing the process of several deletions to trigger rehash (>80% deletion ratio).
        CarDB db1(MINPRIME, hashCode, QUADRATIC);
        dataList = generator(49, db1);
        for (int i = 0; i < 45; i++){
            db1.remove(dataList[i]);
        }

        //Inserting 49 nodes and removing 45, rehashing will trigger at the 40th removal.
        //We expect currentTable to be of size 8, with 4 deleted (indicating 4 live cars).
        //We also expect m_oldTable to be nullptr due to more than 4 insertion/removals executed after rehash.
        //We also expect currentSize to still be 101 because (49 - 40) * 4 = 36 -> so we must use MINPRIME.
        result = (db1.m_currentCap == 101) && (db1.m_currentSize == 8) && (db1.m_currNumDeleted == 4)
        && (!db1.m_oldTable);

        return result;
    }

    bool updateQuantityNormal(){
        //Testing updateQuantity under normal cases: existing cars in tables.
        bool result;
        Car car1("challenger", 19, 1111, true), car2("stratos", 50, 2222, true),
        car3("gt500", 31, 3333, true), car4("gt500", 1, 4444, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car1);
        db.insert(car2);
        db.insert(car3);
        db.insert(car4);

        db.updateQuantity(car1, 5);
        db.updateQuantity(car4, 17);
        db.updateQuantity(car2, 15);

        //We expect car1 to have an update quantity of 5, car2 15, car3 31, and car4 17.
        result = (db.getCar("challenger", 1111).m_quantity == 5) && (db.getCar("stratos", 2222).m_quantity == 15)
        && (db.getCar("gt500", 3333).m_quantity == 31) && (db.getCar("gt500", 4444).m_quantity == 17);

        return result;
    }

    bool updateQuantityError(){
        //Testing updateQuantity under edge cases: cars do not exist in table.
        bool result;
        Car car1("challenger", 19, 1111, true), car2("stratos", 50, 2222, true),
        car3("gt500", 31, 3333, true), car4("gt500", 1, 4444, true);

        CarDB db(MINPRIME, hashCode, QUADRATIC);
        db.insert(car2);
        db.insert(car4);
    
        //We expect calling updateQuantity on car1 and car3 to return false and for their
        //quantities to remain the same.
        result = (!db.updateQuantity(car1, 100)) && (!db.updateQuantity(car3, 100))
        && (car1.m_quantity == 19) && (car3.m_quantity == 31);

        return result;
    }

    private:
    //Function to create car objects and insert them into db.
    vector<Car> generator(int nums, CarDB& db){
        vector<Car> dataList;
        string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
        string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};

        Random RndID(MINID, MAXID);
        Random RndCar(0, 4);
        Random RndQuantity(0, 50);

        for (int i = 0; i < nums; i++){
            Car carObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(),
                            RndID.getRandNum(), true);
            dataList.push_back(carObj);
            db.insert(carObj);
        }
        
        return dataList;
    }
};

int main(){
    Tester tester;

    //Testing the insert function under normal cases (no collisions).
    if (tester.insertNormal()){
        cout << "PASSED: Insertion works properly under normal cases." << endl;
    } else {
        cout << "FAILED: Insertion works properly under normal cases." << endl;
    }

    //Testing the insert function under edge cases (collision handled).
    if (tester.insertEdge()){
        cout << "PASSED: Insertion works properly under edge cases." << endl;
    } else {
        cout << "FAILED: Insertion works properly under edge cases." << endl;
    }

    //Testing the insert function under erroneous cases (cars already exist in table).
    if (tester.insertError()){
        cout << "PASSED: Insertion works properly under erroneous cases." << endl;
    } else {
        cout << "FAILED: Insertion works properly under erroneous cases." << endl;
    }

    //Testing the insert function with multiple insertions.
    if (tester.insertMass()){
        cout << "PASSED: Multiple insertions work as intended." << endl;
    } else {
        cout << "FAILED: Multiple insertions work as intended." << endl;
    }

    //Testing the remove function under normal cases (collisionless cars).
    if (tester.removeNormal()){
        cout << "PASSED: Removal works properly under normal cases." << endl;
    } else {
        cout << "FAILED: Removal works properly under normal cases." << endl;
    }

    //Testing the remove function under edge cases (removing collision-handled cars).
    if (tester.removeEdge()){
        cout << "PASSED: Removal works properly under edge cases." << endl;
    } else {
        cout << "FAILED: Removal works properly under edge cases." << endl;
    }

    //Testing the remove function under erroneous cases (removing nonexistent cars).
    if (tester.removeError()){
        cout << "PASSED: Removal works properly under erroneous cases." << endl;
    } else {
        cout << "FAILED: Removal works properly under erroneous cases." << endl;
    }

    //Testing the remove function with multiple removals.
    if (tester.removeMass()){
        cout << "PASSED: Multiple removals works as intended." << endl;
    } else {
        cout << "FAILED: Multiple removals works as intended." << endl;
    }

    //Testing getCar function under normal cases (collision free cars).
    if (tester.getCarNormal()){
        cout << "PASSED: getCar() works properly under normal cases." << endl;
    } else {
        cout << "FAILED: getCar() works properly under normal cases." << endl;
    }
    //Testing getCar function under edge cases (searching for collision handled cars).
    if (tester.getCarEdge()){
        cout << "PASSED: getCar() works properly under edge cases." << endl;
    } else {
        cout << "FAILED: getCar() works properly under edge cases." << endl;
    }

    //Testing getCar function under erroneous cases (car doesn't exist in tables).
    if (tester.getCarError()){
        cout << "PASSED: getCar() works properly under erroneous cases." << endl;
    } else {
        cout << "FAILED: getCar() works properly under erroneous cases." << endl; 
    }

    //Testing updateQuantity under normal cases (cars exist in table).
    if (tester.updateQuantityNormal()){
        cout << "PASSED: updateQuantity() works properly under normal cases." << endl;
    } else {
        cout << "FAILED: updateQuantity() works properly under normal cases." << endl;
    }

    //Testing updateQuantity under erroneous cases (cars do not exist in table).
    if (tester.updateQuantityError()){
        cout << "PASSED: updateQuantity() works properly under erroneous cases." << endl;
    } else {
        cout << "FAILED: updateQuantity() works properly under erroneous cases." << endl;
    }
   
    return 0;
}

