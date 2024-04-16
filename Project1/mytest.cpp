// CMSC 341 - Fall 2023 - Project 1
#include "csr.h"

class Tester{
    public:
    bool constructorEmpty(){
        //Creating a CSR object with no parameters should result in a
        //empty CSR object
        CSR CSR1;
        if (CSR1.empty()){
            return true;
        }
        return false;
    }

    bool constructorCopy(){
        //Using the copy constructor should construct an object that is
        //identical to the CSR object passed in as the argument
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing copy constructor with rhs being a compressed CSR.
        CSR CSR1;
        CSR1.compress(3, 3, array, 9);
        CSR CSR2(CSR1);

        //We expect CSR2 == CSR1
        result = (CSR1.m_m == CSR2.m_m && CSR1.m_n == CSR2.m_n
        && CSR1.m_nonzeros == CSR2.m_nonzeros);

        for (int i = 0; i < CSR1.m_nonzeros; i++){
            result = result && (CSR1.m_values[i] == CSR2.m_values[i]);
            result = result && (CSR1.m_col_index[i] == CSR2.m_col_index[i]);
        }
        for (int j = 0; j < CSR1.m_m + 1; j++){
            result = result && (CSR1.m_row_index[j] == CSR2.m_row_index[j]);
        }
        
        //Testing copy constructor with rhs being an empty CSR.
        CSR CSR3;
        CSR CSR4(CSR3);
        //We expect CSR4 == CSR3, a empty CSR object.
        result = result && (CSR3.empty() && CSR4.empty());

        return result;
    }

    bool compressNormal(){
        //Testing compress() under normal cases.
        bool result = true;
        int array1[] = {100,0,0,0,200,0,0,0,300};
        int array2[] = {0, 0, 0};

        //Testing compress() given a matching matrix size and array size.
        CSR CSR1;
        CSR1.compress(3, 3, array1, 9);
        /*We expect all of our arrays to contain the following:
        m_values = [100, 200, 300]
        m_col_index = [0, 1, 2]
        m_row_index = [0, 1, 2, 3]
        */
        result = result && (CSR1.m_values[0] == 100) && (CSR1.m_values[1] == 200)
        && (CSR1.m_values[2] == 300); //Checking m_values

        for (int i = 0; i < 3; i++){ //Checking m_col_index
            result = result && (CSR1.m_col_index[i] == i);
        }
        for (int j = 0; j < 4; j++){ //Checking m_row_index
            result = result && (CSR1.m_row_index[j] == j);
        }

        //Testing compress() given a valid matrix size but only given zeros
        CSR CSR2;
        CSR2.compress(1, 1, array2, 3);
        //We expect m_m = 1, m_n = 1, m_nonzeros = 0, have a sparseRatio of 100
        // and obj not considered empty.
        result = result && (CSR2.m_m == 1) && (CSR2.m_n = 1) && (!CSR2.empty())
        && (CSR2.m_nonzeros == 0) && (CSR2.sparseRatio() == 100);
        return result;
    }

    bool compressError(){
        //Testing compress() under erroneous/edge cases.
        bool result = true;
        int array1[] = {100,0,0,0,200,0,0,0,300};

        //Testing compress with 0 rows and cols.
        CSR CSR1;
        CSR1.compress(0, 0, array1, 9);
        //We expect CSR1 to be considered an empty object.
        result = result && CSR1.empty();

        //Testing compress with more values than matrix size requested.
        CSR CSR2;
        CSR2.compress(2, 2, array1, 9);
        /*We expect CSR2 to only contain the following:
        m_values = [100]
        m_col_index = [0]
        m_row_index = [0, 1]
        and have a sparseRatio of 75
        */
        result = result && (CSR2.m_values[0] == 100) && (CSR2.m_col_index[0] == 0)
        && (CSR2.m_row_index[0] == 0) && (CSR2.m_row_index[1] == 1) && (CSR2.sparseRatio() == 75);
        
        //Testing compress with matrix size larger than inputs given.
        CSR CSR3;
        CSR3.compress(10, 10, array1, 9);
        /*We expect CSR3 to contain the following:
        m_values = [100, 200, 300]
        m_col_index = [0, 4, 8]
        m_row_index = [0, 3, ...]    <- 3's up to end of m_row_index
        and have a sparseRatio of 97
        */
        result = result && (CSR3.m_values[0] == 100) && (CSR3.m_values[1] == 200)
        && (CSR3.m_values[2] == 300) && (CSR3.m_col_index[0] == 0)
        && (CSR3.m_col_index[1] == 4) && (CSR3.m_col_index[2] == 8)
        && (CSR3.sparseRatio() == 97); //Verifying all values

        result == result && (CSR1.m_row_index[0] == 0) && (CSR1.m_row_index[1] == 3)
        && (CSR1.m_row_index[CSR1.m_m] == 3); //Checking index 0, 1, and last.
        return result;
    }

