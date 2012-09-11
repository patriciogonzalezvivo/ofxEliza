/*
 Program Name:
 Chatterbot Eliza 2.0
 
 Intro:
 This version of the program is smarter than ever, more new features have been
 added since the last submission and also now the conversation log between
 the users and the chatbot is automaticaly saved into the file: log.txt
 And finaly the "script file" (script.txt) wich acts as a knowledge base
 for the chatbot has been totaly rewritten and it is definitly better
 than in the previous versions of the program.
 
 Description:
 These program is an Eliza like chatterbot,bots like Eliza are the results
 of researchs in Artificial Intelligence (more specificly: in NLP and NLU)
 NLP: Natural Language Processing, NLU: Natural Language Understanding
 The first chatterbot was published in 1966 by Joseph Weizenbaum a professor of MIT.
 (http://i5.nyu.edu/~mm64/x52.9265/january1966.html)
 And also,most of the chatterbots that have been written this days are largely based
 on the original chatterbot Eliza that was written by Joseph Weizenbaum which means
 that they use some appropriate keywords to select the responses to generate
 when they get new inputs from the users. More generaly,the technique that are
 in use in a "chatterbot database" or "script file" to represent the chatterbot
 knowledge is known as "Case Base Reasoning" or CBR. A very good example of an Eliza
 like chatterbot would be "Alice",these program has won the Loebner prize for most
 human chatterbot three times (www.alicebot.org). The goal of NLP and NLU is to create
 programs that are capable of understanding natural languages and also capable
 of processing it to get input from the user by "voice recognition" or to produce
 output by "text to speech". During the last decades there has been a lot of progress
 in the domains of "Voice Recognition" and "Text to Speech",however the goal of NLU
 that is to make software that are capable of showing a good level of understanding
 of "natural languages" in general seems quiet far to many A.I experts. The general
 view about this subject is that it would take at list many decades before any computer
 can begin to really understand "natural language" just as the humans do.
 These code is Copyrighted and has limited warranty.
 
 Features of the Program:
 There are many interesting features in the current program,these chatterbot
 is capable of avoiding repetitions when selecting new responses,it can also
 follow the context of a conversation with a user. When the programs detect
 new keywords that are not part of the "script file", it saves it on the file
 "unknown.txt" and finaly it simulates a "human typist" when displaying responses
 on the screen. Actualy,there are many more features in the program,you will be
 capable of finding them by reading the "source code".
 
 Changes Since Last Submission:
 The implementation of the program has been improved, the repetitions made by the program
 are better handled, the context in a conversation is also better handled, the program can now
 correct grammatical errors that can occure after conjugating verbs. Finaly, the database is
 bigger than the last time. Also a new feature has been added for controling the usage of
 short inputs by the user. With these new feature, the chatterbot make sure that the user use
 longer sentences and these in turn might garranty the chances of finding more keywords within
 the user's input. The database (script.txt) has been updated, more entries have been added
 and also some corrections have been made with the formating of some keywords.
 
 Author:
 Gonzales Cenelia
 
 Website:
 www.ai-search.4t.com
 */

#pragma once
//#pragma warning(disable:4786)

#include "ofMain.h"
#include "Tokenizer.h"

typedef struct {
	vstring keywords;
	vstring contexts;
	vstring responses;
	vstring cmd;
} data;

typedef struct {
	std::string verbP1;
	std::string verbP2;
} transpos;

typedef struct {
	std::string verbP1;
	std::string verbP2;
} correction;

template<typename T>
void shuffle(T &array, size_t size) {
    for(int i = 0; i < size; ++i) {
        int index = rand() % size;
        std::swap(array[i], array[index]);
    }
}

class ofxEliza {
public:

    ofxEliza();
    ~ofxEliza();
    
    // loading database into memory
    void init(string _scriptFile = "script.txt", string _logFile = "log.txt");
    void load();
    void save( string _scriptFile = "script.txt");
    
    string ask(string _inputString);
    
	void preProcessInput();
	void preProcessResponse();
    
	void memorise_input();
    
	void print_response();
	void print_database_content();
	void save_unknown_sentences();
    
	void save_log(std::string str);
    
	bool quit() const {
		return m_bQuitProgram;
	}
    
	bool learn() const {
		return m_bNewData;
	}
    
private:
	void saveTopic(const vstring vList, const std::string sSymbol);
	void saveComment(const std::string comment);
	void saveTransposTable();
	void saveCorrections();
	void saveKeyWords();
    
	bool transpose( std::string &str ) ;
	void transpose_sentence( std::string &str );
    
	void correct_sentence( std::string &str ) ;
    
	void handle_repetition();
	void handle_user_repetition();
	void handle_unknown_sentence();
	void handle_null_input();
	void handle_short_input();
    
	void verify_context(vstring vContext);
    
	void verify_keyword_boundary(std::string keyWord, int pos);
    
	void print_current_data();
    
	void add_response(vstring v);
    
	void find_keyword();
	void find_response();
    
	void execute();
	
	void dump_data();
	void select_response();
	void extract_suffix();
    
	void clear();
	
	bool bot_repeat();
    
	bool similar_response();
    
