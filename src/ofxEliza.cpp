//
//  ofxEliza.cpp
//  Eliza
//
//  Created by Patricio Gonzalez Vivo on 9/11/12.
//
//

#include "ofxEliza.h"

ofxEliza::ofxEliza():linePos(2), m_bNewData(0), m_bMemoryRecall(0), m_nShortInputCount(0),m_bLearning(0), m_nUserRepeatCount(0), m_sUserName("USER") {
    seed_random_generator();
}

ofxEliza::~ofxEliza() {
    logfile.flush();
    logfile.close();
};


// loading database into memory
string ofxEliza::init(string _scriptFile, string _logFile){
    load(_scriptFile);
    return start(_logFile);
};

void ofxEliza::load(string _scriptFile){
    std::fstream fin( ofToDataPath(_scriptFile).c_str() , std::ios::in);
    if(fin.fail()) {
        throw std::string("can't open script file");
    }
    std::string buffer = "";
    std::string temp = "";
    int counter = 0, counter2 = 0, line = 0;
    transpos current_transpos;
    std::ostringstream os;
    char cSymbol = 0;
    char cPrevSymbol;
    while(std::getline(fin, buffer)) {
        line++;
        cPrevSymbol = cSymbol;
        cSymbol = buffer[0];
        if(temp.length() > 0 && cSymbol != ';'
           && cSymbol != cPrevSymbol) {
            temp.erase(temp.length() - 1, 1);
            comments.push_back(temp);
            temp.erase();
        }
        buffer.erase(0, 1);
        switch(cSymbol) {
            case ';':
                temp += ';';
                temp += buffer;
                temp += '\n';
                dump_data();
                break;
            case 'S':
                signOn.push_back(buffer);
                break;
            case 'T':
                ++counter;
                buffer.erase(buffer.length() - 1, 1);
                if(counter % 2 == 1) {
                    current_transpos.verbP1 = buffer;
                } else {
                    current_transpos.verbP2 = buffer;
                    transpos_list.push_back(current_transpos);
                }
                break;
            case 'E':
                ++counter2;
                buffer.erase(buffer.length() - 1, 1);
                if(counter2 % 2 == 1) {
                    current_correction.verbP1 = buffer;
                } else {
                    current_correction.verbP2 = buffer;
                    correction_list.push_back(current_correction);
                }
                break;
            case 'K':
                buffer.erase(buffer.length() - 1, 1);
                current_data.keywords.push_back(buffer);
                break;
            case 'R':
                current_data.responses.push_back(buffer);
                break;
            case 'M':
                subjectRecall.push_back(buffer);
                break;
            case 'N':
                nullResponse.push_back(buffer);
                break;
            case 'X':
                noKeyWord.push_back(buffer);
                break;
            case 'Z':
                topicChanger.push_back(buffer);
                break;
            case 'W':
                inputRepeat.push_back(buffer);
                break;
            case 'V':
                shortInput.push_back(buffer);
                break;
            case 'C':
                current_data.contexts.push_back(buffer);
                break;
            case 'A':
                current_data.cmd.push_back(buffer);
                break;
            case 0:
                break;
            default:
                os << "(Script error) Unknown symbol: " << cSymbol << ", line: " << line;
                throw os.str();
        }
    }
    dump_data();
    m_nTransPosNum = transpos_list.size();
    m_nCorrectionNum = correction_list.size();
    fin.close();
}

// the functions below can be used to save
// the content of the dabase to the script file.
void ofxEliza::save( string _scriptFile, string _unknownFile ){
    if( m_bNewData ) {
        scriptfile.open( ofToDataPath(_scriptFile).c_str(), std::ios::out);
        if(scriptfile.fail()) {
            throw std::string("Can't save data");
        }
        saveComment(comments[0]);
        saveTopic(signOn, "S");
        
        saveComment(comments[1]);
        saveTransposTable();
        saveCorrections();
        saveComment(comments[2]);
        
        saveTopic(nullResponse, "N");
        saveComment(comments[3]);
        
        saveTopic(subjectRecall, "M");
        saveComment(comments[4]);
        
        saveTopic(noKeyWord, "X");
        saveComment(comments[5]);
        
        saveTopic(topicChanger, "Z");
        saveComment(comments[6]);
        
        saveTopic(inputRepeat, "W");
        saveComment(comments[7]);
        
        saveKeyWords();
        
        scriptfile.flush();
        scriptfile.close();
    }
    saveUnknownSentences(_unknownFile);
}

