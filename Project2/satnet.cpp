// UMBC - CMSC 341 - Fall 2023 - Proj2
#include "satnet.h"
SatNet::SatNet(){
    m_root = nullptr;
}

SatNet::~SatNet(){
    clear();
}

void SatNet::insert(const Sat& satellite){
    if (satellite.m_id > MAXID || satellite.m_id < MINID) {return;}
    m_root = insert(m_root, satellite);
}

Sat* SatNet::insert(Sat* root, const Sat& satellite){
    //Insert satellite when we reach correct position, which is when root is nullptr.
    if (!root){
        root = new Sat(satellite);
        return root;
    }

    if (root->m_id > satellite.m_id){ //Go left if sat id < root id
        root->m_left = insert(root->m_left, satellite);
        updateHeight(root); //Update height after insert.
        return rotate(root); //Do any rotations necessary.
    }
    if (root->m_id < satellite.m_id){ //Go right if sat id > root id
        root-> m_right = insert(root->m_right, satellite);
        updateHeight(root);
        return rotate(root);
    }

    return root; //Case where ID already in tree.
}

Sat* SatNet::rotate(Sat* root){
    //Rotate left if tree is skewed to the left, and tree has a V shape.
    if (balanceFactor(root) > 1 && balanceFactor(root->m_left) >= 0){
        return rightRotate(root);
    }
    //Rotate right if root is skewed to the right, and tree has a V shape.
    else if (balanceFactor(root) < -1 && balanceFactor(root->m_right) <= 0){
        return leftRotate(root);
    }
    //Left-right rotation if tree is skewed to the left and right subtree
    //is not simple.
    else if (balanceFactor(root) > 1 && balanceFactor(root->m_left) <= 0){
        root->m_left = leftRotate(root->m_left);
        return rightRotate(root);
    }
    //Right-left rotation if tree is skewed to the right and left subtree
    //is not simple
    else if (balanceFactor(root) < -1 && balanceFactor(root->m_right) >= 0){
        root->m_right = rightRotate(root->m_right);
        return leftRotate(root);
    }
    else {return root;} //Case where no rotation is required.
}

int SatNet::balanceFactor(Sat* root){
    if (!root) {return 0;}
    int leftChildHeight, rightChildHeight;
    //Checking height of each child. If child doesn't exist, then set height to -1.
    if (!root->m_left){leftChildHeight = -1;} else {leftChildHeight = root->m_left->m_height;}
    if (!root->m_right){rightChildHeight = -1;} else {rightChildHeight = root->m_right->m_height;}

    //BF calculated using leftHeight - rightHeight.
    return leftChildHeight - rightChildHeight;
}

void SatNet::clear(){
    clear(m_root);
    m_root = nullptr;
}

void SatNet::clear(Sat* root){
    //Recursively delete using postorder traversal.
    if (!root) {return;}
    clear(root->m_left);
    clear(root->m_right);
    delete root;
    root = nullptr;
}

void SatNet::remove(int id){
    m_root = remove(m_root, id);
}

Sat* SatNet::remove(Sat* root, int id){
    if (!root) {return nullptr;} //Return if Sat with id doesn't exist.

    if (root->m_id > id){ //Search left.
        root->m_left = remove(root->m_left, id);
        updateHeight(root); //Update height if necessary.
    }
    if (root->m_id < id){ //Search right.
        root->m_right = remove(root->m_right, id);
        updateHeight(root);
    }
    
    if(root->m_id == id){
        if (root->m_height == 0){ //Case where node is leaf.
            delete root;
            return nullptr;
        }
        else if (root->m_left && root->m_right){ //Case where node has left and right child.
            Sat* temp = findMin(root->m_right);
            root->m_id = temp->m_id;
            root->m_right = remove(root->m_right, temp->m_id);
            updateHeight(root);
            return root;
        }
        else if (root->m_left){ //Case where node has left child.
            Sat* temp = root;
            root = root->m_left;
            delete temp;
            updateHeight(root);
            return root;
        }
        else{ //Case where node has right child.
            Sat* temp = root;
            root = root->m_right;
            delete temp;
            updateHeight(root);
            return root;
        }
    }
    return root;
}

Sat* SatNet::findMin(Sat* root){
    //Recursive function that returns address of min Sat.
    if (!root->m_left){
        return root;
    }
    return findMin(root->m_left);
}

void SatNet::dumpTree() const {
    dump(m_root);
}

void SatNet::dump(Sat* satellite) const{
    if (satellite != nullptr){
        cout << "(";
        dump(satellite->m_left);//first visit the left child
        cout << satellite->m_id << ":" << satellite->m_height;//second visit the node itself
        dump(satellite->m_right);//third visit the right child
        cout << ")";
    }
}

