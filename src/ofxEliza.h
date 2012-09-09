#pragma once

#include "ofMain.h"

#include "Eliza.h"

class ofxEliza {
public:
    
    ofxEliza(){
		eliza.load_data();
		eliza.start();
    };
    
    string ask(string question){
        eliza.set_input(question);
        eliza.respond();
        return eliza.get_respond();
    }
    
    void save(){
        if(eliza.learn()) {
            eliza.save_data();
        }
        eliza.save_unknown_sentences();
    }
    
private:
    Eliza eliza;
};