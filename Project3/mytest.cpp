// CMSC 341 - Fall 2023 - Project 3

#include "pqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

// Priority functions compute an integer priority for a patient.  Internal
// computations may be floating point, but must return an integer.

int priorityFn1(const Patient & patient);
int priorityFn2(const Patient & patient);

// a name database for testing purposes
const int NUMNAMES = 20;
string nameDB[NUMNAMES] = {
    "Ismail Carter", "Lorraine Peters", "Marco Shaffer", "Rebecca Moss",
    "Lachlan Solomon", "Grace Mclaughlin", "Tyrese Pruitt", "Aiza Green", 
    "Addie Greer", "Tatiana Buckley", "Tyler Dunn", "Aliyah Strong", 
    "Alastair Connolly", "Beatrix Acosta", "Camilla Mayo", "Fletcher Beck",
    "Erika Drake", "Libby Russo", "Liam Taylor", "Sofia Stewart"
};

// We can use the Random class to generate the test data randomly!
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
    bool copyConstructorNormal(){
        //This tests whether the copy constructor works under normal cases.

        //Testing the copy constructor on a leftist heap.
        PQueue heap1(priorityFn2, MINHEAP, LEFTIST);
        generator(10, heap1);
        PQueue heap2(heap1);
        //We expect heap2 to be identical to heap1.
        return testEquality(heap1, heap2);
    }

    bool copyConstructorEdge(){
        //This tests whether the copy constructor works under edge cases.
        bool result = true;

        //Testing the copy constructor using an empty heap as the argument.
        PQueue heap1(priorityFn1, MAXHEAP, LEFTIST);
        PQueue heap2(heap1);
        //We expect heap2 to be identical to heap1 and with no elements.
        result = (!heap1.m_heap && !heap2.m_heap) && (heap1.m_priorFunc == heap2.m_priorFunc)
        && (heap1.m_heapType == heap2.m_heapType) && (heap1.m_structure == heap2.m_structure);

        return result;
    }

    bool assignOperatorNormal(){
        //Testing if the overloaded assignment operator works under normal cases.

        //Testing assigning one nonempty queue to another nonempty queue.
        PQueue heap1(priorityFn2, MINHEAP, LEFTIST);
        PQueue heap2(priorityFn1, MAXHEAP, SKEW);
        generator(10, heap1);
        generator(10, heap2);
        heap2 = heap1;
        //We expect heap2 to be identical to heap1: same size, priorityfn, etc.
        return testEquality(heap1, heap2); //testEquality tests everything.
    }

    bool assignOperatorEdge(){
        //Testing if the overloaded assignment operator works under edge cases.
        bool result = true;

        //Testing assigning an empty queue to a nonempty queue.
        PQueue heap1(priorityFn1, MAXHEAP, LEFTIST);
        PQueue heap2(priorityFn2, MINHEAP, LEFTIST);
        generator(10, heap1);
        heap1 = heap2;
        //We expect heap1 to be an empty heap and to contain the same format as heap2.
        result = result && testEquality(heap1, heap2) && (!heap1.m_heap);

        //Testing assigning a nonempty queue to an empty queue.
        PQueue heap3 (priorityFn1, MAXHEAP, SKEW);
        PQueue heap4 (priorityFn2, MINHEAP, LEFTIST);
        generator(10, heap3);
        heap4 = heap3;
        //We expect heap4 to be a nonempty heap and to be identical to heap3.
        result = result && testEquality(heap4, heap3);

        //Testing assigning an empty queue to another empty queue;
        PQueue heap5(priorityFn1, MINHEAP, SKEW);
        PQueue heap6(priorityFn1, MINHEAP, LEFTIST);
        heap5 = heap6;
        //We expect both heaps to remain empty but heap5 contain the format of heap6.
        result = result && testEquality(heap5, heap6);

        return result;
    }

    bool assignOperatorError(){
        //Testing the assignment operator in edge cases (self-assignment).
        PQueue heap1(priorityFn2, MINHEAP, SKEW);
        generator(5, heap1);
        heap1 = heap1;
        //If we didn't catch self-assignment, then we would attempt to copy data that has been wiped.
        //So, we just have to test whether or not the strucutre still exists.
        return (heap1.m_heap);
    }

    bool clearTest(){
        //Testing the clear function under all conditions.
        bool result = true;

        //Testing clear on a nonempty PQueue object.
        PQueue heap1(priorityFn1, MAXHEAP, LEFTIST);
        generator(10, heap1);
        heap1.clear();
        //We expect heap1 to now be an empty heap and m_size = 0.
        result = (!heap1.m_heap) && (heap1.m_size == 0);

        //Testing clear on an empty PQueue object.
        PQueue heap2(priorityFn2, MINHEAP, SKEW);
        heap2.clear();
        //We expect heap2 to remain an empty obj with m_heap == nullptr and m_size = 0.
        result = result && (!heap2.m_heap) && (heap2.m_size == 0);

        return result;
    }

    bool dequeueNormal(){
        //Testing getNextPatient under normal cases.
        bool result = true;

        //Testing on leftist heap.
        PQueue queue1(priorityFn1, MAXHEAP, LEFTIST);
        generator(5, queue1);
        queue1.getNextPatient();
        //We expect queue1 to have 4 elements.
        result = result && (queue1.m_size == 4);

        //Testing on skew heap.
        PQueue queue2(priorityFn1, MAXHEAP, SKEW);
        generator(5, queue2);
        queue2.getNextPatient();
        //We expect queue2 to have 4 elements.
        result = result && (queue2.m_size == 4);
        
        return result;
    }

    bool dequeueEdge(){
        //Testing getNextPatient when a heap only contains 1 patient.

        PQueue queue1(priorityFn2, MINHEAP, LEFTIST);
        generator(1, queue1);
        queue1.getNextPatient();
        //We expect queue1 to have m_size 0 and for m_heap to point to nullptr.
        return (!queue1.m_heap) && (queue1.m_size == 0);
    }

    bool dequeueError(){
        //Testing whether the getNextPatient function works under erroneous cases.
        bool result = false;

        //Calling getNextPatient on an empty queue.
        PQueue queue(priorityFn1, MAXHEAP, LEFTIST);
        try{
            queue.getNextPatient();
        }
        catch (const std::out_of_range &e){
            result = true;
        }
        //We expect an out_of_range exception to be thrown.
        return result;
    }

    bool mergeQueuesNormal(){
        //Testing the mergeWithQueue function under normal cases.

        //Testing merging two nonempty leftist queues together.
        PQueue queue1(priorityFn2, MINHEAP, LEFTIST);
        PQueue queue2(priorityFn2, MINHEAP, LEFTIST);
        generator(10, queue1);
        generator(5, queue2);
        queue1.mergeWithQueue(queue2);
        //We expect queue1 to be a leftist heap with m_size 15, and queue2 to be empty;
        return (isLeftist(queue1.m_heap)) && (queue1.m_size == 15) && (!queue2.m_heap);
    }

    bool mergeQueuesEdge(){
        //Testing the mergeWithQueue function under edge cases.
        bool result = true;

        //Testing merging a nonempty queue into an empty queue.
        PQueue queue1(priorityFn2, MINHEAP, SKEW);
        PQueue queue2(priorityFn2, MINHEAP, SKEW);
        generator(10, queue1);
        queue2.mergeWithQueue(queue1);
        //We expect for queue2 to now contain 10 elements and queue1 to be empty.
        result = result && (queue2.m_size == 10) && (!queue1.m_heap);

        //Testing merging an empty queue into a nonempty queue.
        PQueue queue3(priorityFn1, MAXHEAP, LEFTIST);
        PQueue queue4(priorityFn1, MAXHEAP, LEFTIST);
        generator(10, queue3);
        queue3.mergeWithQueue(queue4);
        //We expect queue3 and queue4 to remain untouched.
        result = result && (queue3.m_heap) && (queue3.m_size == 10) && (!queue4.m_heap);

        //Testing merging an empty queue into an empty queue.
        PQueue queue5(priorityFn1, MAXHEAP, SKEW);
        PQueue queue6(priorityFn1, MAXHEAP, SKEW);
        queue5.mergeWithQueue(queue6);
        //We expect queue5 and queue6 to remain empty.
        result = result && (!queue5.m_heap) && (!queue6.m_heap);

        return result;
    }

    bool mergeQueuesError(){
        //Testing the mergeWithQueue function under erroneous cases (mismatched queues).
        PQueue queue1(priorityFn2, MINHEAP, LEFTIST);
        PQueue queue2(priorityFn1, MAXHEAP, SKEW);
        generator(10, queue1);
        generator (10, queue2);
        
        bool result = false;
        //We expect to throw an error if we try to merge these two queues together.
        try{
            queue1.mergeWithQueue(queue2);
        }
        catch (const std::domain_error &e){
            result = true;
        }

        return result;
    }

    bool setStructureNormal(){
        //Testing the setStructure function for both leftist and skew.
        bool result = true;

        //Converting a skew heap into a leftist heap.
        PQueue heap1 (priorityFn2, MINHEAP, SKEW);
        generator(10, heap1);
        heap1.setStructure(LEFTIST);
        //We expect heap1 to now reperesent a leftist heap and still a MINHEAP.
        result = result && isLeftist(heap1.m_heap) && isMinHeap(heap1.m_heap);
        return result;
    }

    bool setStructureEdge(){
        //Testing the setStructure function under edge cases (empty heaps).
        bool result = true;

        //Testing converting LEFTIST to SKEW.
        PQueue heap1(priorityFn2, MINHEAP, LEFTIST);
        heap1.setStructure(SKEW);
        //We expect heap1 to remain empty but now have the SKEW structure.
        result = result && (heap1.m_structure == SKEW) && (!heap1.m_heap);

        //Testing converting SKEW to LEFTIST.
        PQueue heap2 (priorityFn2, MINHEAP, SKEW);
        heap1.setStructure(LEFTIST);
        //We expect heap2 to remain empty but now have the LEFTIST structure.
        result = result && (heap1.m_structure == LEFTIST) && (!heap2.m_heap);

        return result;
    }

    bool setPriorityFnNormal(){
        //Testing the setPriorityFn function under normal cases.
        bool result = true;

        //Testing setting a minheap queue into a maxheap queue.
        PQueue queue1(priorityFn2, MINHEAP, LEFTIST);
        generator(10, queue1);
        queue1.setPriorityFn(priorityFn1, MAXHEAP);
        //We expect queue1 to now be a MAXHEAP with priorityFn1.
        result = result && (queue1.m_priorFunc == priorityFn1) && (isMaxHeap(queue1.m_heap));

        //Testing setting a maxheap queue into a minheap queue.
        PQueue queue2(priorityFn1, MAXHEAP, SKEW);
        generator(10, queue2);
        queue2.setPriorityFn(priorityFn2, MINHEAP);
        //We expect queue2 to now be a MINHEAP with priorityFn2.
        result = result && (queue2.m_priorFunc == priorityFn2) && (isMinHeap(queue2.m_heap));

        return result;
    }

    bool setPriorityFnEdge(){
        //Testing the setPriorityFn function under edge cases (empty heaps).
        bool result;

        //Testing setting an empty minheap queue into a maxheap queue.
        PQueue queue1(priorityFn2, MINHEAP, SKEW);
        queue1.setPriorityFn(priorityFn1, MAXHEAP);
        //We expect queue1 to now remain an empty heap but with priorityFn1 and be a MAXHEAP.
        result = (queue1.m_priorFunc == priorityFn1) && (!queue1.m_heap) && (queue1.m_heapType == MAXHEAP);

        //Testing setting an empty maxheap queue into a minheap queue.
        PQueue queue2(priorityFn1, MAXHEAP, LEFTIST);
        queue2.setPriorityFn(priorityFn2, MINHEAP);
        //We expect queue2 to now reamin an empty heap but with priorityFn2 and be a MINHEAP.
        result = result && (queue2.m_priorFunc == priorityFn2) && (!queue2.m_heap) && (queue2.m_heapType == MINHEAP);

        return result;
    }


    bool massMaxHeapInsertionTest(){
        //Testing whether a heap maintains its max heap property after 300 insertions.
        bool result = true;

        //Testing on leftist heap.
        PQueue maxHeapQueue(priorityFn1, MAXHEAP, LEFTIST);
        generator(300, maxHeapQueue);
        //We expect maxHeapQueue to be a leftist max heap and have 300 elements
        result = result && isMaxHeap(maxHeapQueue.m_heap) && (maxHeapQueue.m_size == 300)
        && isLeftist(maxHeapQueue.m_heap);

        //Testing on skew heap.
        PQueue maxHeapQueue2(priorityFn1, MAXHEAP, SKEW);
        generator(300, maxHeapQueue2);
        //We expect maxHeapQueue2 to be a skew max heap and have 300 elements.
        result = result && isMaxHeap(maxHeapQueue2.m_heap) && (maxHeapQueue2.m_size == 300);

        return result;
    }

    bool massMaxHeapRemovalTest(){
        //Testing whether a heap maintains its max heap property after 150 removals.
        bool result = true;

        //Testing on leftist heap.
        PQueue maxHeapQueue(priorityFn1, MAXHEAP, LEFTIST);
        generator(300, maxHeapQueue);
        for (int i = 0; i < 150; i++){
            maxHeapQueue.getNextPatient();
        }
        //We expect maxHeapQueue to be a leftist max heap and have 150 elements.
        result = result && isMaxHeap(maxHeapQueue.m_heap) && (maxHeapQueue.m_size == 150)
        && isLeftist(maxHeapQueue.m_heap);

        //Testing on skew heap.
        PQueue maxHeapQueue2(priorityFn1, MAXHEAP, SKEW);
        generator(300, maxHeapQueue2);
        for (int j = 0; j < 150; j++){
            maxHeapQueue2.getNextPatient();
        }
        //We expect maxHeapQueue2 to be a skew max heap and have 150 elements.
        result = result && isMaxHeap(maxHeapQueue2.m_heap) && (maxHeapQueue2.m_size == 150);

        return result;
    }

    bool massMinHeapInsertionTest(){
        //Testing whether a heap maintains its min heap property after 300 insertions.
        bool result = true;

        //Testing on leftist heap.
        PQueue minHeapQueue(priorityFn2, MINHEAP, LEFTIST);
        generator(300, minHeapQueue);
        //We expect minHeapQueue to be a leftist min heap and have 300 elements.
        result = result && isMinHeap(minHeapQueue.m_heap) && (minHeapQueue.m_size == 300)
        && isLeftist(minHeapQueue.m_heap);

        //Testing on skew heap.
        PQueue minHeapQueue2(priorityFn2, MINHEAP, SKEW);
        generator(300, minHeapQueue2);
        //We expect minHeapQueue2 to be a skew min heap and have 300 elements.
        result = result && isMinHeap(minHeapQueue2.m_heap) && (minHeapQueue2.m_size == 300);

        return result;
    }

    bool massMinHeapRemovalTest(){
        //Testing whether a heap maintains its min heap property after 150 removals.
        bool result = true;

        //Testing on a leftist heap.
        PQueue minHeapQueue(priorityFn2, MINHEAP, LEFTIST);
        generator(300, minHeapQueue);
        for (int i = 0; i < 150; i++){
            minHeapQueue.getNextPatient();
        }
        //We expect minHeapQueue to be a leftist min heap and have 150 elements.
        result = result && isMinHeap(minHeapQueue.m_heap) && (minHeapQueue.m_size == 150)
        && isLeftist(minHeapQueue.m_heap);

        //Testing on a skew heap.
        PQueue minHeapQueue2(priorityFn2, MINHEAP, SKEW);
        generator(300, minHeapQueue2);
        for (int j = 0; j < 150; j++){
            minHeapQueue2.getNextPatient();
        }
        //We expect minHeapQueue2 to be a min heap and have 150 elements.
        result = result && isMinHeap(minHeapQueue2.m_heap) && (minHeapQueue2.m_size == 150);

        return result;
    }

    private:

    //Helper function to insert a given # of patients into a given heap.
    void generator(int numsOfPatients, PQueue& heap){
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<numsOfPatients;i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        heap.insertPatient(patient);
        }
    }

    //Helper function that compares whether two heaps are state-wise identical.
    bool testEquality(PQueue& heap1, PQueue& heap2){
        //Return false if heap setups do not match.
        if ((heap1.m_size != heap2.m_size) || (heap1.m_priorFunc != heap2.m_priorFunc)
        || (heap1.m_heapType != heap2.m_heapType)) {
            return false;
        }
        //Calling testNodeEquality to verify the trees of the heaps match.
        return testNodeEquality(heap1.m_heap, heap2.m_heap);
    }

    //Helper function that checks the underlying tree of two heaps and returns whether they're identical or not.
    bool testNodeEquality (Node* root1, Node* root2){
        //Return true if we arrive to nullptr at the same time.
        if (!root1 && !root2) {return true;}
        //Return false if a subtree exists for one heap but not the other.
        if ((!root1 && root2) || (root1 && !root2)) {return false;}

        //Comparing two nodes.
        if (root1->m_patient.m_BP != root2->m_patient.m_BP || root1->m_patient.m_opinion != root2->m_patient.m_opinion
        || root1->m_patient.m_oxygen != root2->m_patient.m_oxygen || root1->m_patient.m_RR != root2->m_patient.m_RR
        || root1->m_patient.m_temperature != root2->m_patient.m_temperature) {return false;}

        bool result = true;
        result = result && (testNodeEquality(root1->m_left, root2->m_left));
        result = result && (testNodeEquality(root1->m_right, root2->m_right));
        return result;
    }

    //Helper function to test if a tree is a leftist heap.
    bool isLeftist (Node* root){
        //Returning true for base cases.
        if (!root) {return true;}
        if (!root->m_left && !root->m_right) {return true;}
        if (root->m_left && !root->m_right) {return true;}

        //If right exists but not left, then not a leftist.
        if (!root->m_left && root->m_right){
            return false;
        }
        //If left npl less than right npl, then not a leftist.
        if (root->m_left->m_npl < root->m_right->m_npl){
            return false;
        }

        //Check left and right subtrees.
        bool result = true;
        result = result && (isLeftist(root->m_left));
        result = result && (isLeftist(root->m_right));
        return result;
    }

    //Helper function to test if a heap is a max heap.
    bool isMaxHeap (Node* root){
        if (!root) {return true;}

        if (root->m_left && root->m_right){ //Checking if both left and right child are smaller than root.
            if (priorityFn1(root->m_left->m_patient) > priorityFn1 (root->m_patient)){
                return false;
            }
            else if (priorityFn1(root->m_right->m_patient) > priorityFn1(root->m_patient)){
                return false;
            }
        }
        if (!root->m_left && root->m_right){ //Case where right exists but not left.
            if (priorityFn1(root->m_right->m_patient) > priorityFn1(root->m_patient)){
                return false;
            }
        }
        if (root->m_left && !root->m_right){ //Case where left exists but not right.
            if (priorityFn1(root->m_left->m_patient) > priorityFn1(root->m_patient)){
                return false;
            }
        }

        //Checking both left and right subtrees.
        return isMaxHeap(root->m_left) && isMaxHeap(root->m_right);
    }

    //Helper function to test is a heap is a min heap.
    bool isMinHeap(Node* root){
        if (!root) {return true;}

        if (root->m_left && root->m_right){
            if (priorityFn2(root->m_left->m_patient) < priorityFn2(root->m_patient)){
                return false;
            }
            else if (priorityFn2(root->m_right->m_patient) < priorityFn2(root->m_patient)){
                return false;
            }
        }
        if (!root->m_left && root->m_right){
            if (priorityFn2(root->m_right->m_patient) < priorityFn2(root->m_patient)){
                return false;
            }
        }
        if (root->m_left && !root->m_right){
            if (priorityFn2(root->m_left->m_patient) < priorityFn2(root->m_patient)){
                return false;
            }
        }

        //Checking both left and right subtrees.
        return isMinHeap(root->m_left) && isMinHeap(root->m_right);
    }
};

