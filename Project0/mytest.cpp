// UMBC - CMSC 341 - Fall 2023 - Proj0
// Giovanni Birriel Gomez
#include "bingo.h"
class Tester{
    public:
    // This function is a sample test function
    // It shows how to write a test case 
    bool assignmentNormal(Bingo & lhs, Bingo & rhs){
        // we expect that lhs object is an exact copy of rhs object 
        bool result = true;
        // we expect that the corresponding cells in lhs and rhs
        //      cards carry the same cell information (exact same copy)
        for (int i=0;i<rhs.m_numRows;i++){
            for (int j=0;j<rhs.m_numCols;j++){
                result = result && (lhs.m_card[i][j] == rhs.m_card[i][j]);
            }
        }
        // we expect that the corresponding cells in lhs and rhs
        //      m_helper carry the same cell information (exact same copy)
        for (int i=0;i<rhs.m_helperSize;i++){
            result = result && (lhs.m_helper[i] == rhs.m_helper[i]);
        }
        // we expect that the corresponding cells in lhs and rhs
        //      m_trackRows carry the same cell information (exact same copy)
        for (int i=0;i<rhs.m_numRows;i++){
            result = result && (lhs.m_trackRows[i] == rhs.m_trackRows[i]);
        }
        // we expect that the corresponding cells in lhs and rhs
        //      m_trackCols carry the same cell information (exact same copy)
        for (int i=0;i<rhs.m_numCols;i++){
            result = result && (lhs.m_trackCols[i] == rhs.m_trackCols[i]);
        }
        result = result && (lhs.m_minBallVal == rhs.m_minBallVal);
        result = result && (lhs.m_maxBallVal == rhs.m_maxBallVal);
        return result;
    }

