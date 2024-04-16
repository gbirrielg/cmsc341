// CMSC 341 - Fall 2023 - Project 3
#include "pqueue.h"
PQueue::PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}

PQueue::~PQueue() {
    clear();
}

void PQueue::clear() {
    clear(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

void PQueue::clear(Node *root) {
    // Traverse the heap postorder and delete nodes.
    if (!root) {return;}
    clear(root->m_left);
    clear(root->m_right);
    delete root;
}

PQueue::PQueue(const PQueue &rhs) {
    //Copy constructor. Calls copyHeap to make deep copy of rhs.
    m_heap = copyHeap(rhs.m_heap);
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
}

PQueue &PQueue::operator=(const PQueue &rhs) {
    if (this == &rhs) {return *this;} //Preventing self-assignment.
    clear(); //Deallocating memory of lhs to copy rhs.

    //Calls copyHeap to make deep copy of rhs and sets variables.
    m_heap = copyHeap(rhs.m_heap);
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    return *this;
}

Node* PQueue::copyHeap(Node* root){
    if (!root) {return nullptr;} //If rhs is empty, return nullptr.
    
    //Dynamically creating node with same patient info. Also copies npl.
    Node* temp = new Node(root->getPatient());
    temp->m_npl = root->m_npl;

    if (root->m_left){ //Go left and copy
        temp->m_left = copyHeap(root->m_left);
    }
    if (root->m_right){ //Go right and copy
        temp->m_right = copyHeap(root->m_right);
    }

    return temp;
}

void PQueue::mergeWithQueue(PQueue &rhs) {
    if (this == &rhs) {return;} //Preventing self merge.
    //Preventing the merging of mismatched heaps.
    if (m_structure != rhs.m_structure || m_heapType != m_heapType || m_priorFunc != rhs.m_priorFunc){
        throw std::domain_error("ERROR: Mismatched heaps.");
    }

    //Calling merge to merge both heaps.
    m_heap = merge(m_heap, rhs.m_heap);
    m_size += rhs.m_size; //Setting new size.
    rhs.m_heap = nullptr; //Setting root of rhs to nullptr (because it's been merged with lhs).
    rhs.m_size = 0; //Setting rhs size to 0.

}

Node* PQueue::merge(Node* lhs, Node* rhs){
    //This function combines two heaps/subtrees recursively.
    if (!lhs) {return rhs;}
    if (!rhs) {return lhs;}

    Node* temp;

    //Begin by constructing heap based on heapType value.
    if (m_heapType == MINHEAP){ //Case when the heapType is a min heap.
        if (m_priorFunc(lhs->getPatient()) < m_priorFunc(rhs->getPatient())){
            temp = lhs;
            temp->m_right = merge(lhs->m_right, rhs);
        } else {
            temp = rhs;
            temp->m_right = merge(lhs, rhs->m_right);
        }
    }
    else { //Case when the heapType is a max heap.
        if (m_priorFunc(lhs->getPatient()) < m_priorFunc(rhs->getPatient())){
            temp = rhs;
            temp->m_right = merge(lhs, rhs->m_right);
        } else {
            temp = lhs;
            temp->m_right = merge(lhs->m_right, rhs);
        }
    }

    //Restructuring heap based on struture type, leftist or skew.
    if (m_structure == SKEW){ //If skew, swap at every node visited.
        swap(temp->m_right, temp->m_left);
    } else { //If leftist, update NPL of every node visited and check whether or not to swap.
        updateNPL(temp);
        checkNPLs(temp);
    }
    return temp;
}

void PQueue::updateNPL(Node* node){
    int leftNPL, rightNPL;
    //Calculating left and right npls
    if (!node->m_left) {leftNPL = -1;} else {leftNPL = node->m_left->m_npl;}
    if (!node->m_right) {rightNPL = -1;} else {rightNPL = node->m_right->m_npl;}
    //Return the smallest npl plus 1.
    node->m_npl = (leftNPL > rightNPL ? rightNPL + 1 : leftNPL + 1);
}

void PQueue::checkNPLs(Node* node){
    //Checkling left and right child of node to determine if we need to swap.
    if (!node->m_left && !node->m_right) {return;} //Do nothing if node is leaf.
    else if (!node->m_right) {return;} //Left exists without right, so do nothing.

    else if (!node->m_left && node->m_right){ //If right exists but not left, swap.
        swap(node->m_left, node->m_right);
    }
    else if (node->m_left->getNPL() < node->m_right->getNPL()) { //If left NPL less than right, swap.
        swap(node->m_left, node->m_right);
    }
}

void PQueue::insertPatient(const Patient &patient) {
    Node* temp = new Node(patient);
    m_size++;
    m_heap = merge(m_heap, temp); //Merging heap with new node.
}

int PQueue::numPatients() const {
    return m_size;
}

prifn_t PQueue::getPriorityFn() const {
    return m_priorFunc;
}

Patient PQueue::getNextPatient() {
    if (m_size == 0) { //Throw expection if heap is empty.
        throw std::out_of_range("OUT OF RANGE: heap is empty.");
    }

    m_size--;
    Patient patient = m_heap->m_patient;
    Node* temp = m_heap; //Store root to delete.
    m_heap = merge(m_heap->m_left, m_heap->m_right); //Merge left subtree and right subtree.
    delete temp;
    return patient;
}

void PQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    if (m_priorFunc == priFn && m_heapType == heapType) {return;} //Prevent needless refactoring.
    Patient arr[m_size]; //Storing all patients.
    int size = m_size;

    for (int i = 0; i < size; i++){
        arr[i] = getNextPatient();
    }

    //Setting new priorityFn and heapType, then rebuilding.
    m_priorFunc = priFn;
    m_heapType = heapType;
    for (int j = 0; j < size; j++){
        insertPatient(arr[j]);
    }
}

void PQueue::setStructure(STRUCTURE structure) {
    if (m_structure == structure) {return;} //Prevent needless refactoring.
    Patient arr[m_size]; //Storing all patients.
    int size = m_size;
    
    for (int i = 0; i < size; i++){
        arr[i] = getNextPatient();
    }

    //Setting new structure, then rebuilding.
    m_structure = structure;
    for (int j = 0; j < size; j++){
        insertPatient(arr[j]);
    }
}

STRUCTURE PQueue::getStructure() const {
    return m_structure;
}

void PQueue::printPatientQueue() const {
    printPatientQueue(m_heap);
}

void PQueue::printPatientQueue(Node *root) const {
    if (!root) {return;}

    // Printing out the patients in our heap, preorder traversal.
    cout << "[" << m_priorFunc(root->getPatient()) << "] " << root->m_patient << endl;

    // After root, go left, then right.
    printPatientQueue(root->m_left);
    printPatientQueue(root->m_right);
}

void PQueue::dump() const {
    if (m_size == 0){
        cout << "Empty heap.\n";
    } else {
        dump(m_heap);
    }
    cout << endl;
}
void PQueue::dump(Node *pos) const {
    if (pos != nullptr){
        cout << "(";
        dump(pos->m_left);
        if (m_structure == SKEW)
            cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient();
        else
            cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient() << ":" << pos->m_npl;
        dump(pos->m_right);
        cout << ")";
    }
}

ostream &operator<<(ostream &sout, const Patient &patient) {
    sout << patient.getPatient()
         << ", temperature: " << patient.getTemperature()
         << ", oxygen: " << patient.getOxygen()
         << ", respiratory: " << patient.getRR()
         << ", blood pressure: " << patient.getBP()
         << ", nurse opinion: " << patient.getOpinion();
    return sout;
}

ostream &operator<<(ostream &sout, const Node &node) {
    sout << node.getPatient();
    return sout;
}