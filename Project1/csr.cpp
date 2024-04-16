// CMSC 341 - Fall 2023 - Project 1
#include "csr.h"
///////////////CSR Class Implementation///////////////
CSR::CSR(){
    m_m = 0;
    m_n = 0;
    m_nonzeros = 0;
    m_values = nullptr;
    m_col_index = nullptr;
    m_row_index = nullptr;
    m_next = nullptr;
}
CSR::~CSR(){
    //If memory has been allocated, delete.
    if (m_values){
        delete[] m_values;
        delete[] m_col_index;
        delete[] m_row_index;
    }
}
CSR::CSR(const CSR & rhs){
    m_m = rhs.m_m;
    m_n = rhs.m_n;
    m_nonzeros = rhs.m_nonzeros;
    m_values = nullptr;
    m_col_index = nullptr;
    m_row_index = nullptr;
    m_next = nullptr;

    if (rhs.empty()) {return;} //Return if rhs is empty, lhs already set.

    if (rhs.m_nonzeros == 0) {return;} //If rhs is an all zero CSR, then return without allocing memory.

    // Allocating memory for member variables.
    m_values = new int[m_nonzeros];
    m_col_index = new int[m_nonzeros];
    m_row_index = new int[m_m + 1];

    // Copying rhs's arrays to lhs.
    for (int i = 0; i < m_nonzeros; i++){
        m_values[i] = rhs.m_values[i];
        m_col_index[i] = rhs.m_col_index[i];
    }
    for (int j = 0; j < m_m + 1; j++){
        m_row_index[j] = rhs.m_row_index[j];
    }
}
void CSR::clear(){
    if (m_values){ //if m_values has been set, then mem has been allocated.
        delete[] m_values;
        delete[] m_col_index;
        delete[] m_row_index;
    }
    m_m = 0;
    m_n = 0;
    m_nonzeros = 0;
    m_values = nullptr;
    m_col_index = nullptr;
    m_row_index = nullptr;
}
bool CSR::empty() const{
    if (m_m == 0 && m_n == 0){
        return true;
    }
    return false;
}
void CSR::compress(int m, int n, int array[], int arraySize){
    clear(); //emptying out calling obj.

    // Keeping obj empty if row or col = 0;
    if (m == 0 || n == 0) {return;}
    
    // If array is empty, then make CSR all zeros.
    if (arraySize == 0) {
        m_m = m;
        m_n = n;
        return;
    }
    m_m = m;
    m_n = n;

    int maxIndex; //represents the max index to traverse array until.
    maxIndex = ((m_m * m_n) < arraySize ? (m_m * m_n) : arraySize);

    // Finding how many nonzero values exist to create arrays.
    for (int i = 0; i < maxIndex; i++){
        if (array[i] != 0) {m_nonzeros++;}
    }
   
    // If arr passed was not empty but only contained 0's then make
    // CSR all zeros.
    if (m_nonzeros == 0) {return;}
    
    // Allocating memory for CSR member variables.
    m_values = new int[m_nonzeros];
    m_col_index = new int[m_nonzeros];
    m_row_index = new int[(m_m + 1)];

    int valsEntered = 0; // Tracking # of nonzeroes
    int currCol = 0; // Tracking current column
    int currRow = 0; // Tracking current row
    for (int j = 0; j < maxIndex; j++){
        currCol = j % m_n;
        if (currCol == 0){
            m_row_index[currRow] = valsEntered; //Adding seen # of values after each row.
            currRow++;
        }
        if (array[j] != 0){
            m_values[valsEntered] = array[j];
            m_col_index[valsEntered] = currCol;
            valsEntered++;
        }
    }

    //In the case that all values have been seen but there's more rows, we must
    //finish inserting values into m_row_index.
    while (currRow < m_m + 1){
        m_row_index[currRow] = valsEntered;
        currRow++;
    }
}
int CSR::getAt(int row, int  col) const{
     if (row >= m_m || row < 0 || col >= m_n || col < 0 || empty()){
        throw std::runtime_error("RUNTIME ERROR: Index out of range.");
    }

    if (m_nonzeros == 0){ //Returning zero if CSR is just an all zero matrix.
        return 0;
    }
    
    int val = 0;
    //Locating nonzero value if it exists given the row and col.
    for (int i = m_row_index[row]; i < m_row_index[row+1]; i++){
        if (m_col_index[i] == col){
            val = m_values[i];
        }
    }
    return val;
} 
bool CSR::operator==(const CSR & rhs) const{
    if (m_m != rhs.m_m || m_n != rhs.m_n || m_nonzeros != rhs.m_nonzeros){
        return false;
    }
    
    // In the case that both CSRs are filled with zeros.
    if (m_nonzeros == 0){
        return true;
    }

    //Verifiying equality of all 3 arrays of both CSRs.
    bool result = true;
    for (int i = 0; i < m_nonzeros; i++){
        result = result && (m_values[i] == rhs.m_values[i]);
        result = result && (m_col_index[i] == rhs.m_col_index[i]);
    }
    for (int j = 0; j < m_m + 1; j++){
        result = result && (m_row_index[j] == m_row_index[j]);
    }
    return result;
}
int CSR::sparseRatio(){
    if (empty()) {return 0;} // Returning 0 if empty.

    if (m_nonzeros == 0) {return 100;} // If m_nonzeros is 0, then CSR is all zeros.

    return ((m_m * m_n - m_nonzeros) / double(m_m * m_n)) * 100;
}
void CSR::dump(){
    cout << endl;
    if (!empty()){
        for (int i=0;i<m_nonzeros;i++)
            cout << m_values[i] << " ";
        cout << endl;
        for (int i=0;i<m_nonzeros;i++)
            cout << m_col_index[i] << " ";
        cout << endl;
        for (int i=0;i<m_m+1;i++)
            cout << m_row_index[i] << " ";
    }
    else
        cout << "The object is empty!";
    cout << endl;
}

