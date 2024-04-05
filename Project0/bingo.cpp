// UMBC - CMSC 341 - Fall 2023 - Proj0
// Giovanni Birriel Gomez
#include "bingo.h"
Bingo::Bingo(){
    m_trackCols = nullptr;
    m_trackRows = nullptr;
    m_helper = nullptr;
    m_card = nullptr;
    m_helperSize = 0;
    m_numCols = 0;
    m_numRows = 0;
    m_minBallVal = 0;
    m_maxBallVal = 0;
}

Bingo::Bingo(int rows, int columns, int min, int max){
    // Checking if args are valid. If not, create default card.
    if (!isValidCard(rows, columns, min, max)){
        m_trackCols = nullptr;
        m_trackRows = nullptr;
        m_helper = nullptr;
        m_card = nullptr;
        m_helperSize = 0;
        m_numCols = 0;
        m_numRows = 0;
        m_minBallVal = 0;
        m_maxBallVal = 0;
        return;
    }

    // Setting member variables & allocating memory for obj.
    m_numCols = 5, m_numRows = rows, m_minBallVal = min, m_maxBallVal = max, m_helperSize = 0;
    m_trackCols = nullptr, m_trackRows = nullptr, m_helper = nullptr;
    m_card = new Cell*[m_numRows];
    for (int i = 0; i < m_numRows; i++){
        m_card[i] = new Cell[m_numCols];
    }
}

bool Bingo::reCreateCard(int rows, int columns, int min, int max){
    // Checking if args are valid. If not, return false.
    if (!isValidCard(rows, columns, min, max)){
        return false;
    }

    // If obj is non-default, deallocate memory and set to default.
    if (m_numCols != 0){
        clear();
    }

    // Setting member variables & allocating memory for new obj.
    m_numCols = 5, m_numRows = rows, m_minBallVal = min, m_maxBallVal = max;
    m_card = new Cell*[m_numRows];
    for (int i = 0; i < m_numRows; i++){
        m_card[i] = new Cell[m_numCols];
    }

    return true;
}

Bingo::~Bingo(){
    // Checking if obj has memory allocated before deleting.
    if (m_card){
        for (int i = 0; i < m_numRows; i++){
            delete[] m_card[i];
        }
        delete[] m_card;
    }
    // Checking if obj has been initalized before deleting.
    if (m_helper){
        delete[] m_trackCols;
        delete[] m_trackRows;
        delete[] m_helper;
    }
}

void Bingo::clear(){
    // Checking if obj has memory allocated before deleting.
    if (m_card){
        for (int i = 0; i < m_numRows; i++){
            delete[] m_card[i];
        }
        delete[] m_card;
    }

    // Checking if obj has been initialized before deleting.
    if (m_helper){
        delete[] m_trackCols;
        delete[] m_trackRows;
        delete[] m_helper;
    }
    
    // Setting obj's values to the default.
    m_trackCols = nullptr;
    m_trackRows = nullptr;
    m_helper = nullptr;
    m_card = nullptr;
    m_helperSize = 0;
    m_numCols = 0;
    m_numRows = 0;
    m_minBallVal = 0;
    m_maxBallVal = 0;
}

bool Bingo::initCard(){
    // Checks if memory has been allocated for obj (thus a valid card).
    if (!m_card){
        return false;
    }

    // Setting up helper variables.
    m_helperSize = m_maxBallVal + 1;
    m_helper = new Cell[m_helperSize];
    m_trackCols = new int[m_numCols]();
    m_trackRows = new int[m_numRows]();

    // This for block sets vals for the Cells. toBeEntered holds the possible values allowed for
    // each column i. valsPerCol determines how many values can exist per column. Values are added
    // column by column.
    int valsPerCol = (m_maxBallVal - m_minBallVal + 1) / m_numCols;
    int toBeEntered[m_maxBallVal - m_minBallVal + 1];

    for (int i = 0; i < m_numCols; i++){
        // min represent minimum value allows in column, max represents maximum value allowed.
        int min = m_minBallVal + (valsPerCol * i);
        int max = (m_minBallVal + (valsPerCol * (i + 1))) - 1;
        Random random(min, max, SHUFFLE);
        random.getShuffle(toBeEntered); // Filling toBeEntered with values from min to max.
        for (int j = 0; j < m_numRows; j++){
            m_card[j][i].setCol(i);
            m_card[j][i].setRow(j);
            m_card[j][i].setVal(toBeEntered[j]);

            // Storing added cell to m_helper
            m_helper[toBeEntered[j]] = m_card[j][i];
        }
    }

    return true;
}

vector<int> Bingo::drawBalls(){
    vector<int> drawings;
    Random random(m_minBallVal, m_maxBallVal, SHUFFLE);
    random.getShuffle(drawings); // Inserting values from minBallVal to maxBallVal.
    return drawings;
}