string ofxEliza::start(string _logFile){
    time_t ltime;
	time(&ltime);
	logfile.open( ofToDataPath(_logFile).c_str(), std::ios::out | std::ios::app);
	if(logfile.fail()) {
		throw std::string("can't save conversation log");
	}
	logfile << "\n\nConversation log - " << ctime(&ltime) << std::endl;
	response_list = signOn;
	select_response();
	
    if(m_sResponse.length() > 0) {
		std::cout << m_sResponse;
	}
    
	saveLog("ELIZA");
    
    return m_sResponse;
}

string ofxEliza::ask(string _inputString){
    // gets input from the user
    save_prev_input();
    m_sInput = _inputString;
    saveLog("USER");
    
    // Finds and display a response
    // to the current input of the user.
    // removes punctuation from the input
    // and do some more preprocessing
    if(m_sInput.length() > 0) {
		tok.cleanString(m_sInput, " ?!,;");
		trimRight(m_sInput, '.');
		UpperCase(m_sInput);
		m_sInput.insert(0, " ");
		m_sInput.append(" ");
	}
    
    save_prev_responses();
    save_prev_response();
    
    if(null_input()) {
        handle_null_input();
    } else if(user_repeat()) {
        handle_user_repetition();
    } else if(short_input()) {
        handle_short_input();
    }
    else {
        reset_repeat_count();
        reset_short_input_count();
        find_response();
    }
    
    select_response();
    preProcessResponse();
    handle_repetition();
    
    saveLog("ELIZA");
    
    return m_sResponse;
}

void ofxEliza::preProcessResponse() {
	if(is_template(m_sResponse)) {
		findSymbol(m_sResponse);
		if(m_sKeyWord == m_sInput) {
			m_sSuffix = m_sInput;
		} else if(!m_bMemoryRecall){
			extract_suffix();
		} else {
			m_bMemoryRecall = 0;
		}
		if(m_sSuffix.length() == 0) {
			while(is_template(m_sResponse) && response_list.size() > 1) {
				response_list.erase(response_list.begin());
				m_sResponse = response_list[0];
			}
			if(is_template(m_sResponse)) {
				response_list = topicChanger;
				select_response();
			}
		}
		if(m_sSuffix.length() > 0 && m_sSymbol != "%") {
			transpose_sentence(m_sSuffix);
			correct_sentence(m_sSuffix);
			trimRight(m_sSuffix, ' ');
			m_sSuffix.insert(0, " ");
		}
		replace(m_sResponse, m_sSymbol, m_sSuffix);
	}
	if(m_sUserName != "USER") {
		replace(m_sResponse, "USER", m_sUserName);
	}
}

bool ofxEliza::similar_response() {
	int len = m_sResponse.length();
	int len2 = m_sPrevResponse.length();
	int repeat_count = 0;
	for(int i = 0; i < len && i < len2; ++i) {
		if(m_sResponse[i] == m_sPrevResponse[i]) {
			++repeat_count;
		} else {
			break;
		}
	}
	return repeat_count > 1;
}

inline bool ofxEliza::bot_repeat() {
	int pos = rfind(vResponseLog, m_sResponse);
	if(pos != -1) {
		return (pos + 1 < response_list.size());
	}
	return 0;
}

// select responses randomly from a list of responses
void ofxEliza::select_response() {
	shuffle(response_list, response_list.size());
	m_sResponse = response_list[0];
}

void ofxEliza::add_response(vstring v) {
	int size = v.size();
	for(int i = 0; i < size; ++i) {
		response_list.push_back(v[i]);
	}
}

void ofxEliza::save_user_name() {
	if(m_sKeyWord == "MY NAME IS" || m_sKeyWord == "YOU CAN CALL ME") {
        extract_suffix();
        m_sUserName = m_sSuffix;
        trimLR(m_sUserName, " ");
	}
}