    bool emptyTest(){
        //Testing empty() on empty and nonempty CSR objects.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};
        
        //We expect empty to return true if empty() is called on an empty CSR.
        CSR CSR1;
        result = result && (CSR1.empty());
        //We expect empty to return false if empty() is called on an nonempty CSR.
        CSR CSR2;
        CSR2.compress(3, 3, array, 9);
        result = result && !(CSR2.empty());

        return result;
    }

    bool clearTest(){
        //Testing clear() on empty and nonempty CSR objects.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing clear on empty CSR.
        CSR CSR1;
        CSR1.clear();
        //We expect an empty object to remain empty after calling clear().
        result = result && CSR1.empty();

        //Testing clear on nonempty CSR.
        CSR CSR2;
        CSR2.compress(3, 3, array, 9);
        CSR2.clear();
        //We expect an nonempty object to become empty after calling clear().
        result = result && CSR2.empty();

        return result;
    }

    bool sparseRatioNormal(){
        //Testing sparseRatio() under normal cases.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing sparseRatio() on a nonempty CSR object.
        CSR CSR1;
        CSR1.compress(3, 3, array, 9);
        //We expect 66 to be returned because 6/9 * 100 = 66
        return result = result && (CSR1.sparseRatio() == 66);
    }

    bool sparseRatioEdge(){
        //Testing sparseRatio() under edge cases.
        bool result = true;
        int array1[] = {0};
        int array2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        //Testing sparseRatio() on a CSR object filled with zeroes;
        CSR CSR1;
        CSR1.compress(1, 1, array1, 1);
        //We expect 100 to be returned because 0 is the only value.
        result = result && (CSR1.sparseRatio() == 100);

        //Testing sparseRatio() on a CSR object filled with nonzeros.
        CSR CSR2;
        CSR2.compress(3, 3, array2, 9);
        //We expect 0 to be returned because there are no zeros.
        result = result && (CSR2.sparseRatio() == 0);
        return result;
    }

    bool sparseRatioError(){
        //Testing sparseRatio() on an empty object.
        CSR CSR1;
        //We expect sparseRatio() to return 0;
        return CSR1.sparseRatio() == 0;
    }

    bool getAtNormal(){
        //Testing getAt() under normal cases.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing getAt() on known value locations of an nonempty CSR.
        CSR CSR1;
        CSR1.compress(3, 3, array, 9);
        //We expect to return 100 at row 1, col 1.
        result = result && (CSR1.getAt(1, 1) == 100);
        //We expect to return 200 at row 1, col 2.
        result = result && (CSR1.getAt(1, 2) == 200);
        //We expect to return 0 at row 0, col 1.
        result = result && (CSR1.getAt(0, 1) == 0);
        return result;
    }
    
    bool getAtEdge(){
        //Testing getAt() under edge cases.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing getAt() at the beginning and end of our matrix.
        CSR CSR1;
        CSR1.compress(3, 3, array, 9);
        //We expect to return 0 at row 0, col 0.
        result = result && (CSR1.getAt(0, 0) == 0);
        //We expect to return 300 at row 2, col 2.
        result == result && (CSR1.getAt(2, 2) == 300);
        return result;
    }

    bool getAtError(){
        //Testing getAt() under erroneous cases.
        bool result = false;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Testing getAt() on empty CSR object.
        CSR CSR1;
        try{
            CSR1.getAt(0, 0);
        }
        //We expect for an error to be thrown.
        catch (const std::runtime_error &e) {
            result = true;
        }

        //Testing getAt() on an invalid index of an nonempty CSR object.
        CSR CSR2;
        CSR2.compress(3, 3, array, 9);
        try{
            CSR2.getAt(3, 3);
        }
        //We expect for an error to be thrown.
        catch (const std::runtime_error &f){
            result = result && true;
        }
        return result;
    }

    /******************************
    *Testing functions for CSRList*
    ******************************/

