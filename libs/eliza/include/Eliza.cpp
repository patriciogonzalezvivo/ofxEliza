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
	
#include "eliza.h"

// gets input from the user
void Eliza::get_input() {
	save_prev_input();
	std::getline(std::cin, m_sInput);
	save_log("USER");
}

// removes punctuation from the input
// and do some more preprocessing
void Eliza::preProcessInput() {
	if(m_sInput.length() > 0) {
		tok.cleanString(m_sInput, " ?!,;");
		trimRight(m_sInput, '.');
		UpperCase(m_sInput);
		m_sInput.insert(0, " ");
		m_sInput.append(" ");
	}
}

void Eliza::preProcessResponse() {
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

void Eliza::memorise_input() {
	m_sSymbol = "@";
	extract_suffix();
	memory.push(m_sSuffix);
}

bool Eliza::similar_response() const {
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

inline bool Eliza::bot_repeat() const {
	int pos = rfind(vResponseLog, m_sResponse);
	if(pos != -1) {
		return (pos + 1 < response_list.size());
	}
	return 0;
}

void Eliza::start() {
	time_t ltime; 
	time(&ltime);
	logfile.open("log.txt", std::ios::out | std::ios::app);
	if(logfile.fail()) {
		throw std::string("can't save conversation log");
	}
	logfile << "\n\nConversation log - " << ctime(&ltime) << std::endl;
	response_list = signOn;
	select_response();
	print_response();
	save_log("ELIZA");
}

// prints the bot response on the screen
void Eliza::print_response() {
	if(m_sResponse.length() > 0) {
		std::cout << m_sResponse;
	}
}

// select responses randomly from a list of responses
inline void Eliza::select_response() {
	shuffle(response_list, response_list.size());
	m_sResponse = response_list[0];
}

void Eliza::add_response(vstring v) {
	int size = v.size();
	for(int i = 0; i < size; ++i) {
		response_list.push_back(v[i]);
	}
}

void Eliza::save_user_name() {
	if(m_sKeyWord == "MY NAME IS" || m_sKeyWord == "YOU CAN CALL ME") {
		 extract_suffix();
		 m_sUserName = m_sSuffix;
		 trimLR(m_sUserName, " ");
	}
}

void Eliza::clear() {
	current_data.contexts.clear();
	current_data.keywords.clear();
	current_data.responses.clear();
	current_data.cmd.clear();
}

inline void Eliza::dump_data() {
	if(current_data.keywords.size() > 0) {
		database.push_back(current_data);
		clear();
	}
}

inline void Eliza::extract_suffix() {
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

void Eliza::findSymbol(std::string str) {
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
inline bool Eliza::isGoodKey(const std::string key, const std::string bkey, int pos, int bestp) const{
	if(pos != std::string::npos && (key.length() > bkey.length() || 
		(pos != bestp && key.length() == bkey.length()))) {
		return 1;
	}
	return 0;
}

// these function finds and display a response 
// to the current input of the user.
void Eliza::respond() {
	preProcessInput();

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
	check_quit_message();
	handle_repetition();
	
//	print_response();
	save_log("ELIZA");
}

// function for finding the best keyword from the database
// this function also handles context and also verifies the
// boundaries of keywords that are found inside the user input
void Eliza::find_keyword() {
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
void Eliza::verify_context(vstring vContext) { 

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

void Eliza::verify_keyword_boundary(std::string keyWord, int pos) {
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

void Eliza::find_response() {
	find_keyword();
	std::string tempStr = m_sKeyWord;
	tempStr.insert(0, " ");
	tempStr.append(" ");
	if(tempStr.find(" MY ") != std::string::npos ||
		tempStr.find(" I'M ") != std::string::npos ||
		tempStr.find(" I ") != std::string::npos) {
		memorise_input();
	}
	save_user_name();
	if(!bot_understand()) {
		handle_unknown_sentence();
	}
}

// this function makes sure that the bot doesn't
// repeats itself too much during the conversation
inline void Eliza::handle_repetition() {
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

void Eliza::handle_user_repetition() {
	++m_nUserRepeatCount;
	if(m_nUserRepeatCount > 1) {
		response_list = inputRepeat;
	} else {
		find_response();
	}
}

void Eliza::handle_short_input() {
	++m_nShortInputCount;
	if(m_nShortInputCount > 2) {
		response_list = shortInput;
	} else {
		find_response();
	}
}

void Eliza::handle_unknown_sentence() {
	unknownSentences.push_back(m_sInput);
	if(memory.size() > 0) {
		remind_prev_subject();
	} else {
		response_list = noKeyWord;
	}
}

void Eliza::handle_null_input() {
	response_list = nullResponse;
}

void Eliza::remind_prev_subject() {
	response_list = subjectRecall;
	m_sSuffix = memory.top();
	m_bMemoryRecall = 1;
	memory.pop();
}

// transposes a word
bool Eliza::transpose( std::string &str ) {
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
void Eliza::transpose_sentence( std::string &str ) {
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
void Eliza::correct_sentence( std::string &str ) {
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

void Eliza::print_current_data() {
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

// prints database content on the screen
// for debugging purpose
void Eliza::print_database_content() {
	int nDatabaseSize = database.size();
	for(int i = 0; i < nDatabaseSize; ++i) {
		current_data = database[i];
		print_current_data();
	}
}

// loading database into memory
void Eliza::load_data() {
	std::fstream fin( ofToDataPath("script.txt").c_str() , std::ios::in);
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

// with these specific feature of the program,to make new updates to the
// script file,you just need to look at the "unknown.txt" file to search
// for new keyword that can be added to uptadate the chatterbot vocabulary
void Eliza::save_unknown_sentences() {
	size_t nSize = unknownSentences.size();
	if(nSize > 0) {
		std::fstream outfile( ofToDataPath("unknown.txt").c_str() , std::ios::out | std::ios::app);
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

// the functions below can be used to save
// the content of the dabase to the script file.
void Eliza::save_data() {
	scriptfile.open( ofToDataPath("script.txt").c_str(), std::ios::out);
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

void Eliza::saveTopic(const vstring vList, const std::string sSymbol) {
	int listSize = vList.size();
	for(int i = 0; i < listSize; ++i) {
		scriptfile << sSymbol << vList[i] << std::endl;
	}
}

void Eliza::saveComment(const std::string comment) {
	vstring vComment;
	vComment.push_back(comment);
	saveTopic(vComment, "");
}

void Eliza::saveTransposTable() {
	for(int i = 0; i < m_nTransPosNum; ++i) {
		scriptfile << "T" << transpos_list[i].verbP1 << "\"" << std::endl;
		scriptfile << "T" << transpos_list[i].verbP2 << "\"" << std::endl;
	}
}

void Eliza::saveCorrections() {
	for(int i = 0; i < m_nCorrectionNum; ++i) {
		scriptfile << "E" << correction_list[i].verbP1 << "\"" << std::endl;
		scriptfile << "E" << correction_list[i].verbP2 << "\"" << std::endl;
	}
}

void Eliza::saveKeyWords() {
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

void Eliza::save_log(std::string str) {
	if(str == "ELIZA") {
		logfile << m_sResponse << std::endl;
	} else if(str == "USER") {
		logfile << ">" << m_sInput << std::endl;
	}
}