void ofxEliza::clear() {
	current_data.contexts.clear();
	current_data.keywords.clear();
	current_data.responses.clear();
	current_data.cmd.clear();
}

void ofxEliza::dump_data() {
	if(current_data.keywords.size() > 0) {
		database.push_back(current_data);
		clear();
	}
}

inline void ofxEliza::extract_suffix() {
	int pos = m_sInput.find(m_sKeyWord);
	m_sSuffix.erase();
	if(pos != std::string::npos) {
		if(m_sSymbol == "*") {
			pos += m_sKeyWord.length();
			m_sSuffix = m_sInput.substr(pos, m_sInput.length() - pos);
		} else if(m_sSymbol == "@") {
			m_sSuffix = m_sInput.substr(pos, m_sInput.length());
		} else if(m_sSymbol == "%") {
			m_sSuffix = m_sPrevResponse;
		}
		trimLR(m_sSuffix, " ");
	}
}

void ofxEliza::findSymbol(std::string str) {
	m_sSymbol.erase();
	if(str.find("*") != std::string::npos) {
		m_sSymbol = "*";
	} else if(str.find("@") != std::string::npos) {
		m_sSymbol = "@";
	} else if(str.find("%") != std::string::npos) {
		m_sSymbol = "%";
	}
}

// helper function for the procedure "find_keyword"
inline bool ofxEliza::isGoodKey(const std::string key, const std::string bkey, int pos, int bestp) const{
	if(pos != std::string::npos && (key.length() > bkey.length() ||
                                    (pos != bestp && key.length() == bkey.length()))) {
		return 1;
	}
	return 0;
}

// function for finding the best keyword from the database
// this function also handles context and also verifies the
// boundaries of keywords that are found inside the user input
void ofxEliza::find_keyword() {
	response_list.clear();
	std::string bestKey;
	bool bGeneralKey = 0;
	int bestPos = -1;
	int dbase_size = database.size();
	for(int index = 0; index < dbase_size; ++index) {
		verify_context(database[index].contexts);
		if(good_context()) {
			keyword_list = database[index].keywords;
			size_t keyNum = keyword_list.size();
			for(int i = 0; i < keyNum; ++i) {
				std::string thisKey = keyword_list[i];
				std::string keyword = thisKey;
				trimLR(keyword, "_");
				keyword.insert(0, " ");
				keyword.append(" ");
				if(thisKey == "*" && bestKey == "") {
					keyword = m_sInput;
					bGeneralKey = 1;
				}
				int thisPos = m_sInput.find(keyword);
				if(thisPos == std::string::npos) {
					continue;
				}
				verify_keyword_boundary(thisKey, thisPos);
				if(wrong_boundary()) {
					continue;
				}
				if(bGeneralKey) {
					thisKey = "*";
					bGeneralKey = 0;
				}
				trimLR(thisKey, "_");
				if(isGoodKey(thisKey, bestKey, thisPos, bestPos)) {
					if(thisKey.length() > bestKey.length()) {
						response_list = database[index].responses;
					} else {
						add_response(database[index].responses);
					}
					bestKey = thisKey;
					bestPos = thisPos;
				}
			}
		}
	}
	bestKey == "*" ? m_sKeyWord = m_sInput : m_sKeyWord = bestKey;
}

// handle the context of the conversation
// (the previous replies of the chatterbot)
void ofxEliza::verify_context(vstring vContext) {
    
	m_bGoodContext = 0;
	
	size_t nNum = vContext.size();
    
	if(nNum == 0) {
		m_bGoodContext = 1;
		return;
	}
	
	for(int i = 0; i < nNum; ++i) {
		if(is_template(vContext[i])) {
			findSymbol(vContext[i]);
			replace(vContext[i], m_sSymbol, m_sSuffix);
		}
        
		if(m_sPrevResponse == vContext[i]) {
			m_bGoodContext = 1;
			break;
		}
	}
}