int Bingo::play(int numDraws, vector<int> rndBalls){
    // Checking if card has been initialized or not. If not, return 0.
    if (!m_card || !m_helper){
        return 0;
    }
    
    int numOfPulls = 0; // Tracks number of draws executed.
    for (int i = 0; i < numDraws; i++){
        int valPulled = rndBalls[i];
        if (m_helper[valPulled].getVal() == valPulled){
            Cell *targetCell = &m_helper[valPulled]; // Pointer to cell for cleaner code.
            numOfPulls++; // Incrementing number of draws executed
            targetCell->setVal(EMPTYCELL);
            m_card[targetCell->getRow()][targetCell->getCol()].setVal(EMPTYCELL); // Setting cell to EMPTYCELL.

            // These two if statements first increment the helper arrays. Then if a row
            // or column is filled, or max number of draws have been issued, return the
            // number of balls drawn.
            if (++m_trackCols[targetCell->getCol()] == m_numRows || numOfPulls == numDraws){
                return numOfPulls;
            }
            if (++m_trackRows[targetCell->getRow()] == m_numCols || numOfPulls == numDraws){
                return numOfPulls;
            }
        }
    }
    return numOfPulls;
}

const Bingo & Bingo::operator=(const Bingo & rhs){
    // Preventing self assignment.
    if (this == &rhs){
        return *this;
    }
    clear(); // Flush obj of any memory.
    // Copying variables and allocating memory for helper variables.
    m_numRows = rhs.m_numRows;
    m_numCols = rhs.m_numCols;
    m_minBallVal = rhs.m_minBallVal;
    m_maxBallVal = rhs.m_maxBallVal;
    m_helperSize = rhs.m_helperSize;

    // If rhs obj is default obj, then just return.
    if (!rhs.m_card){
        return *this;
    }

    // Allocating memory for our cells.
    m_card = new Cell*[m_numRows];
    for (int i = 0; i < m_numRows; i++){
        m_card[i] = new Cell[m_numCols];
    }

    // If rhs obj is not initialized, then just return.
    if (!rhs.m_helper){
        return *this;
    }

    // Allocating memory for helper arrays.
    m_helper = new Cell[m_helperSize];
    m_trackCols = new int[m_numCols];
    m_trackRows = new int[m_numRows];

    // Copying helper variables.
    for (int i = 0; i < m_helperSize; i++){
        m_helper[i] = rhs.m_helper[i];
    }
    for (int i = 0; i < m_numCols; i++){
        m_trackCols[i] = rhs.m_trackCols[i];
    }
    for (int i = 0; i < m_numRows; i++){
        m_trackRows[i] = rhs.m_trackRows[i];
    }

    // Setting lhs cells to the cell values of rhs.
    for(int i = 0; i < m_numRows; i++){
        for (int j = 0; j < m_numCols; j++){
            m_card[i][j].setCol(rhs.m_card[i][j].getCol());
            m_card[i][j].setRow(rhs.m_card[i][j].getRow());
            m_card[i][j].setVal(rhs.m_card[i][j].getVal());
        }
    }

    return *this;
}

// The dump function renders the card in the terminal
// This function is provided to facilitate debugging
// Using this function as a test case will not be accepted
void Bingo::dumpCard(){
    cout << "  ";
    cout << "\033[1;35m B   I   N   G   O\033[0m";    
    cout << endl;
    for (int i=1;i <= m_numRows;i++){
        if ((i < 10))
            cout << "\033[1;35m" << "0" << i << " \033[0m";
        else
            cout << "\033[1;35m" << i << " \033[0m";
        for (int j=1;j<=m_numCols;j++){
            if (m_card[i-1][j-1].getVal() == EMPTYCELL)
                cout << "\033[1;31m" << m_card[i-1][j-1].getVal() << "\033[0m" << "  ";
            else
                cout << m_card[i-1][j-1].getVal() << "  ";
        }
        cout << endl;
    }
    cout << endl;
}

// the overloaded equality operator for the Cell object
bool operator==(const Cell & lhs, const Cell & rhs){
    return ((lhs.m_col == rhs.m_col) &&
            (lhs.m_row == rhs.m_row) &&
            (lhs.m_value == rhs.m_value));
}

bool Bingo::isValidCard(int rows, int columns, int min, int max){
        // This if statement checks for every possible invalid arg input.
        // Outcome indicates whether the card is valid or invalid.
        if (rows < 2 || rows > 15 || columns != 5 || min < MINVAL || max > MAXVAL
        || (max - min + 1) % 5 != 0 || (rows * columns) > (max - min + 1)){
            return false;
        }
        return true;
    }