	bool null_input() const {
		return m_sInput.length() == 0;
	}
    
	bool short_input() {
		return tok.getTokenNumber(m_sInput) < 4;
	}
    
	bool bot_is_repeating() {
		return (bot_repeat() || similar_response());
	}
    
	bool user_repeat() const {
		return (m_sInput.length() > 0 && m_sInput == m_sPrevInput);
	}
    
	bool bot_understand() const {
		return response_list.size() > 0;
	}
    
	bool good_context() const {
		return m_bGoodContext;
	}
    
	bool wrong_boundary() const {
		return m_bWrongBoundary;
	}
    
	void reset_repeat_count() {
		m_nUserRepeatCount = 0;
	}
    
	void reset_short_input_count() {
		m_nShortInputCount = 0;
	}
	
	void save_prev_response() {
		m_sPrevResponse = m_sResponse;
	}
    
	void save_prev_input() {
		m_sPrevInput = m_sInput;
	}
    
	void save_prev_responses() {
		if(m_sResponse.length() > 0) {
			previous_responses.push(m_sResponse);
		}
	}
	
	void remind_prev_subject();
    
	void check_quit_message() {
		if(m_sResponse.find("BYE") != std::string::npos) {
			m_bQuitProgram = 1;
		}
	}
    
	void seed_random_generator() {
		srand((unsigned) time(NULL));
	}
    
	bool is_template(std::string str) {
		return (str.find("*") != std::string::npos ||
                str.find("@") != std::string::npos ||
                str.find("%") != std::string::npos);
	}
    
	void findSymbol(std::string str);
	void save_user_name();
    
	bool isGoodKey(const std::string key, const std::string bkey, int pos, int bestp) const;
    
private:
    void trimLeft(std::string &str, char c) {
        int pos = 0;
        for(; str[pos] == c; ++pos)
            ;
        str.erase( 0, pos );
    }
    
    // trim the right character of a given string
    void trimRight(std::string &str, char c) {
        int pos = str.length();
        if( pos == 0 ) return;
        for(--pos; str[pos] == c; --pos)
            ;
        str.erase(++pos, str.length() - pos);
    }
    
    // trim all left and right characters that is specified by
    // the string "characters"
    void trimLR(std::string &str, std::string characters) {
        int len = characters.length();
        for(int i = 0; i < len; ++i) {
            trimLeft( str, characters[i] );
            trimRight( str, characters[i] );
        }
    }
    
    // removes a substring from a given string
    bool remove(std::string &str, std::string substr) {
        int pos = str.find(substr);
        if(pos != std::string::npos) {
            str.erase( pos, substr.length());
        }
        return (pos != std::string::npos);
    }
    
    // replace a substring by another substring
    bool replace(std::string &str, std::string oldsubstr, std::string newsubstr) {
        int pos = str.find(oldsubstr);
        if( pos != std::string::npos ) {
            remove( str, oldsubstr );
            str.insert( pos, newsubstr );
        }
        return (pos != std::string::npos);
    }
    
    void UpperCase( std::string &str ) {
        int len = str.length();
        for( int i = 0; i < len; i++ ) {
            if ( str[i] >= 'a' && str[i] <= 'z' ) {
                str[i] -= 'a' - 'A';
            }
        }
    }
    
    void LowerCase( std::string &str ) {
        int len = str.length();
        for( int i = 0; i < len; i++ ) {
            if ( str[i] >= 'A' && str[i] <= 'Z' ) {
                str[i] += 'a' - 'A';
            }
        }
    }
    
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
    
private:
	std::stack<std::string>		previous_responses;
	std::vector<transpos>		transpos_list;
	std::vector<correction>		correction_list;
	
	std::stack<std::string>		memory;
	
	std::vector<data>			database;
	std::vector<double>			time_delay;
	
	std::fstream				scriptfile;
	std::fstream				logfile;
	data						current_data;
	Tokenizer					tok;
	correction					current_correction;
	
	std::string					m_sInput;
	std::string					m_sResponse;
	std::string					m_sPrevInput;
	std::string					m_sKeyWord;
	std::string					m_sSuffix;
	std::string					m_sPrevResponse;
	std::string					m_sUserName;
	std::string					m_sCommand;
	std::string					m_sSymbol;
    
	vstring						nullResponse;
	vstring						myResponse;
	vstring						noKeyWord;
	vstring						topicChanger;
	vstring						subjectRecall;
	vstring						unknownSentences;
	vstring						inputRepeat;
	vstring						shortInput;
	vstring						keyword_list;
	vstring						response_list;
	vstring						previous_inputs;
	vstring						vResponseLog;
	vstring						command_list;
	vstring						comments;
	vstring						signOn;
	unsigned int				linePos;
	unsigned int				m_nUserRepeatCount;
	unsigned int				m_nShortInputCount;
	unsigned int				m_nCorrectionNum;
	unsigned int				m_nTransPosNum;
	bool						m_bWrongBoundary;
	bool						m_bGoodContext;
	bool						m_bQuitProgram;
	bool						m_bLearning;
	bool						m_bNewData;
	bool						m_bMemoryRecall;
    
};