//////////////CSRList Class Implementation///////////////
CSRList::CSRList(){
    m_size = 0;
    m_head = nullptr;
}
CSRList::CSRList(const CSRList & rhs){
    // If rhs empty, then lhs empty.
    if (rhs.empty()){
        m_size = 0;
        m_head = nullptr;
        return;
    }

    m_size = rhs.m_size;
    m_head = nullptr;
    *this = rhs;
}
CSRList::~CSRList(){
    if (!empty()){
        CSR* temp;
        while (m_head->m_next){
            temp = m_head;
            m_head = m_head->m_next;
            delete temp;
        }
        delete m_head;
    }
}
bool CSRList::empty() const{
    if (!m_head){
        return true;
    }
    return false;
}
void CSRList::insertAtHead(const CSR & matrix){
    if (empty()){ //If list is currently empty, make matrix the head.
        m_head = new CSR(matrix);
        m_size++;
        return;
    }
    CSR* tempCSR = new CSR(matrix); //Create a copy of matrix.
    tempCSR->m_next = m_head; //Set matrix's nextptr to head.
    m_head = tempCSR; //Make matrix head.
    m_size++;
}
void CSRList::clear(){
    if (empty()){
        return;
    }
    //Deleting all objects in the list.
    CSR* temp;
    while (m_head->m_next){
        temp = m_head;
        m_head = m_head->m_next;
        delete temp;
    }
    delete m_head;
    m_head = nullptr;
    m_size = 0;
}

int CSRList::getAt(int CSRIndex, int row, int col) const{
    if (CSRIndex >= m_size || empty()){ //If CSRIndex not in list, throw error
        throw std::runtime_error("RUNTIME ERROR: Object not in list.");
    }

    CSR* temp = m_head;
    int currMatrix = 0;
    while (currMatrix != CSRIndex){ //Traversing to CSRIndex
        temp = temp->m_next;
        currMatrix++;
    }
    return temp->getAt(row, col);
}

bool CSRList::operator== (const CSRList & rhs) const{
    if (m_size != rhs.m_size){
        return false;
    }
    if (m_size == 0){
        return true;
    }

    //Verifying all of the CSR's in both lists.
    bool result = true;
    CSR* templhs = m_head;
    CSR* temprhs = rhs.m_head;
    while (templhs->m_next){
        result = result && (templhs == temprhs);
        templhs = templhs->m_next;
        temprhs = temprhs->m_next;
    }
    result = result && (templhs == temprhs);
    return result;
}
const CSRList& CSRList::operator=(const CSRList & rhs){
    //Preventing self assignment.
    if (this == &rhs){
        return *this;
    }
    
    clear(); //Deallocating memory if lhs has it.

    if (rhs.empty()){
        return *this;
    }

    //Copying everything from rhs list to lhs list.
    m_size = rhs.m_size;
    m_head = new CSR(*rhs.m_head);
    CSR* templhs = m_head;
    CSR* temprhs = rhs.m_head->m_next;

    while (temprhs){
        templhs->m_next = new CSR(*temprhs);
        templhs = templhs->m_next;
        temprhs = temprhs->m_next;
    }
    return *this;
}
int CSRList::averageSparseRatio(){
    if (empty()){
        return 0;
    }

    int ratioSum = 0;
    CSR* temp = m_head;
    while (temp->m_next){
        ratioSum += temp->sparseRatio(); //Adding up all ratios from all CSRs.
        temp = temp->m_next;
    }
    ratioSum += temp->sparseRatio();

    return (ratioSum / double(m_size * 100)) * 100; //Calculating average.
}
void CSRList::dump(){
    if (!empty()){
        CSR* temp = m_head;
        while (temp != nullptr){
            temp->dump();
            temp = temp->m_next;
        }
    }
    else
        cout << "Error: List is empty!" << endl;
}