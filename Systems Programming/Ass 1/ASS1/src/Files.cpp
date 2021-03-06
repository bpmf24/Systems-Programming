#include "../include/Files.h"
#include "../include/GlobalVariables.h"
#include <iostream>
#include <algorithm>
using namespace std;


BaseFile ::BaseFile(string name) :name(name) {
}


string BaseFile::getName() const {
    return name;
}

void BaseFile::setName(string newName) {
    name = newName;
}

File ::File(string name, int size) : BaseFile(name),size(size)
{}

int File ::getSize() {
    return size;
}
void File ::clear() {
    delete(this);
}

BaseFile* File::clone() const { return  new File(*this);}

bool File::dirOrFile() { return false;}

Directory::Directory(string name, Directory *parent) : BaseFile(name),children(), parent(){
    this->setParent(parent);
}

Directory::Directory(const Directory& other):BaseFile(other.getName()),children() ,parent(nullptr){
    dircopy(other.children);
    if(verbose==1 || verbose==3){
        cout << "Directory::Directory(const Directory& other):BaseFile(other.getName()) ,parent(other.getParent()),children()";
    }
}

void Directory::dircopy(vector<BaseFile*> other)   {

    for(unsigned int i=0 ;i<other.size();i++){
        this->addFile((other[i]->clone()));
    }
}

Directory:: Directory(Directory &&other):BaseFile(other.getName()),children(), parent(nullptr)
{
    if(verbose==1 || verbose==3) {
        cout << "Directory:: Directory(Directory &&other):BaseFile(other.getName())";
    }
    clear();
    steal(other);
}

void Directory :: steal(Directory& other)
{
    parent = other.getParent();
    children = other.children;
}

Directory& Directory::operator=( Directory &&other)
{
    if(verbose==1 || verbose==3) {
        cout << "Directory& Directory::operator=( Directory &&other)";
    }
    if(this != &other){
        clear();
        this->setName(other.getName());
        steal(other);
    }
    return *this;
}


Directory& Directory::operator=(const Directory &other) {
    if(verbose==1 || verbose==3) {
        cout << "Directory& Directory::operator=(const Directory &other)";
    }
    if(this != &other){
        clear();
        this->setName(other.getName());
        this->setParent(nullptr);
        dircopy(other.children);
    }
    return *this;
}



Directory* Directory::getParent() const {
    return parent;
}

void Directory::setParent(Directory *newParent) {
    parent = newParent;
}

void Directory:: addFile(BaseFile* file){
    if(file->dirOrFile()){
        dynamic_cast<Directory*>(file)->setParent(this);
    }
    children.push_back(file);
}

void Directory:: removeFile(string name) {
    if (children.size() == 0) {
        return;
    } else {
        for (vector<BaseFile *>::iterator it = children.begin(); it != children.end(); ++it) {
            if ((*it)->getName() == name) {
                if ((*it)->dirOrFile()) {
                    Directory *t = dynamic_cast<Directory *>(*it);
                    delete t;
                    t = nullptr;
                    children.erase(it);
                    return;
                } else {
                    File *f = dynamic_cast<File *>(*it);
                    delete f;
                    f = nullptr;
                    children.erase(it);
                    return;
                }
            }
        }
    }

}

Directory ::~ Directory(){
    if(verbose == 1 || verbose == 3){
        cout << "Directory ::~ Directory()" << endl;
    }
    clear();
};

void Directory:: removeFile(BaseFile* file){
    this->removeFile(file->getName());
}

void Directory::sortByName() {
    sort(children.begin(), children.end(),[](BaseFile *baseFile1,BaseFile *baseFile2){
        return baseFile1->getName()<baseFile2->getName();
    });
}

void Directory::sortBySize() {
    sort(children.begin(), children.end(),[](BaseFile *baseFile1,BaseFile *baseFile2){
        if(baseFile1->getSize()== baseFile2->getSize()) {
            return baseFile1->getName() < baseFile2->getName();
        }
        else {
            return baseFile1->getSize() < baseFile2->getSize();
        }
    });
}

vector<BaseFile*> Directory:: getChildren(){
    return children ;
}

int Directory::getSize() {

    int sum = 0;
    for(vector<BaseFile*> ::iterator  it = children.begin();it!= children.end();it++)
    {
        sum+= (*it)->getSize();
    }
    return sum;
}

string Directory::getAbsolutePath() {
    if(parent != nullptr && parent->getParent() == nullptr) {
        return "/" + getName();
    }
    if(parent!= nullptr) {
        return getParent()->getAbsolutePath() + "/" + getName();
    }
    else{
        return "/";
    }
}

BaseFile* Directory::clone() const { return  new Directory(*this);}

void Directory:: clear() {
    for(vector<BaseFile*> :: iterator it = children.begin(); it != children.end(); it++)
    {
        if(typeid(*it) == typeid(Directory))
            ((Directory*)*it)->removeFile((*it)->getName());
        delete *it;
        *it = nullptr;
    }
    children.clear();
}

void Directory::printChildren(){
    for (vector<BaseFile *> ::iterator it = children.begin(); it != children.end(); ++it) {
        if((*it)->dirOrFile()){
        cout << "DIR" << "\t" << (*it)->getName() <<  "\t" <<  (*it)->getSize()  << endl ; }
        else{
    cout << "FILE" << "\t" << (*it)->getName() <<  "\t" <<  (*it)->getSize() << endl ; }
}
    }
bool Directory::containsChild(BaseFile* file) {
    vector<BaseFile*> :: iterator it;
    for(it = children.begin(); it != children.end(); it++){
        if((*it)->getName() == (file)->getName()) {
            return true;
        }
    }
    return false;
}
bool Directory::dirOrFile() { return true;}

size_t Directory::dirCountBySlash(){
    size_t j = 0;
    if(getParent() == nullptr){
        return j;
    }
    else{
        j++;
        return getParent()->dirCountBySlash() + j;
    }
}