    bool listConstructorEmpty(){
        //Testing the default constructor of CSRList.
        
        //Creating a CSRList with the default constructor.
        CSRList CSRL1;
        //We expect CSRL1 to be a CSRList object with m_size 0 and m_head = nullptr
        return (CSRL1.m_size == 0) && (!CSRL1.m_head);
    }

    bool listConstructorCopy(){
        //Testing the copy constructor of CSRList for all cases.
        bool result = true;

        //Testing the copy constructor when copying an empty CSRList(edge).
        CSRList CSRL1;
        CSRList CSRL2(CSRL1);
        //We expect CSRL2 to be an empty CSRList.
        result = result && (CSRL2.empty());

        //Testing the copy constructor when copying a nonempty CSRList(normal).
        CSR CSR1;
        CSRL1.insertAtHead(CSR1);
        CSRList CSRL3(CSRL1);
        //We expect CSRL3 to be of m_size = 1 and for it's m_head == CSRL1.m_head.
        result = result && (*CSRL1.m_head == *CSRL3.m_head) && (CSRL3.m_size == 1);

        return result;
    }

    bool listEmptyTest(){
        //Testing the empty() function of CSRList.
        bool result = true;

        //Testing empty() on an empty list.
        CSRList CSRList1;
        if (CSRList1.empty()){
            result = result && true;
        } else {
            result == result && false;
        }

        //Testing empty() on an nonempty list.
        CSR CSR1;
        CSRList1.insertAtHead(CSR1);
        if (!CSRList1.empty()){
            result = result && true;
        } else {
            result == result && false;
        }
        return result;
    }

    bool listClearTest(){
        //Testing the clear() function of CSRList for all cases.
        bool result = true;

        //Testing clear() on an empty list (error).
        CSRList CSRL1;
        CSRL1.clear();
        //We expect CSRL1 to remain an empty list with m_size = 0.
        result = result && (CSRL1.empty()) && (CSRL1.m_size == 0);

        //Testing clear() on a list with 1 element (normal).
        CSR CSR1;
        CSRL1.insertAtHead(CSR1);
        CSRL1.clear();
        //We expect CSRL1 to be an empty list with m_size = 0.
        result = result && (CSRL1.empty()) && (CSRL1.m_size == 0);

        //Testing clear on a list with 2 or more elements (normal).
        CSR CSR2;
        CSR CSR3;
        CSRL1.insertAtHead(CSR2);
        CSRL1.insertAtHead(CSR3);
        CSRL1.clear();
        //We expect CSRL1 to be an empty list with m_size = 0.
        result = result && (CSRL1.empty()) && (CSRL1.m_size == 0);
        
        return result;
    }
    
    bool insertAtHeadTest(){
        //Testing insertAtHead() on an empty list.
        bool result = true;
        int array[] = {0,0,0,0,100,200,0,0,300};

        //Creating an list and then inserting a CSR object in it (edge).
        CSRList CSRList1;
        CSR CSR1;
        CSR1.compress(3, 3, array, 9);
        CSRList1.insertAtHead(CSR1);
        //We expect CSRList1's head to be the inserted node and the list to be not empty
        result = result && (*CSRList1.m_head == CSR1) && (CSRList1.m_size == 1);
        
        //Inserting another element into CSRList1 (normal).
        CSR CSR2;
        CSRList1.insertAtHead(CSR2);
        //We expect CSRList2's head to be the empty CSR object CSR2.
        result = result && (CSRList1.m_head->empty()) && (CSRList1.m_size == 2);
        return result;
    }

    bool averageSparseRatioTest(){
        //Testing averageSparseRatio() of CSRList for all cases.
        bool result = true;
        int array1[] = {0,0,0,0};
        int array2[] = {0,0,1,2};

        //Testing averageSparseRatio() on an empty CSRList (error).
        CSRList CSRL1;
        //We expect the average sparse ratio of CSRL1 to be 0.
        result = result && (CSRL1.averageSparseRatio() == 0);

        //Testing averageSparseRatio() on a list containing 1 element (edge).
        CSR CSR1;
        CSR1.compress(2, 2, array1, 4); //CSR with ratio of 100,
        CSRL1.insertAtHead(CSR1);
        //We expect the average sparse ratio of CSRL1 to be 100.
        result = result && (CSRL1.averageSparseRatio() == 100);

        //Testing averageSparseRatio() on a list with 2 or more elements (normal).
        CSR CSR2;
        CSR2.compress(2, 2, array2, 4); //CSR with ratio of 50.
        CSRL1.insertAtHead(CSR2);
        //We expect the average sparse ratio of CSRL1 to be 75.
        result = result && (CSRL1.averageSparseRatio() == 75);
        
        return result;
    }

