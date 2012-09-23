#ofxEliza
   
These program is an Eliza like chatterbot,bots like Eliza are the results of researchs in Artificial Intelligence (more specificly: in NLP: Natural Language Processing and NLU: Natural Language Understanding ) develop by [Gonzales Cenelia]( http://www.planetsourcecode.com/vb/scripts/ShowCode.asp?txtCodeId=73024&lngWId=1 ) and adapted as openframeworks addon by [Patricio Gonzalez Vivo](www.patriciogonzalezvivo.com)

The first chatterbot was published in 1966 by Joseph Weizenbaum a professor of MIT. (http://i5.nyu.edu/~mm64/x52.9265/january1966.html)
And also,most of the chatterbots that have been written this days are largely based on the original chatterbot Eliza that was written by Joseph Weizenbaum which means that they use some appropriate keywords to select the responses to generate when they get new inputs from the users. More generaly,the technique that are in use in a "chatterbot database" or "script file" to represent the chatterbot knowledge is known as "Case Base Reasoning" or CBR. A very good example of an Eliza like chatterbot would be "Alice",these program has won the Loebner prize for most human chatterbot three times (www.alicebot.org). The goal of NLP and NLU is to create programs that are capable of understanding natural languages and also capable of processing it to get input from the user by "voice recognition" or to produce output by "text to speech". During the last decades there has been a lot of progress in the domains of "Voice Recognition" and "Text to Speech",however the goal of NLU that is to make software that are capable of showing a good level of understanding of "natural languages" in general seems quiet far to many A.I experts. The general view about this subject is that it would take at list many decades before any computer can begin to really understand "natural language" just as the humans do.
These code is Copyrighted and has limited warranty.
	
##Features of the Program
There are many interesting features in the current program:
Features:

- these chatterbot is capable of avoiding repetitions when selecting new responses 
- conversation log between the users and the chatbot is automaticaly saved into the file: log.txt
- use a "script file" (script.txt) wich acts as a knowledge base for the chatbot 
- it can also follow the context of a conversation with a user. 
- When the programs detect new keywords that are not part of the "script file", it saves it on the file "unknown.txt"  