int main(){

    Tester tester;
    //Testing the copy constructor under normal cases.
    if (tester.copyConstructorNormal()){
        cout << "PASSED: Copy constructor works under normal cases." << endl;
    } else {
        cout << "FAILED: Copy constructor works under normal cases." << endl;
    }

    //Testing the copy constructor under edge cases.
    if (tester.copyConstructorEdge()){
        cout << "PASSED: Copy constructor works under edge cases." << endl;
    } else {
        cout << "FAILED: Copy constructor works under edge cases." << endl;
    }

    //Testing the assignment operator under normal cases.
    if (tester.assignOperatorNormal()){
        cout << "PASSED: The assignment operator works under normal cases." << endl;
    } else {
        cout << "FAILED: The assignment operator works under normal cases." << endl;
    }

    //Testing the assignment operator under edge cases.
    if (tester.assignOperatorEdge()){
        cout << "PASSED: The assignment operator works under edge cases." << endl;
    } else {
        cout << "FAILED: The assignment operator works under edge cases." << endl;
    }

    //Testing the assignment operator under erroneous cases.
    if (tester.assignOperatorError()){
        cout << "PASSED: The assignment operator works as expected under erroneous cases." << endl;
    } else {
        cout << "FAILED: The assignment operator works as expected under erroneous cases." << endl;
    }

    //Testing the clear function under all cases.
    if (tester.clearTest()){
        cout << "PASSED: Clear function works under all cases." << endl;
    } else {
        cout << "FAILED: Clear functon works under all cases." << endl;
    }

    //Testing whether dequeueing works as expected under normal cases.
    if (tester.dequeueNormal()){
        cout << "PASSED: Dequeueing works under normal cases." << endl;
    } else {
        cout << "FAILED: Dequeueing works under normal cases." << endl;
    }

    //Testing whether dequeueing works as expected under edge cases.
    if (tester.dequeueEdge()){
        cout << "PASSED: Dequeueing works under edge cases." << endl;
    } else {
        cout << "FAILED: Dequeueing works under edge cases." << endl;
    }

    //Testing whether dequeueing works as expected under erroneous cases.
    if (tester.dequeueError()){
        cout << "PASSED: Dequeueing on an empty queue throws an error." << endl;
    } else {
        cout << "FAILED: Dequeueing on an empty queue throws an error." << endl;
    }

    //Testing whether mergeWithQueue function works under normal cases.
    if (tester.mergeQueuesNormal()){
        cout << "PASSED: mergeWithQueue works under normal conditions." << endl;
    } else {
        cout << "FAILED: mergeWithQueue works under normal conditions." << endl;
    }

    //Testing whether mergeWithQueue function works under edge cases.
    if (tester.mergeQueuesEdge()){
        cout << "PASSED: mergeWithQueue works under edge cases." << endl;
    } else {
        cout << "FAILED: mergeWithQueue works under edge cases." << endl;
    }

    //Testing whether mergeWithQueue function works as expected under erroneous cases.
    if (tester.mergeQueuesError()){
        cout << "PASSED: mergeWithQueue throws an error if queues are mismatched." << endl;
    } else {
        cout << "FAILED: mergeWithQueue throws an error if queues are mismatched." << endl;
    }

    //Testing the setStructure function under normal cases.
    if (tester.setStructureNormal()){
        cout << "PASSED: setStructure works as expected under normal cases." << endl;
    } else {
        cout << "FAILED: setStructure works as expected under normal cases" << endl;
    }

    //Testing the setStructure function under edge cases.
    if (tester.setStructureEdge()){
        cout << "PASSED: setStructure works as expected under edge cases." << endl;
    } else {
        cout << "FAILED: setStructure works as expected under edge cases." << endl;
    }

    //Testing the setPriorityFn function under normal cases.
    if (tester.setPriorityFnNormal()){
        cout << "PASSED: setPriorityFn works as expected under normal cases." << endl;
    } else {
        cout << "FAILED: setPriorityFn works as expected under normal cases." << endl;
    }

    //Testing the setPriorityFn function under edge cases.
    if (tester.setPriorityFnEdge()){
        cout << "PASSED: setPriorityFn works as expected under edge cases." << endl;
    } else {
        cout << "FAILED: setPriorityFn works as expected under edge cases." << endl;
    }

    //Testing whether a heap remains a max heap after 300 insertions.
    if (tester.massMaxHeapInsertionTest()){
        cout << "PASSED: Max heap queues retain max heap property after 300 insertions." << endl;
    } else {
        cout << "FAILED: Max heap queues retain max heap property after 300 insertions." << endl;
    }

    //Testing whether a heap remains a max heap after 150 removals.
    if (tester.massMaxHeapRemovalTest()){
        cout << "PASSED: Max heap queues retain max heap property after 150 removals." << endl;
    } else {
        cout << "FAILED: Max heap queues retain max heap property after 150 removals." << endl;
    }

    //Testing whether a heap remains a min heap after 300 insertions.
    if (tester.massMinHeapInsertionTest()){
        cout << "PASSED: Min heap queues retain min heap property after 300 insertions." << endl;
    } else {
        cout << "FAILED: Min heap queues retain min heap property after 300 insertions." << endl;
    }

    //Testing whether a heap remains a min heap after 150 removals.
    if (tester.massMinHeapRemovalTest()){
        cout << "PASSED: Min heap queues retain min heap property after 150 removals." << endl;
    } else {
        cout << "FAILED: Min heap queues retain min heap property after 150 removals." << endl;
    }

    return 0;
}

int priorityFn1(const Patient & patient) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [115-242]
    //temperature + respiratory + blood pressure
    //the highest priority would be 42+40+160 = 242
    //the lowest priority would be 35+10+70 = 115
    //the larger value means the higher priority
    int priority = patient.getTemperature() + patient.getRR() + patient.getBP();
    return priority;
}

int priorityFn2(const Patient & patient) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [71-111]
    //nurse opinion + oxygen
    //the highest priority would be 1+70 = 71
    //the lowest priority would be 10+101 = 111
    //the smaller value means the higher priority
    int priority = patient.getOpinion() + patient.getOxygen();
    return priority;
}