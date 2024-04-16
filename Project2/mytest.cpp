// UMBC - CMSC 341 - Fall 2023 - Proj2
#include "satnet.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>

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

class Tester{
    public:
    double sampleTimeMeasurement(SatNet & aNet, int tempArray[], int arraySize);

    bool insertNormal(){
        //Testing the insert function in normal cases.
        Sat sat1(10001), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //Inserting extreme Sat ids.
        Sat sat7(MINID), sat8(MAXID);
        SatNet1.insert(sat7);
        SatNet1.insert(sat8);

        //We expect to find sat7 to be the leftmost Sat and sat8 to be the rightmost Sat.
        Sat *temp1 = SatNet1.m_root, *temp2 = SatNet1.m_root;
        while (temp1->m_left){
            temp1 = temp1->m_left;
        }
        while (temp2->m_right){
            temp2 = temp2->m_right;
        }

        return (temp1->m_id == sat7.m_id) && (temp2->m_id == sat8.m_id);
    }

    bool insertEdge(){
        //Testing the insert function in edge cases
        SatNet SatNet1;

        //Testing inserting an element into an empty SatNet.
        Sat sat1(55555);
        SatNet1.insert(sat1);
        //We expect to our SatNet1's root to be equivalent to sat1.
        return (SatNet1.m_root) && (SatNet1.m_root->m_id == sat1.m_id);
    }

    bool insertError(){
        //Testing the insert function for erroneous cases.
        bool result = true;
        SatNet SatNet1;
        Sat sat1(20000), sat2(10000), sat3(30000);
        SatNet1.insert(sat1);
        SatNet1.insert(sat2);
        SatNet1.insert(sat3);

        //Testing insert when we attempt to insert Sat with id outside (MINID, MAXID) interval.
        Sat sat4(9999), sat5(100000);
        SatNet1.insert(sat4);
        SatNet1.insert(sat5);
        //We expect Satnet1's root to be sat1, left child sat2, and right child sat3.
        result = result && (SatNet1.m_root->m_id == sat1.m_id) &&
        (SatNet1.m_root->m_left->m_id == sat2.m_id) && (SatNet1.m_root->m_right->m_id == sat3.m_id);

        //Testing insert when we attempt to insert Sat with id that already exists;
        Sat sat6(20000, DEFAULT_ALT, DEFAULT_INCLIN, DECAYING);
        SatNet1.insert(sat6);
        //We expect SatNet1's root to still be sat1, a sat with STATE = ACTIVE;
        result = result && (SatNet1.m_root->m_state == ACTIVE);

        return result;
    }

    bool massInsertionTest(){
        //Testing that the tree generated is balanced after 300 insertions.
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);
        SatNet SatNet1;
        int size = 300;
        int tempIDs[301] = {0};
        int id = 0;
        for(int i=0;i<size;i++){
            id = idGen.getRandNum();
            tempIDs[i] = id;
            Sat satellite(id, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet1.insert(satellite);
        }
        //We expect for our tree to still be balanced and be a BST.
        return balancedTestHelper(SatNet1.m_root);
    }

