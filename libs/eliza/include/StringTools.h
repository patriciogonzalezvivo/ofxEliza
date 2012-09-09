//
//  StringTools.h
//  Eliza
//
//  Created by Patricio Gonzalez Vivo on 9/8/12.
//
//

#ifndef Eliza_StringTools_h
#define Eliza_StringTools_h

int find(vstring v, std::string str) {
    int iLim = v.size();
    for(int i = 0; i < iLim; ++i) {
        if(v[i] == str) {
            return i;
        }
    }
    return -1;
}

int rfind(vstring v, std::string str) {
    int start = v.size() - 1;
    for(int i = start; i >= 0; --i) {
        if(v[i] == str) {
            return start - i;
        }
    }
    return -1;
}

template<typename T>
void shuffle(T &array, size_t size) {
    for(int i = 0; i < size; ++i) {
        int index = rand() % size;
        std::swap(array[i], array[index]);
    }
}

#endif