    bool listGetAtTest(){
        //Testing CSRList's getAt() for all test cases.
        bool result = true;
        int totalCatches = 0; //To track how many catches were made.
        int array[] = {0, 1, 2, 3};

        //Testing getAt() when CSRList is empty (error).
        CSRList CSRL1;
        //We expect a runtime_error to be thrown.
        try{
            CSRL1.getAt(0, 0, 0);
        }
        catch (const std::runtime_error &e){
            totalCatches++;
        }

        //Testing getAt() when CSRList has objects but we go out of bounds (error).
        CSR CSR1;
        CSR1.compress(2, 2, array, 4);
        CSRL1.insertAtHead(CSR1);
        try{
            CSRL1.getAt(1, 0, 0);
        }
        //We expect a runtime_error to be thrown.
        catch (const std::runtime_error &f){
            totalCatches++;
        }

        //Testing getAt() on an object that exists in the list but going out of bounds (error).
        try{
            CSRL1.getAt(0, 2, 2);
        }
        //We expect a runtime_error to be thrown.
        catch (const std::runtime_error &g){
            totalCatches++;
        }

        //Testing getAt() in a normal case.
        try{
            int valReturned = CSRL1.getAt(0, 0, 1);
            result = result && (valReturned == 1);
        }
        catch (const std::runtime_error &h){
            totalCatches++;
        }

        return result && (totalCatches == 3); //We expect 3 catches.
    }

    bool listEqualityTest(){
        //Testing CSRList's overloaded == operator.
        bool result = true;
        int array1[] = {0,1,2,3};

        //Testing equality between two empty, equal CSRLists.
        CSRList CSRL1, CSRL2;
        //We expect the overloaded operator to return true.
        result = result && (CSRL1 == CSRL2);

        //Testing equality between two nonempty, equal CSRLists.
        CSR CSR1;
        CSR1.compress(2, 2, array1, 4);
        CSRL1.insertAtHead(CSR1);
        CSRL2.insertAtHead(CSR1);
        //We expect the overloaded operator to return true.
        result = result && (CSRL1 == CSRL2);

        //Testing equality between nonequal CSRLists.
        CSRL1.clear();
        //We expect the overloaded operator to return false.
        result = result && !(CSRL1 == CSRL2);

        return result;
    }