    bool assignmentEdge(){
        bool result = true;
        // Case: We assign a default object to an object that has memory allocated (non-default obj)
        // and assign a non-default object to a default object.

        // Assigning an empty object to an object with memory allocated.
        Bingo testObj1(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        Bingo testObj2;
        testObj1.initCard();
        testObj1 = testObj2;
        // We expect testObj1 to now be a default object
        result = result && isDefaultObject(testObj1);

        // Assigning a non-empty object to an empty object.
        Bingo testObj3;
        Bingo testObj4(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        testObj4.initCard();
        testObj3 = testObj4;
        // We expect testObj3 to now be equal to testObj4.
        result = result && !isDefaultObject(testObj3);

        return result;
    }

    bool constructorNormal(){
        // Case: We attempt to construct a Bingo object with legal params.

        Bingo testObj(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        // We expect that the object's main variables are set to what was passed in.
        // We also expect m_card to not be null, thus memory has been allocated.
        if (isCorrectlySet(testObj, CARDROWS, CARDCOLS, MINVAL, MAXVAL)){
            return true;
        }
        return false;
    }

    bool constructorEdge(){
        bool result = true;
        // Case: We attempt to construct Bingo objects with edge case legal params.

        Bingo testObj1(2, 5, MINVAL, MAXVAL);
        // We expect that the object successfully constructs at the
        // lower bounds of number of rows and columns allowed.
        result = result && isCorrectlySet(testObj1, 2, 5, MINVAL, MAXVAL);

        Bingo testObj2(12, 5, MINVAL, MAXVAL);
        // We expect that the object successfully constructs at the
        // upper bounds of number of rows and columns allowed.
        result = result && isCorrectlySet(testObj2, 12, 5, MINVAL, MAXVAL);

        Bingo testObj3(5, 5, 11, 35);
        // We expect that the object successfully constructs when the
        // number of cells on the card is equal to the range of numbers to be pulled.
        result = result && isCorrectlySet(testObj3, 5, 5, 11, 35);

        return result;
    }

    bool constructorError(){
        bool result = true;
        // Case: We attempt to construct Bingo objects with erroneous params.

        Bingo testObj1(-5, -10, MINVAL, MAXVAL);
        // We expect that the object is a default object because
        // row and column numbers are invalid.
        result = result && isDefaultObject(testObj1);

        Bingo testObj2(CARDROWS, CARDCOLS, -5, -10);
        // We expect that the object is a default object because
        // the min and max ranges are invalid
        result = result && isDefaultObject(testObj2);

        Bingo testObj3(10, 5, 15, 20);
        // We expect that the object is a default object because there are more
        // cells than the range of numbers possible, thus they cannot all be filled.
        result = result && isDefaultObject(testObj3);

        Bingo testObj4(5, 5, 11, 36);
        // We expect that the object is a default object because the range of
        // numbers that can be pulled is not divisible by 5.
        result = result && isDefaultObject(testObj4);

        return result;
    }

    bool recreateNormal(){
        bool result = true;
        // Case: We attempt to call reCreateCard(...) on Bingo objects using legal params.

        // Testing reCreateCard(...) on non-default Bingo object.
        Bingo testObj1(5, 5, MINVAL, MAXVAL);
        testObj1.reCreateCard(10, 5, MINVAL, MAXVAL);
        // We expect testObj1 to now be an object with its member variables set to the
        // params passed into the reCreateCard function call.
        result = result && isCorrectlySet(testObj1, 10, 5, MINVAL, MAXVAL);

        // Testing reCreateCard(...) on default Bingo object.
        Bingo testObj2;
        testObj2.reCreateCard(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        // We expect testObj2 to now be an object with its member variables set to the
        // params passed into the reCreateCard function call.
        result = result && isCorrectlySet(testObj2, CARDROWS, CARDCOLS, MINVAL, MAXVAL);

        return result;
    }
    
    bool recreateError(){
        bool result = true;

        // Testing reCreateCard(...) on non-default Bingo object using erroneous params.
        Bingo testObj1(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        testObj1.reCreateCard(-5, -5, MINVAL, MAXVAL);
        // We expect testObj1 to still have the values it was initially constructed with.
        result = result && isCorrectlySet(testObj1, CARDROWS, CARDCOLS, MINVAL, MAXVAL);

        //Testing reCreateCard(...) on default Bingo object using erroneous params.
        Bingo testObj2;
        testObj2.reCreateCard(-5, -5, MINVAL, MAXVAL);
        // We expect testObj2 to still be a default object.
        result = result && isDefaultObject(testObj2);

        return result;
    }

    // check every columns
    bool initCardNormal(){
        bool result = true;

        // Testing initCard() on a non-default Bingo object.
        Bingo testObj1(10, 5, MINVAL, MAXVAL);
        testObj1.initCard();
        // We expect the object to be populated and have helper member functions set.
        result = result && (testObj1.m_helper && testObj1.m_trackCols && testObj1.m_trackRows);

        // We expect the values placed into the objects column fall in the correct range.
        int valsPerCol = (testObj1.m_maxBallVal - testObj1.m_minBallVal + 1) / testObj1.m_numCols;
        int toBeEntered[testObj1.m_maxBallVal - testObj1.m_minBallVal + 1];
        for (int i = 0; i < testObj1.m_numCols; i++){
            int min = testObj1.m_minBallVal + (valsPerCol * i);
            int max = (testObj1.m_minBallVal + (valsPerCol * (i + 1))) - 1;
            for (int j = 0; j < testObj1.m_numRows; j++){
                // We expect the m_card[j][i] cell value to be between min and max
                result = result && testObj1.m_card[j][i].getVal() >= min
                && testObj1.m_card[j][i].getVal() <= max;
            }
        }

        return result;
    }

    bool initCardError(){
        bool result = true;
        // Case: We attempt to call initCard() on a default Bingo object

        // Testing initCard() on a default Bingo object.
        Bingo testObj1;
        testObj1.initCard();
        // We expect the object to remain a default object with no memory allocated.
        result = result && isDefaultObject(testObj1);

        return result;
    }

    bool playNormal(){
        bool result = true;
        // Case: Testing play(...) on a non-default Bingo object that has its cells populated.

        Bingo testObj1(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        testObj1.initCard();
        // We expect play(...) to return a positive integer.
        result = result && testObj1.play(BALLS, testObj1.drawBalls()) > 0;

        return result;
    }

    bool playError(){
        bool result = true;
        // Case: We attempt to call play on a default Bingo object or a Bingo object
        // that has not been filled in with values yet.

        // Testing play(...) on a default object.
        Bingo testObj1;
        // We expect play(...) to return 0;
        result = result && testObj1.play(BALLS, testObj1.drawBalls()) == 0;

        // Testing play(...) on non-default object that hasn't been initalized yet.
        Bingo testObj2(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        // We expect play(...) to return 0;
        result = result && testObj2.play(BALLS, testObj2.drawBalls()) == 0;

        return result;
    }

    bool clearNormal(){
        bool result = true;
        // Case: We attempt to call clear on non-default, initalized Bingo object.

        // Testing clear() on an initialized object.
        Bingo testObj1(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        testObj1.initCard();
        testObj1.clear();
        // We expect testObj1 to now be a default object set to the default values.
        result = result + isDefaultObject(testObj1);

        return result;
    }

    bool clearEdge(){
        bool result = true;
        // Case: We attempt to call clear on non-default, but not initalized object, and a default object.

        // Testing clear() on a non-default, not initialized object.
        Bingo testObj1(CARDROWS, CARDCOLS, MINVAL, MAXVAL);
        testObj1.clear();
        // We expect testObj1 to now be a default object set to the default values.
        result = result && isDefaultObject(testObj1);

        //Testing clear() on a default object.
        Bingo testObj2;
        testObj2.clear();
        // We expect testObj2 to remain a default object.
        result = result && isDefaultObject(testObj2);

        return result;
    }

    private:
    // This function takes in a Bingo object and checks whether or not it is a default object.
    bool isDefaultObject(const Bingo & obj){
        // This if statement checks if all the variables of the object are set to the
        // default values. If so, then obj is a default object and function returns true.
        if (obj.m_numRows == 0 && obj.m_numCols == 0 && obj.m_minBallVal == 0
        && obj.m_maxBallVal == 0 && obj.m_card == nullptr){
            return true;
        }
        return false;
    }

    // This function takes in a Bingo object and the params the Bingo object was created
    // with and checks if the member variables have been properly set.
    bool isCorrectlySet(const Bingo & obj, int row, int col, int min, int max){
        if (obj.m_numRows == row && obj.m_numCols == col && obj.m_minBallVal == min
        && obj.m_maxBallVal == max && obj.m_card){
            return true;
        }
        return false;
    }
};


int main(){
    Tester tester;
    Bingo obj1(CARDROWS,CARDCOLS,MINVAL,MAXVAL);
    obj1.initCard();
    Bingo obj2;
    obj2 = obj1;

    // Testing to see whether the assignment operator normal case works correctly
    if (tester.assignmentNormal(obj2,obj1))
        cout << "PASSED: Assignment operator test for normal cases." << endl;
    else
        cout << "FAILED: Assignment operator test for normal cases." << endl;

    // Testing to see if edge assignments works correctly.
    if (tester.assignmentEdge()){
        cout << "PASSED: Using assignment operator at edge cases." << endl;
    } else {
        cout << "FAILED: Using assignment operator at edge cases." << endl;
    }

    // Testing to see if normal construction params result in a non-default object.
    if (tester.constructorNormal()){
        cout << "PASSED: Constructing Bingo objects under normal arguments." << endl;
    } else{
        cout << "FAILED: Constructing Bingo objects under normal arguments." << endl;
    }

    // Testing to see if multiple legal edge case params result in a non-default object.
    if (tester.constructorEdge()){
        cout << "PASSED: Constructing Bingo objects under edge case arguments." << endl;
    } else {
        cout << "FAILED: Constructing Bingo objects under edge case arguments." << endl;
    }

    // Testing to see if erroneous params result in a default object.
    if (tester.constructorError()){
        cout << "PASSED: Constructing Bingo objects under erroneous arguments." << endl;
    } else {
        cout << "FAILED: Constructing Bingo objects under erroneous arguments." << endl;
    }

    // Testing whether reCreateCard() successfully refactors non-default objects.
    if (tester.recreateNormal()){
        cout << "PASSED: Using reCreateCard under normal arguments." << endl;
    } else {
        cout << "FAILED: Using reCreateCard under normal arguments." << endl;
    }

    // Testing whether reCreateCard() does not alter an object if erroneous params are passed in.
    if (tester.recreateError()){
        cout << "PASSED: Using reCreateCard under erroneous arguments." << endl;
    } else {
        cout << "FAILED: Using reCreateCard under erroneous arguments." << endl;
    }

    // Testing initCard() to see if it successfully fills in card with correct ranges and
    // sets up helper member variables.
    if (tester.initCardNormal()){
        cout << "PASSED: Using initCard under normal conditions (obj not default)." << endl;
    } else {
        cout << "FAILED: Using initCard under normal conditions (obj not default)." << endl;
    }

    // Testing whether initCard() does not alter an object if the calling object is a default object.
    if (tester.initCardError()){
        cout << "PASSED: Using initCard under erroneous conditions (default obj)." << endl;
    } else {
        cout << "FAILED: Using initCard under erroneous conditions (default obj)." << endl;
    }

    // Testing play() on non-default Bingo objects that have been initalized.
    if (tester.playNormal()){
        cout << "PASSED: Using play under normal conditions (obj not default and initialized)." << endl;
    } else {
        cout << "FAILED: Using play under normal conditions (obj not default and initialized)." << endl;
    }

    // Testing play() on default objects and objects that have not been initalized.
    if (tester.playError()){
        cout << "PASSED: Using play under erroneous conditions (obj is default or not initialized)." << endl;
    } else {
        cout << "FAILED: Using play under erroneous conditions (obj is default or not initialized)." << endl;
    }

    // Testing clear() on non-default, initalized objects.
    if (tester.clearNormal()){
        cout << "PASSED: Using clear under normal conditions (non-default, initalized obj)." << endl;
    } else {
        cout << "FAILED: Using clear under normal conditions (non-default, initalized obj)." << endl;
    }

    // Testing clear() on non-default, but not initalized object, and a default object.
    if (tester.clearEdge()){
        cout << "PASSED: Using clear under edge conditions (non-initialized, non-default obj, and default obj)" << endl;
    } else {
        cout << "FAILED: Using clear under edge conditions (non-initialized, non-default obj, and default obj)" << endl;
    }
    return 0;
}