void ofxEliza::verify_keyword_boundary(std::string keyWord, int pos) {
	bool bFrontWordsAloud = 0;
	bool bBackWordsAloud = 0;
    
	m_bWrongBoundary = 0;
	
	if(keyWord[0] != '_') {
		bFrontWordsAloud = 1;
	} else {
		trimLeft(keyWord, '_');
	}
	
	char lastChar = keyWord[keyWord.length()-1];
	if(lastChar != '_') {
		bBackWordsAloud = 1;
	} else {
		trimRight(keyWord, '_');
	}
    
	keyWord.insert(0, " ");
	keyWord.append(" ");
	
	if(!bFrontWordsAloud && pos > 0) {
		m_bWrongBoundary = 1;
	}
    
	else if(!bBackWordsAloud && (pos + keyWord.length()) != m_sInput.length()) {
		m_bWrongBoundary = 1;
	}
    
	else if(bBackWordsAloud && (pos + keyWord.length()) == m_sInput.length()) {
		m_bWrongBoundary = 1;
	}
}

void ofxEliza::find_response() {
	find_keyword();
	std::string tempStr = m_sKeyWord;
	tempStr.insert(0, " ");
	tempStr.append(" ");
	if(tempStr.find(" MY ") != std::string::npos ||
       tempStr.find(" I'M ") != std::string::npos ||
       tempStr.find(" I ") != std::string::npos) {
		m_sSymbol = "@";
        extract_suffix();
        memory.push(m_sSuffix);
	}
	save_user_name();
	if(!bot_understand()) {
		handle_unknown_sentence();
	}
}

// this function makes sure that the bot doesn't
// repeats itself too much during the conversation
void ofxEliza::handle_repetition() {
	while(bot_is_repeating()) {
		if(response_list.size() > 1) {
			response_list.erase(response_list.begin());
		}
		else {
			break;
		}
		select_response();
		preProcessResponse();
	}
	if(bot_is_repeating()) {
		if(memory.size() > 0) {
			remind_prev_subject();
		} else {
			response_list = topicChanger;
		}
		select_response();
		preProcessResponse();
	}
	vResponseLog.push_back(m_sResponse);
}

void ofxEliza::handle_user_repetition() {
	++m_nUserRepeatCount;
	if(m_nUserRepeatCount > 1) {
		response_list = inputRepeat;
	} else {
		find_response();
	}
}

void ofxEliza::handle_short_input() {
	++m_nShortInputCount;
	if(m_nShortInputCount > 2) {
		response_list = shortInput;
	} else {
		find_response();
	}
}

void ofxEliza::handle_unknown_sentence() {
	unknownSentences.push_back(m_sInput);
	if(memory.size() > 0) {
		remind_prev_subject();
	} else {
		response_list = noKeyWord;
	}
}

void ofxEliza::handle_null_input() {
	response_list = nullResponse;
}

void ofxEliza::remind_prev_subject() {
	response_list = subjectRecall;
	m_sSuffix = memory.top();
	m_bMemoryRecall = 1;
	memory.pop();
}

// transposes a word
bool ofxEliza::transpose( std::string &str ) {
	std::string _str = " " + str + " ";
	std::string backup = str;
	std::string verbP1 = "";
	std::string verbP2 = "";
	for(int i = 0; i < m_nTransPosNum; ++i) {
		verbP1 = transpos_list[i].verbP1;
		verbP2 = transpos_list[i].verbP2;
		if(replace(_str, verbP1, verbP2)) {
			break;
		} else if(replace(_str, verbP2, verbP1)) {
			break;
		}
	}
	str = _str.substr(1, _str.length() - 2);
	return backup != str;
}

// transpose a complete sentence
void ofxEliza::transpose_sentence( std::string &str ) {
	std::string thisWord = "";
	std::string tempStr = "";
	vstring stringToken;
	tok.setDelim(" !?,-");
	trimRight(str, '.');
	tok.tokenize(str, stringToken);
	int nTokNum = stringToken.size();
	for(int i = 0; i < nTokNum; ++i) {
		thisWord = stringToken[i];
		transpose(thisWord);
		tempStr += thisWord + " ";
	}
	str = tempStr;
}