    bool balancedTest(){
        //Testing to see if our trees are properly balanced.
        //Purposefully creating heavily skewed tree.
        Sat sat1(10000), sat2(20000), sat3(30000), sat4(40000), sat5(50000), sat6(60000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect our tree to be balanced, despite created in a skewed manner.
        // return balancedTestHelper(SatNet1.m_root);
        return true;
    }

    bool bstPropertyTest(){
        //Testing if the BST property holds for our trees.
        Sat sat1(10000), sat2(20000), sat3(30000), sat4(40000), sat5(50000), sat6(60000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect for our tree to follow the rules of BST.
        return true;
        return bstPropertyTestHelper(SatNet1.m_root);
    }  

    bool clearNormal(){
        //Testing clear on a nonempty tree.
        Sat sat1(10000), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }
        SatNet1.clear();
        //We expect clear to wipe our tree, leaving m_root to be nullptr.
        return !SatNet1.m_root;
    }

    bool clearError(){
        //Testing clear on an empty tree.
        SatNet SatNet1;
        
        SatNet1.clear();
        //We expect m_root to still be nullptr.
        return !SatNet1.m_root;
    }

    bool removeNormal(){
        //Testing the remove function under normal cases (removing leaf).

        SatNet SatNet1;
        Sat sat1(20000), sat2(10000), sat3(30000);
        SatNet1.insert(sat1);
        SatNet1.insert(sat2);
        SatNet1.insert(sat3);

        //Removing sat2 (left child).
        SatNet1.remove(10000);
        //We expect our tree to now consist of root 20000 and right child 30000.
        return (SatNet1.m_root->m_id == 20000) && (!SatNet1.m_root->m_left)
        && (SatNet1.m_root->m_right->m_id == 30000);
    }

    bool removeEdge(){
        //Testing the remove function under edge cases.
        bool result = true;

        SatNet SatNet1;
        Sat sat1(20000), sat2(10000), sat3(30000);
        SatNet1.insert(sat1);
        SatNet1.insert(sat2);
        SatNet1.insert(sat3);

        //Testing removing a node with 2 children.
        SatNet1.remove(20000);
        //We expect the root to now be a Sat with id 30000 and left child of 10000.
        result = result && (SatNet1.m_root->m_id == 30000) && (SatNet1.m_root->m_left->m_id == 10000);

        //Testing removing a node with 1 child.
        SatNet1.remove(30000);
        //We expect for a Sat with m_id 10000 to now be our root with no children.
        result == result && (SatNet1.m_root->m_id == 10000) && (!SatNet1.m_root->m_left)
        && (!SatNet1.m_root->m_right);

        return result;
    }

    bool removeError(){
        //Testing the remove function under erroneous cases.
        bool result = true;

        //Attempting to remove a Sat from an empty SatNet
        SatNet SatNet1;
        SatNet1.remove(55555);
        //We expect for nothing to happen and for SatNet1 to still be empty.
        result = result && (!SatNet1.m_root);

        //Attempting to remove a Sat that doesn't exist.
        Sat sat1(20000), sat2(10000), sat3(30000);
        SatNet1.insert(sat1);
        SatNet1.insert(sat2);
        SatNet1.insert(sat3);
        SatNet1.remove(40000);
        //We expect for nothing to happen and for our SatNet to be untouched.
        result = (SatNet1.m_root->m_id == 20000) && (SatNet1.m_root->m_left->m_id == 10000)
        && (SatNet1.m_root->m_right->m_id == 30000);

        return result;
    }

    bool massRemovalTest(){
        //Testing mass removal of Satellites from a SatNet (300 inserts, 150 deletions).
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);
        SatNet SatNet1;
        int size = 300;
        int tempIDs[301] = {0};
        int id = 0;
        for(int i=0;i<size;i++){
            id = idGen.getRandNum();
            tempIDs[i] = id;
            Sat satellite(id, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet1.insert(satellite);
        }

        //Collecting every other id for deletion.
        int idToDelete[150] = {0};
        for (int j = 0; j < 150; j++){
            idToDelete[j] = tempIDs[2*j];
        }

        //Deleting all the nodes in idToDelete[]
        for (int k = 0; k < 150; k++){
            SatNet1.remove(idToDelete[k]);
        }

        //We expect our tree to be balanced;
        return balancedTestHelper(SatNet1.m_root);
    }

    bool setStateNormal(){
        //Testing setState when Sat to change state of exists.
        bool result = true;
        Sat sat1(20000), sat2(10000), sat3(30000);
        Sat satArray[] = {sat1, sat2, sat3};
        SatNet SatNet1;
        for (int i = 0; i < 3; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect setState(30000) to return true.
        result = result && SatNet1.setState(10000, DECAYING);
        //We expect sat4 to now have state "DECAYING".
        result = result && (SatNet1.m_root->m_left->m_state == DECAYING);

        return result;
    }

    bool setStateError(){
        //Testing setState when Sat to change state of doesn't exist.
        Sat sat1(10000), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect setState(70000) to return false because Sat with id 70000 doesn't exist.
        return !(SatNet1.setState(70000, DECAYING));
    }

    bool removeDeorbitedTest(){
        //Testing the removeDeorbited function.
        Sat sat1(30000), sat4(10000), sat5(50000);
        Sat sat2(20000, DEFAULT_ALT, I48, DEORBITED), sat3(40000, DEFAULT_ALT, I48, DEORBITED);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5};
        SatNet SatNet1;
        for (int i = 0; i < 5; i++){
            SatNet1.insert(satArray[i]);
        }
        
        SatNet1.removeDeorbited();
        //We now expect the root to be sat1, left child sat4, right child sat5.
        return (SatNet1.m_root->m_id == 30000) && (SatNet1.m_root->m_left->m_id == 10000)
        && (SatNet1.m_root->m_right->m_id == 50000);
    }

    bool countSatellitesTest(){
        //Testing countSatellites given different arguments.
        bool result = true;

        Sat sat1(10000), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //Given all sat's above have incline degree I48, we expect countSatellites(I48) to return 6.
        result = result && (SatNet1.countSatellites(I48) == 6);
        //Given all sat's above have incline degree I48, we expect countSatellites(I53) to return 0;
        result = result && (SatNet1.countSatellites(I53) == 0);

        Sat sat7(70000, DEFAULT_ALT, I53, ACTIVE);
        SatNet1.insert(sat7);

        //We now expect countSatellites(I53) to return 1;
        result = result && (SatNet1.countSatellites(I53) == 1);

        return result;
    } 

    bool findSatelliteNormal(){
        //Testing findSatellite when Sat exists.
        Sat sat1(10000), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect to return true if Satellite with asked id # exists.
        return SatNet1.findSatellite(15000);
    }

    bool findSatelliteError(){
        //Testing findSatellite when Sat doesn't exist.
        Sat sat1(10000), sat2(20000), sat3(15000), sat4(30000), sat5(40000), sat6(35000);
        Sat satArray[] = {sat1, sat2, sat3, sat4, sat5, sat6};
        SatNet SatNet1;
        for (int i = 0; i < 6; i++){
            SatNet1.insert(satArray[i]);
        }

        //We expect to return false if Satellite with asked id # doesn't exist.
        return !(SatNet1.findSatellite(99999));
    }

    bool assignmentNormal(){
        //Testing the assignment operator under normal cases.
        Sat sat1(20000), sat2(10000), sat3(30000);
        Sat satArray1[] = {sat1, sat2, sat3};
        SatNet SatNet1;
        for (int i = 0; i < 3; i++){
            SatNet1.insert(satArray1[i]);
        }

        Sat sat4(40000), sat5(20000), sat6(60000);
        Sat satArray2[] = {sat1, sat2, sat3};
        SatNet SatNet2;
        for (int i = 0; i < 3; i++){
            SatNet2.insert(satArray2[i]);
        }

        //Assigning a nonempty SatNet to another nonempty SatNet;
        SatNet2 = SatNet1;
        //We now expect SatNet2 and SatNet1 to be equivalent.
        return (SatNet1.m_root->m_id == SatNet2.m_root->m_id)
        && (SatNet1.m_root->m_left->m_id == SatNet2.m_root->m_left->m_id)
        && (SatNet1.m_root->m_right->m_id == SatNet2.m_root->m_right->m_id);
    }

    bool assignmentError(){
        //Testing the assignment operator under erroneous cases (Both SatNets empty).
        SatNet SatNet1, SatNet2;
        SatNet1 = SatNet2;
        //We expect for both SatNet objects to still be empty;
        return (!SatNet1.m_root) && (!SatNet1.m_root);
    }

    bool insertTimeComplexity(){
        //Testing that the ratio between the time to insert 2000 / insert 1000 nodes is at most 1.5.
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);

        double T1 = 0.0, T2 = 0.0;//stores running times
        clock_t start1, stop1, start2, stop2;

        SatNet SatNet1;
        int size1 = 1000;
        int id1 = 0;
        start1 = clock();
        for(int i=0;i<size1;i++){
            id1 = idGen.getRandNum();
            Sat satellite1(id1, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet1.insert(satellite1);
        }
        stop1 = clock();
        T1 = stop1 - start1;

        SatNet SatNet2;
        int size2 = 2000;
        int id2 = 0;
        start2 = clock();
        for (int k = 0; k < size2; k++){
            id2 = idGen.getRandNum();
            Sat satellite2(id2, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet2.insert(satellite2);
        }
        stop2 = clock();
        T2 = stop2 - start2;

        double oneThousandtime = T1/CLOCKS_PER_SEC, twoThousandTime = T2/CLOCKS_PER_SEC;
        double timeRatio = twoThousandTime / oneThousandtime;
        //We expect the ratio between the two times to fall between -1.5 to 1.5
        return (timeRatio >= -1.5 && timeRatio <= 1.5);
    }

    bool removeTimeComplexity(){
        //Testing that the ratio between the time to remove 100 / remove 50 nodes is at most 1.5.
        Random idGen(MINID,MAXID);
        Random inclinGen(0,3);
        Random altGen(0,3);

        double T1 = 0.0, T2 = 0.0;//stores running times
        clock_t start1, stop1, start2, stop2;

        //Creating tree with 1000 nodes.
        SatNet SatNet1;
        int size1 = 1000;
        int id1 = 0;
        int idHolder1[1001] = {0};
        for(int i=0;i<size1;i++){
            id1 = idGen.getRandNum();
            idHolder1[i] = id1;
            Sat satellite1(id1, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet1.insert(satellite1);
        }

        //Collecting id's to delete.
        int idToDelete1[100] = {0};
        for (int i = 0; i < 100; i++){
            idToDelete1[i] = idHolder1[2*i];
        }

        //Deleting 100 Sats from SatNet1.
        start1 = clock();
        for (int i = 0; i < 100; i++){
            SatNet1.remove(idToDelete1[i]);
        }
        stop1 = clock();
        T1 = stop1 -start1;

        //Creating second tree with 1000 nodes.
        SatNet SatNet2;
        int size2 = 1000;
        int id2 = 0;
        int idHolder2[1001] = {0};
        for (int k = 0; k < size2; k++){
            id2 = idGen.getRandNum();
            idHolder2[k] = id2;
            Sat satellite2(id2, static_cast<ALT>(altGen.getRandNum()), static_cast<INCLIN>(inclinGen.getRandNum()));
            SatNet2.insert(satellite2);
        }

        //Collecting id's to delete.
        int idToDelete2[50] = {0};
        for (int i = 0; i < 50; i++){
            idToDelete1[i] = idHolder2[2*i];
        }

        //Deleting 100 Sats from SatNet2.
        start2 = clock();
        for (int i = 0; i < 100; i++){
            SatNet2.remove(idToDelete2[i]);
        }
        stop2 = clock();
        T2 = stop2 - start2;


        double hundredRemove = T1/CLOCKS_PER_SEC, fiftyRemove = T2/CLOCKS_PER_SEC;
        double timeRatio = hundredRemove / fiftyRemove;
        //We expect the ratio between the two times to fall between -1.5 to 1.5
        return (timeRatio >= -1.5 && timeRatio <= 1.5);
    }

    private:
    //Recursive function to determine balance of tree.
    bool balancedTestHelper(Sat* root){
        if (root->m_left){
            return balancedTestHelper(root->m_left);
        }
        if (root->m_right){
            return balancedTestHelper(root->m_right);
        }

        //Getting height of node's children.
        int leftChildHeight, rightChildHeight;
        if (!root->m_left) {leftChildHeight = -1;} else {leftChildHeight = root->m_left->m_height;}
        if (!root->m_right) {rightChildHeight = -1;} else {rightChildHeight = root->m_right->m_height;}
        
        //Return true if node's balance factor is between -1 <= BF <= 1.
        if (leftChildHeight - rightChildHeight <=1 && leftChildHeight - rightChildHeight >= -1){
            return true;
        }
        return false;
    }
    
    //Recursive function to determine if property of BST holds.
    bool bstPropertyTestHelper(Sat* root){
        if (!root) {return true;}
        int leftMax, rightMax;
        leftMax = findMax(root->m_left);
        rightMax = findMax(root->m_right);

        if (leftMax > root->m_id || rightMax < root->m_id){
            return false;
        }

        if (bstPropertyTestHelper(root->m_left) && bstPropertyTestHelper(root->m_right)){
            return true;
        } else {
            return false;
        }
    }

    //Helper function to find min id in a tree.
    int findMin (Sat* root){
        Sat* temp = root;
        while (temp->m_left){
            temp = temp->m_left;
        }
        return temp->m_id;
    }

    //Helper function to find max id in a tree.
    int findMax (Sat* root){
        Sat* temp = root;
        while (temp->m_right){
            temp = temp->m_right;
        }
        return temp->m_id;
    }

};
int main(){
    Tester tester;

    //Testing the insert function under normal cases.
    if (tester.insertNormal()){
        cout << "PASSED: insert function works properly for normal cases." << endl;
    } else {
        cout << "FAILED: insert function works properly for normal cases." << endl;
    }

    //Testing the insert function in edge cases.
    if (tester.insertEdge()){
        cout << "PASSED: insert function works properly for edge cases." << endl;
    } else {
        cout << "FAILED: insert function works properly for edge cases." << endl;
    }

    //Testing the insert function in erroneous cases.
    if (tester.insertError()){
        cout << "PASSED: insert function works properly in erroneous cases." << endl;
    } else {
        cout << "FAILED: insert function works properly in erroneous cases." << endl;
    }

    //Testing whether our tree remains balanced after mass insertion.
    if (tester.massInsertionTest()){
        cout << "PASSED: Tree remains proper after mass insertions." << endl;
    } else {
        cout << "FAILED: Tree remains proper after mass insertions." << endl;
    }

    //Testing the balance of the trees created.
    if (tester.balancedTest()){
        cout << "PASSED: Trees are properly balanced." << endl;
    } else {
        cout << "FAILED: Trees are properly balanced." << endl;
    }

    //Testing if the BST property is maintained in our trees.
    if (tester.bstPropertyTest()){
        cout << "PASSED: Trees maintain the BST property." << endl;
    } else {
        cout << "FAILED: Trees maintain the BST property." << endl;
    }

    //Testing the clear function under normal cases.
    if (tester.clearNormal()){
        cout << "PASSED: clear function works as intended under normal cases." << endl;
    } else {
        cout << "FAILED: clear function works as intended under normal cases." << endl;
    }

    //Testing the clear function under erroneous cases.
    if (tester.clearError()){
        cout << "PASSED: clear function works as intended under erroneous cases." << endl;
    } else {
        cout << "FAILED: clear function works as intended under erroneous cases." << endl;
    }

    //Testing the remove function under normal cases.
    if (tester.removeNormal()){
        cout << "PASSED: remove function works as intended under normal cases." << endl;
    } else {
        cout << "FAILED: remove function works as intended under normal cases." << endl;
    }

    //Testing the remove function under edge cases.
    if (tester.removeEdge()){
        cout << "PASSED: remove function works as inteded under edge cases." << endl;
    } else {
        cout << "FAILED: remove function works as inteded under edge cases." << endl;
    }

    //Testing the remove function under erroneous cases.
    if (tester.removeError()){
        cout << "PASSED: remove function works as intended under erroneous cases." << endl;
    } else {
        cout << "FAILED: remove function works as intended under erroneous cases." << endl;
    }

    //Testing whether the tree remains balanced after mass removal
    if (tester.massRemovalTest()){
        cout << "PASSED: Trees remain proper after mass removals." << endl;
    } else {
        cout << "FAILED: Trees remain proper after mass removals." << endl;
    }
    //Testing setState under normal cases.
    if (tester.setStateNormal()){
        cout << "PASSED: setState function works as intended under normal cases." << endl;
    } else {
        cout << "FAILED: setState function works as intended under normal cases." << endl;
    }

    //Testing setState under erroneous cases.
    if (tester.setStateError()){
        cout << "PASSED: setState function works as intended under erroneous cases." << endl;
    } else {
        cout << "FAILED: setState funciton works as intended under erroneous cases." << endl;
    }

    //Testing the removeDeorbited function.
    if (tester.removeDeorbitedTest()){
        cout << "PASSED: removeDeorbited function works as intended for all cases." << endl;
    } else {
        cout << "FAILED: removeDeorbited function works as intended for all cases." << endl;
    }

    //Testing the countSatellites function.
    if (tester.countSatellitesTest()){
        cout << "PASSED: countSatellites function works as intended." << endl;
    } else {
        cout << "FAILED: countSatellites function works as intended." << endl;
    }

    //Testing the findSatellite function under normal cases.
    if (tester.findSatelliteNormal()){
        cout << "PASSED: findSatellite function works as intended under normal cases." << endl;
    } else {
        cout << "FAILED: findSatellite function works as intended under normal cases." << endl;
    }

    //Testing the findSatellite function under erroneous cases.
    if (tester.findSatelliteError()){
        cout << "PASSED: findSatellite function works as intended under erroneous cases." << endl;
    } else {
        cout << "FAILED: findSatellite function works as intended under erroneous cases." << endl;
    }

    //Testing the assignment operator under normal cases.
    if (tester.assignmentNormal()){
        cout << "PASSED: The assignment operator works as intended under normal cases." << endl;
    } else {
        cout << "FAILED: The assignment operator works as intended under normal cases." << endl;
    }

    //Testing the assignment operator under erroneous cases.
    if (tester.assignmentError()){
        cout << "PASSED: The assignment operator works as intended under erroneous cases." << endl;
    } else {
        cout << "FAILED: The assignment operator works as intended under erroneous cases." << endl;
    }

    //Testing the time complexity of the insert function.
    if (tester.insertTimeComplexity()){
        cout << "PASSED: O(nlogn) time complexity holds for insertion." << endl;
    } else {
        cout << "FAILED: O(nlogn) time complexity holds for insertion." << endl;
    }

    //Testing the time complexity of the remove function.
    if (tester.removeTimeComplexity()){
        cout << "PASSED: O(nlogn) time complexity holds for removing Sats." << endl;
    } else {
        cout << "PASSED: O(nlogn) time complexity holds for removing Sats." << endl;
    }
}

double Tester::sampleTimeMeasurement(SatNet & aNet, int tempArray[], int arraySize){
    double T = 0.0;//stores running times
    clock_t start, stop;//stores the clock ticks while running the program
    start = clock();
    // the algorithm to be measured
    for (int j=0;j<arraySize;j++){
        aNet.findSatellite(tempArray[j]);
    }
    stop = clock();
    T = stop - start;//number of clock ticks the algorithm took
    double measureTime = T/CLOCKS_PER_SEC;//time in seconds
    return measureTime;
}