    bool listAssignmentTest(){
        //Testing CSRList's assignment operator for all cases.
        bool result = true;
        int array1[] = {0, 1, 2, 3};

        //Testing setting one empty CSRList to another empty CSRList (normal).
        CSRList CSRL1, CSRL2;
        //We expect both lists to remain empty and equivalent.
        result = result && (CSRL1.empty()) && (CSRL2.empty()) && (CSRL1 == CSRL2);

        //Testing setting one nonempty CSRList to another nonempty CSRList (normal).
        CSR CSR1, CSR2;
        CSR1.compress(2, 2, array1, 4);
        CSRL1.insertAtHead(CSR1);
        CSRL2.insertAtHead(CSR2);
        CSRL1 = CSRL2;
        //We expect CSRL1 to be equal to CSRL2.
        result = result && (CSRL1 == CSRL2);

        //Testing setting one nonempty CSRList to an empty CSRList (normal).
        CSRL2.clear();
        CSRL1 = CSRL2;
        //We expect CSRL1 to now be empty.
        result = result && (CSRL1.empty());
        
        return result;
    }
};
// this program presents a sample usage of project 1
int main()
{
    Tester tester;

    //Testing CSR's default constructor.
    if(tester.constructorEmpty()){
        cout << "PASSED: Initializing an empty CSR with default constructor." << endl;
    } else {
        cout << "FAILED: Initializing an empty CSR with default constructor." << endl;
    }

    //Testing CSR's copy constructor.
    if (tester.constructorCopy()){
        cout << "PASSED: Initializing CSR objects with copy constructor." << endl;
    } else {
        cout << "FAILED: Initializing CSR objects with copy constructor." << endl;
    }

    //Testing CSR's compression function under normal cases.
    if (tester.compressNormal()){
        cout << "PASSED: Compressing with matching parameters works as intended." << endl;
    } else {
        cout << "FAILED: Compressing with matching parameters works as intended." << endl;
    }

    //Testing CSR's compression function under erroneous cases.
    if (tester.compressError()){
        cout << "PASSED: Compressing with erroneous or mismatching params works as intended." << endl;
    } else {
        cout << "FAILED: Compressing with erroneous or mismatching params works as intended." << endl;
    }

    //Testing CSR's empty function.
    if (tester.emptyTest()){
        cout << "PASSED: The empty() function works as intended." << endl;
    } else {
        cout << "FAILED: The empty() function works as intended." << endl;
    }

    //Testing CSR's clear function.
    if (tester.clearTest()){
        cout << "PASSED: The clear() function works as intended." << endl;
    } else {
        cout << "FAILED: The clear() function works as intended." << endl;
    }

    //Testing CSR's sparse ratio function under normal cases.
    if (tester.sparseRatioNormal()){
        cout << "PASSED: sparseRatio() returns the correct ratio under normal cases." << endl;
    } else {
        cout << "FAILED: sparseRatio() returns the correct ratio under normal cases." << endl;
    }

    //Testing CSR's sparse ratio function under edge cases.
    if (tester.sparseRatioEdge()){
        cout << "PASSED: sparseRatio() returns the correct ratio under edge cases." << endl;
    } else {
        cout << "FAILED: sparseRatio() returns the correct ratio under edge cases." << endl;
    }

    //Testing CSR's sparse ratio function under erroneous cases.
    if (tester.sparseRatioError()){
        cout << "PASSED: sparseRatio() returns the correct ratio under erroneous cases." << endl;
    } else {
        cout << "FAILED: sparseRatio() returns the correct ratio under erroneous cases." << endl;
    }

    //Testing CSR's get at function under normal cases.
    if (tester.getAtNormal()){
        cout << "PASSED: getAt() returns the correct value in normal cases." << endl;
    } else {
        cout << "FAILED: getAt() returns the correct value in normal cases." << endl;
    }

    //Testing CSR's get at function under edge cases.
    if (tester.getAtEdge()){
        cout << "PASSED: getAt() returns the correct value under edge cases." << endl;
    } else {
        cout << "FAILED: getAt() returns the correct value under edge cases." << endl;
    }

    //Testing CSR's get at function under erroneous cases.
    if (tester.getAtError()){
        cout << "PASSED: getAt() throws runtime_error in erroneous indices." << endl;
    } else {
        cout << "FAILED: getAt() throws runtime_error in erroneous indices." << endl;
    }
    
    //Testing CSRList's default constructor.
    if (tester.listConstructorEmpty()){
        cout << "PASSED: CSRList default constructor works as intended." << endl;
    } else {
        cout << "FAILED: CSRList default constructor works as intended." << endl;
    }

    //Testing CSRList's copy constructor under all cases.
    if (tester.listConstructorCopy()){
        cout << "PASSED: CSRList copy constructor works on all test cases." << endl;
    } else {
        cout << "FAILED: CSRList copy constructor works on all test cases." << endl;
    }
    
    //Testing CSRList's empty function.
    if (tester.listEmptyTest()){
        cout << "PASSED: CSRList empty() works as intended." << endl;
    } else {
        cout << "FAILED: CSRList empty() works as intended." << endl;
    }

    //Testing CSRList's clear function under all cases.
    if (tester.listClearTest()){
        cout << "PASSED: CSRList clear() works on all test cases." << endl;
    } else {
        cout << "FAILED: CSRList clear() works on all test cases." << endl;
    }

    //Testing CSRList's insertAtHead function under all cases.
    if (tester.insertAtHeadTest()){
        cout << "PASSED: CSRList insertAtHead() correctly inserts at head." << endl;
    } else {
        cout << "FAILED: CSRList insertAtHead() correctly inserts at head." << endl;
    }

    //Testing CSRList's getAt function under all cases.
    if (tester.listGetAtTest()){
        cout << "PASSED: CSRList getAt() works on all test cases." << endl;
    } else {
        cout << "FAILED: CSRList getAt() works on all test cases." << endl;
    }

    //Testing CSRList's averageSparseRatio function under all cases.
    if (tester.averageSparseRatioTest()){
        cout << "PASSED: CSRList averageSparseRatio() works correctly on all test cases." << endl;
    } else {
        cout << "FAILED: CSRList averageSparseRatio() works correctly on all test cases." << endl;
    }


    //Testing CSRList's overloaded == operator under all cases.
    if (tester.listEqualityTest()){
        cout << "PASSED: CSRList overloaded equality operator works on all test cases." << endl;
    } else {
        cout << "PASSED: CSRList overloaded equality operator works on all test cases." << endl;
    }

    return 0;
}