// corrects the current sentence
void ofxEliza::correct_sentence( std::string &str ) {
	std::string string1 = "";
	std::string string2 = "";
	std::string _str = " " + str + " ";
	for(int i = 0; i < m_nCorrectionNum; ++i) {
		string1 = correction_list[i].verbP1;
		string2 = correction_list[i].verbP2;
		while(replace(_str, string1, string2))
			;
	}
	str = _str.substr(1, _str.length() - 3);
}

void ofxEliza::print_current_data() {
	std::cout << "Current data:\n";
	std::cout << "Keywords,Contexts and responses:\n";
	int nKeyNum = current_data.keywords.size();
	for(int i = 0; i < nKeyNum; ++i) {
		std::cout << "   " << current_data.keywords[i] << std::endl;
		int nContextNum = current_data.contexts.size();
		for(int k = 0; k < nContextNum; ++k) {
			std::cout << "       " << current_data.contexts[k] << std::endl;
		}
		int nResponseNum = current_data.responses.size();
		for(int j = 0; j < nResponseNum; ++j) {
			std::cout << "      " << current_data.responses[j] << std::endl;
		}
	}
	std::cout << std::endl;
}

// with these specific feature of the program,to make new updates to the
// script file,you just need to look at the "unknown.txt" file to search
// for new keyword that can be added to uptadate the chatterbot vocabulary
void ofxEliza::saveUnknownSentences(string _unknownFile) {
	size_t nSize = unknownSentences.size();
	if(nSize > 0) {
		std::fstream outfile( ofToDataPath(_unknownFile).c_str() , std::ios::out | std::ios::app);
		if(outfile.fail()) {
			throw std::string("can't save unknown sentences");
		}
		for(int i = 0; i < nSize; ++i) {
			outfile << unknownSentences[i] << std::endl;
		}
		outfile.flush();
		outfile.close();
	}
}




void ofxEliza::saveTopic(const vstring vList, const std::string sSymbol) {
	int listSize = vList.size();
	for(int i = 0; i < listSize; ++i) {
		scriptfile << sSymbol << vList[i] << std::endl;
	}
}

void ofxEliza::saveComment(const std::string comment) {
	vstring vComment;
	vComment.push_back(comment);
	saveTopic(vComment, "");
}

void ofxEliza::saveTransposTable() {
	for(int i = 0; i < m_nTransPosNum; ++i) {
		scriptfile << "T" << transpos_list[i].verbP1 << "\"" << std::endl;
		scriptfile << "T" << transpos_list[i].verbP2 << "\"" << std::endl;
	}
}

void ofxEliza::saveCorrections() {
	for(int i = 0; i < m_nCorrectionNum; ++i) {
		scriptfile << "E" << correction_list[i].verbP1 << "\"" << std::endl;
		scriptfile << "E" << correction_list[i].verbP2 << "\"" << std::endl;
	}
}

void ofxEliza::saveKeyWords() {
	vstring keyWords, responses;
	vstring contexts, commands;
	int numOfKeyword, numOfResponse;
	int numOfContext, numOfCommand;
	int nKeyWordsSize = database.size();
	for(int i = 0; i < nKeyWordsSize; ++i) {
		keyWords = database[i].keywords;
		contexts = database[i].contexts;
		responses = database[i].responses;
		commands = database[i].cmd;
		numOfKeyword = keyWords.size();
		numOfContext = contexts.size();
		numOfResponse = responses.size();
		numOfCommand = commands.size();
		for(int j = 0; j < numOfKeyword; ++j) {
			scriptfile << "K" << keyWords[j] << "\"" << std::endl;
		}
		for(int p = 0; p < numOfContext; ++p) {
			scriptfile << "C" << contexts[p] << std::endl;
		}
		for(int k = 0; k < numOfResponse; ++k) {
			scriptfile << "R" << responses[k] << std::endl;
		}
		for(int q = 0; q < numOfCommand; ++q) {
			scriptfile << "A" << commands[q] << std::endl;
		}
		scriptfile << ";" << std::endl;
	}
}

void ofxEliza::saveLog(std::string str) {
	if(str == "ELIZA") {
		logfile << m_sResponse << std::endl;
	} else if(str == "USER") {
		logfile << ">" << m_sInput << std::endl;
	}
}