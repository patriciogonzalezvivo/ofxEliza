#ifndef __ELIZA__H__
#define __ELIZA__H__

#pragma warning(disable:4786)

#include "ofMain.h"

#include "Tokenizer.h"
#include "StringTools.h"

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

class Eliza {
public:
	Eliza ()
		: m_bQuitProgram(0), linePos(2), m_bNewData(0), m_bMemoryRecall(0), m_nShortInputCount(0),
		m_bLearning(0), m_nUserRepeatCount(0), m_sUserName("USER")
    {
		seed_random_generator();
	}

	~Eliza () {
		logfile.flush();
		logfile.close();
	};

	void load_data();
	void save_data();

	void respond();
    std::string get_respond(){
        return m_sResponse;
    }

	void preProcessInput();
	void preProcessResponse();

	void memorise_input();
	void get_input();
    void set_input(std::string _input){
//        std::cout << "\n" << ">";
        save_prev_input();
        m_sInput = _input;
//        cout << m_sInput << endl;
        save_log("USER");
    }

	void start();
	
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
	
	bool bot_repeat() const;

	bool similar_response() const;

	bool null_input() const {
		return m_sInput.length() == 0;
	}

	bool short_input() {
		return tok.getTokenNumber(m_sInput) < 4;
	}

	bool bot_is_repeating() const {
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


#endif