void SatNet::listSatellites() const {
    listSatellites(m_root);
}

void SatNet::listSatellites(Sat* root) const{
    //Inorder traversal for ascending ID order.
    if (root->m_left){ //Go left
        listSatellites(root->m_left);
    }

    cout << root->getID() << ": " << root->getStateStr() << ": " << root->getInclinStr()
    << ": " << root->getAltStr() << endl;

    if (root->m_right){ //Go right
        listSatellites(root->m_right);
    }
}

bool SatNet::setState(int id, STATE state){
    return setState(m_root, id, state);
}

bool SatNet::setState(Sat* root, int id, STATE state){
    if (!root) {return false;} //Return false if Sat doesn't exist.

    if (root->m_id == id){ //Update state and return true if Sat found.
        root->m_state = state;
        return true;
    }
    
    //Going left or right depending on the id.
    if (root->m_id > id){ 
        return setState(root->m_left, id, state);
    }
    if (root->m_id < id){
        return setState(root->m_right, id, state);
    }
}

void SatNet::removeDeorbited(){
    m_root = removeDeorbited(m_root);
}

Sat* SatNet::removeDeorbited(Sat* root){
    if (!root) {return root;}

    //Remove Sat if Sat is DEORBITED.
    if (root->m_state == DEORBITED){
        root = remove(root, root->m_id);
    }
    
    //Traverse left and right to find other DEORBITED Sats.
    if (root->m_left){
        root->m_left = removeDeorbited(root->m_left);
    }
    if (root->m_right){
        root->m_right = removeDeorbited(root->m_right);
    }
    return root;
}

bool SatNet::findSatellite(int id) const {
    return findSatellite(m_root, id);
}

bool SatNet::findSatellite(Sat* root, int id) const{
    if (!root){return false;} //If root is nullptr, then it doesn't exist.
    if (root->m_id == id){return true;} //Return true if found.
    
    if (root->m_id > id){ //Search left
        return findSatellite(root->m_left, id);
    }
    if (root->m_id < id){ //Search right
        return findSatellite(root->m_right, id);
    }
}
const SatNet & SatNet::operator=(const SatNet & rhs){
    if (this == &rhs) {return *this;} //Preventing self assignment.
    clear(); //Must first deallocate memory of lhs.
    m_root = operatorHelper(rhs.m_root);
    return *this;
}

Sat* SatNet::operatorHelper(Sat* root){
    if (!root) {return nullptr;}

    //Dynamically create identical satellite.
    Sat* temp = new Sat(root->m_id, root->m_altitude, root->m_inclin, root->m_state);
    temp->m_height = root->m_height;
    
    if (root->m_left){ //Recursively copy left subtrees.
        temp->m_left = operatorHelper(root->m_left);
    }
    if (root->m_right){ //Recursively copy right subtrees.
        temp->m_right = operatorHelper(root->m_right);
    }

    return temp;
}

int SatNet::countSatellites(INCLIN degree) const{
    return countSatellites(m_root, degree);
}

int SatNet::countSatellites(Sat* root, INCLIN degree) const{
    if (!root) {return 0;}
    int count = 0;
    
    if (root->m_inclin == degree){
        count++; //Increment count if Sat has incline degree.
    }

    //Get the counts of the left and right subtree;
    count += countSatellites(root->m_left, degree);
    count += countSatellites(root->m_right, degree);
    return count;
}

Sat* SatNet::rightRotate(Sat* root){
    Sat* y = root->m_left; //Get root's left child
    Sat* x = root;

    x->m_left = y->m_right; //Setting x's left to y's right child.
    y->m_right = x; //Making y the parent of x, and x the right child of y.

    //Updating both heights accordingly.
    updateHeight(x);
    updateHeight(y);

    return y; //Return rotated subtree.
}

Sat* SatNet::leftRotate(Sat* root){
    Sat* y = root->m_right; //Get root's right child
    Sat* x = root;

    x->m_right = y->m_left; //Setting x's right to y's left child.
    y->m_left = x; //Making y the parent of x, and x the left child of y.

    //Updating both heights accordingly.
    updateHeight(x);
    updateHeight(y);

    return y; //Return rotated subtree.
}

void SatNet::updateHeight(Sat* root){
    int leftChildHeight, rightChildHeight;
    //Checking height of each child. If child doesn't exist, then set height to -1.
    if (!root->m_left){leftChildHeight = -1;} else {leftChildHeight = root->m_left->m_height;}
    if (!root->m_right){rightChildHeight = -1;} else {rightChildHeight = root->m_right->m_height;}

    //We set root's height to the highest child plus 1.
    root->m_height = (leftChildHeight > rightChildHeight ? leftChildHeight + 1 : rightChildHeight + 1);
}