/*

Julian Day 0 = Monday, January 1, 4713 BC, proleptic Julian calendar

char *ctime(const time_t *time);
This returns a pointer to a string of the form day month year hours:minutes:seconds year\n\0.

* Idea to keep extra features hidden from stdout and instead written in the readme 

* Probably need to store date info in case exported to file, wait a day, then import to program. Probably means we need timestamp of file modification

* Maybe export to file occasionally -> seems like a bad idea, just interact when the user is requesting, but could be explored

(#) Need to convert priority type if priority was updated, but sort was not requested

Make option to insert at specific spot
Separate things which are separated by newlines or make option to organize things with number priority. Give everything a sorting priority based on block it is located in
Swap option using line numbers?
Need to check against bad inputs - letters other than 'd' for deleting delete 0th item? numbers greater than size of arr give segfault 
Options to return to main menu

(#) Needs to take white spaces in input task descriptions, 
(#) Importing and exporting is adding new linebreaks 7 newline difference
(#) Make option to move
(#) Spacing seems off when displaying line numbers
(#) Needs to deal with ?() case -> Just add a prefix to everything
(#) Needs to have option to add a prefix to something
(#) Needs to correctly print dates when showing item numbers
(#) Needs to be able to import file
(#) Priorities other than numbers (ASAP) etc
(#) Need to be able to have priorities that contain a number somewhere (assume there is only 1)
(#) Edit priorities option
(#) Take timestamp from when user makes a relevant request (to add, display or update)
(#) Needs file to store tasks, 
(#) Needs to update days remaining on display 
(#) Make option to display line numbers for editing
(#) If there is newlines, and we haven't seen anything relevant yet, ignore it
(#) Do not input newlines before header into file ? currently doing this
*/

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>
#include <ctime>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;

struct widget_t {
	double x;
	double y;
	int count;
};

class Element
{
	public:

	string taskDescr;
	int numDays;
	bool hasNumPriority; // this means update() will subtract from numDays
	bool hasCharNumPriority; // ^# case, should also have numPriority. Special case of numPriority
	bool hasStringNumPriority; // ^#^ case will have this, numPriority, and not CharNumPriority
	bool newLine;
	bool isHeader;
	string priority;
	int breakPos;
	string prefix;

	// nonintuitive, but can be updated later
	Element() // by default, assume everything has pure number priority
	{
		hasNumPriority = true;
		hasCharNumPriority = false;
		hasStringNumPriority = false;
		prefix = "";
		newLine = false;
		isHeader = false;
	}

	Element(double d)
	{
		newLine = true;
		isHeader = false;
		hasNumPriority = false;
	}

	Element(string s) // ^# case. Will print out the priority description first, then the number part of the priority
	{
		hasNumPriority = true;
		hasCharNumPriority = true;
		hasStringNumPriority = false;
		prefix = "";
		newLine = false;
		isHeader = false;
	}

	Element(int a) // pure string case
	{
		hasNumPriority = false;
		hasCharNumPriority = false;
		hasStringNumPriority = false;
		prefix = "";
		newLine = false;
		isHeader = false;
	}

	Element(bool b, int breakPos) // ^#^ case
	{
		hasNumPriority = true;
		hasCharNumPriority = false;
		hasStringNumPriority = true;
		this->breakPos = breakPos;
		prefix = "";
		newLine = false;
		isHeader = false;
	}

	void convertPureInt()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = false;
		this->isHeader = false;
		this->newLine = false;
	} // convertPureInt()

	void convertPureString()
	{
		this->hasNumPriority = false;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = false;	
		this->isHeader = false;
		this->newLine = false;
	} // convertPureString()

	void convertCharNum()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = true;
		this->hasStringNumPriority = false;
		this->isHeader = false;
		this->newLine = false;
	} // convertCharNum()

	void convertStringNum()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = true;
		this->isHeader = false;
		this->newLine = false;
	} // convertStringNum()

	bool isNewLine()
	{
		return this->newLine;
	}

	bool isHeaderFun()
	{
		return this->isHeader;
	}

	void print()
	{
		if(this->newLine)
		{
			cout << "\n";
		}
		else if(this->isHeader)
		{
			cout << this->taskDescr << endl;
		}
		else if(!hasNumPriority && !hasCharNumPriority && !hasStringNumPriority) // pure string case
		{
			cout << this->prefix << "(" << this->priority << ") " << this->taskDescr << endl;
			return;
		}
		else if(hasNumPriority && !hasCharNumPriority && !hasStringNumPriority) // pure int case
		{
			cout << this->prefix << "(" << this->numDays << ") " << this->taskDescr << endl;
			return;
		}
		else if(hasNumPriority && hasCharNumPriority && !hasStringNumPriority)
		{
			cout << this->prefix << "(" << this->priority << this->numDays << ") " << this->taskDescr << endl;
			return;
		}
		else
			cout << this->prefix << "(" << (this->priority).substr(0,this->breakPos) << this->numDays << (this->priority).substr(this->breakPos) << ") " << this->taskDescr << endl;


	} // print()
}; // Class Element

int computejdn(int day, int month, int year)
{
	int a = floor((14 - month) / 12);
    int y = year + 4800 - a;
    int m = month + (12*a) - 3;
    return (day + floor( ( (153 * m) + 2) / 5 ) + (365*y) + floor(y/4) -
		  				floor(y/100) + floor(y/400) - 32045);

} // computejdn()

vector<Element> arr;

// subtracts diff from all integer priorities, returns the diff
int updateSched(int diff)
{
	for(int i = 0; i < arr.size(); i++)
	{
		if(arr[i].hasNumPriority)
			arr[i].numDays -= diff;
	} // for
	return diff;
} // updateSched()

// https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool isNumber(string s)
{
	if(s.find("00") != std::string::npos)
		return false;
	std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
} // isNumber()

Element *ep;
Element e;

// returns a number if one is found in string s
int numberFound(string s)
{
	int i;
	bool numberFound = false;
	string temp = "";
	int pos;
	for(i = 0; i < s.length(); i++)
	{
		if(isNumber(s.substr(i,1)))
		{
			numberFound = true;
			//pos = i;
			break;
		}
	} // for

	if(numberFound)
	{
		while(i < s.length() && isNumber(s.substr(i,1)))
		{
			temp += s.substr(i,1);
			i++;
		} // while
		if(temp.find("00") != std::string::npos || s.find("-0") != std::string::npos) // if 00 or -0 is found, do not treat this as a valid int
			return -1;
		/*if(pos-1 >= 0 && s.substr(pos-1,1) == "-")
		{
			temp = "-" + temp;
		}*/
		stringstream geek(temp); // convert string to int
		geek >> i;
		//cout << i << endl;
		return i;
	} // if

	return -1;
} // numberFound()

void importFile(string fileName)
{
	string inputLine;
	ifstream infile;
	//infile.open ("scheddata.txt");
	infile.open(fileName);
	int pos;
	int newLineCtr = 0;
	bool headerSeen = false;
	bool relevantElementSeen = false;
     while(!(infile.eof()))
     {
	    getline(infile,inputLine);
	    //if(infile.eof())
	    //	break;
	    cout << inputLine << endl;

	    if(relevantElementSeen && inputLine.find("\n") && inputLine.length() == 0) // a line just containing a new line, we want to keep formatting
	    {
	    	newLineCtr++;
	    	ep = new Element(2.0);
	    	e = *ep;
	    	arr.push_back(e);
	    } // if

	    else if(inputLine.substr(0,1).compare("~") == 0)
	    {
	    	relevantElementSeen = true;
	    	if(!headerSeen)
	    	{
	    		//cout << newLineCtr << " New lines seen before first header" << endl;
	    		headerSeen = true;
	    	}
	  
	    	ep = new Element();
	    	e = *ep;
	    	e.isHeader = true;
	    	e.hasNumPriority = false;
	    	e.taskDescr = inputLine;
	    	arr.push_back(e);
	    }

	    else if((pos = inputLine.find("(")) != std::string::npos) //  we are at the first important line of the file
	    {	// need to see if priority is an integer, make a new element object, and assign priority and description. Note that items will be sorted.
	      	// could also take care of case where items are not sorted
	      	// need to take care of ^# case
	      	
	    	relevantElementSeen = true;

	      	string temp;
	      	string prefix = inputLine.substr(0,pos);
	      	int j = pos + 1;
	      	bool isNumberPriority = true; // default, assume the full priority is a pure number
		 	
		 	while(inputLine.substr(j,1).compare(")") != 0) // give temp the value of the string inside (...) AKA the priority
		  	{
		    	temp += inputLine.substr(j,1); 
				j++;
		  	} // while

		  	if(temp == "" || !isNumber(temp)) // check if priority is a pure number
		    	isNumberPriority = false;

		  	if(isNumberPriority) // if priority is a pure number
		  	{
		  		stringstream geek(temp); // convert string to int
		  		int x;						 // ""
				geek >> x;
				// create new element with this priority and the description found in the rest of inputLine	
				ep = new Element();
				e = *ep;
				e.taskDescr = inputLine.substr(j+2); // +2 because it is on ')', and the next char is a space
				e.numDays = x;
				//cout << "pushback" << endl;
				//arr.push_back(e);
		  	} // if
		  	
		  	else if(temp != "" && isNumber(temp.substr(1,temp.length() - 1)) && temp != "-0") // ^# case. Exclude -0
		  	{
		  		// create new element with ^# priority and the description found in the rest of inputline
		  		if(temp.substr(0,1) == "-") // we have a pure negative number
		  		{
		  			stringstream geek(temp);
		  			int y;
		  			geek >> y;
		  			ep = new Element();
		  			e = *ep;
		  			e.taskDescr = inputLine.substr(j+2);
		  			e.numDays = y;
		  		}
		  		
		  		else
		  		{
		  			stringstream geek(temp.substr(1,temp.length() - 1));
			  		int x;
			  		geek >> x;
			  		ep = new Element("s");
			  		e = *ep;
			  		e.taskDescr = inputLine.substr(j+2);
			  		e.numDays = x;
			  		e.priority = temp.substr(0,1);
			  		//cout << "pushback1" << endl;
			  		//arr.push_back(e);
		  		}
		  		
		  	} // else if

		  	else if(temp != "" && (numberFound(temp) + 1)) // ^#^ case. Number found somewhere in priority, not ^# case. NumberFound returns -1 if no number found, +1 is 0 = false, quick maffs. We also want 0 to count as finding a number
		  	{
		  		//cout << temp << endl;
		  		char strDaysTemp[2000];
		  		string strDays;
		  		int pos;
		  		int digits;
		  		ep = new Element(true, 0);
		  		e = *ep;
		  		e.taskDescr = inputLine.substr(j+2);
		  		e.numDays = numberFound(temp);
		  		sprintf(strDaysTemp, "%d", e.numDays); // convert int to string
		    	strDays = (string)strDaysTemp; 
		  		pos = temp.find(strDays); // find the position of the integer in the priority string
		  		digits = floor(log10(e.numDays)) + 1;
		  		e.priority = temp.erase(pos,digits); // delete integer in the string
		  		//cout << temp << endl;
		  		e.breakPos = pos;					// position needs to be saved, so we can print it correctly later
		  		//cout << "pushback2" << endl;
		  		//arr.push_back(e);
		  		//cout << arr.size() << endl;
		  		//cout << pos << endl;
		  	} // else if

		  	else
		  	{ 
		  		// create new element with other priority and the description found in the rest of inputLine
		  		ep = new Element(1);
		  		e = *ep;
		  		e.taskDescr = inputLine.substr(j+2);
		  		e.priority = temp;
		  		//cout << "pushback3" << endl;
		  		//arr.push_back(e);
		  	} // else
		  	
		  	e.prefix = prefix;
		  	arr.push_back(e);

		} // if
     } // while  	
	infile.close();
	//cout << "Counted " << newLineCtr << " new lines" << endl;
	cout << "Finished file import" << endl;
} // importFile

time_t now; 
tm *ltm;
int dd;
int mm;
int yyyy;
int currentDay;
int currentMonth;
int currentYear;

// Displays the item numbers
void displayItemNumbers()
{
	// need to update the number of days each time its printed if the day has changed
	now = time(0);                                        // get current time
	ltm = localtime(&now); // How exactly does this work?
	dd = ltm->tm_mday;
	mm = ltm->tm_mon + 1;
	yyyy = ltm->tm_year + 1900;

	if(currentDay != dd || currentMonth != mm || currentYear != yyyy)
	{
		updateSched(computejdn(dd, mm, yyyy) - computejdn(currentDay, currentMonth, currentYear));
		currentDay = dd;
		currentMonth = mm;
		currentYear = yyyy;
	}

	cout << "\n\n\n\n" << "*******************SCHEDULE*************************\n\n" << endl;
	int i;
	for(i = 0; i < arr.size(); i++)
	{
		cout << i << ": "; // print number, then call print function
		arr[i].print();
	}
	cout << "\n\n\n\n\n\n\n\n" << endl;
	//cout << i << endl;
	//cout << "The last element in the array is "; 
	//arr[i-1].print();
	//cout << arr[i-1].isNewLine() << endl;
} // displayItemNumbers()

void addCharNumElement(string priority, int dueDate, string descr)
{

} // addCharNumElement()

// Currently adds a pure string element to the top of the schedule
void addCharElement(string priority, string descr) 
{
	ep = new Element(1);
	e = *ep;
	e.priority = priority;
	e.taskDescr = descr;
	arr.insert(arr.begin(), e);
} // addCharElement()

// Update everything, then insert, respecting order of numbers
void insertNumPriority(Element e)
{
	int inserted = 0;
	if(!arr.empty())
	{
		for(int i = 0; i < arr.size(); i++)
		{
			if(arr[i].hasNumPriority && arr[i].numDays > e.numDays) // if tempdays casted to an int is bigger than numDays, place element here (before)
			{	
				arr.insert(arr.begin() + i, e);// place here and move everything else back one
				inserted = 1;
				break;
			}
		}
	}
	else
	{
		arr.push_back(e);
		inserted = 1;
	}
	if(inserted == 0)
		arr.push_back(e);
}

void refreshSchedule()
{
	now = time(0);                                        // get current time
	ltm = localtime(&now); 
	dd = ltm->tm_mday;		
	mm = ltm->tm_mon + 1;
	yyyy = ltm->tm_year + 1900;

	if(currentDay != dd || currentMonth != mm || currentYear != yyyy)
	{
		updateSched(computejdn(dd, mm, yyyy) - computejdn(currentDay, currentMonth, currentYear));
		currentDay = dd;
		currentMonth = mm;
		currentYear = yyyy;
	} // if
} // refreshSchedule()


int main(int argc, char* argv[])
{
	/*cout << argv[0] << endl;
	cout << argv[1] << endl;
	cout << argc << endl;
	cout << argv[1] << endl;

	cout << (string)argv[1] << endl;*/
	bool importedFile = false;

	if(argc == 2) // if a file was provided, import it
	{
		importFile((string)argv[1]);
		importedFile = true;
	}

	//vector<string> arr; // array of user's to-do items
	// can calculate the day that this started running, then keep track of this day and update as required, save into file 
	//before shutdown. Will be a glitch between days, but when printing out, it should be fine because it will update
	//time_t now;
	//tm *ltm;
	now = time(0);                                        // get current time
	ltm = localtime(&now);
	currentDay = ltm->tm_mday;                     // These are set when main program is run
	currentMonth = ltm->tm_mon + 1;
	currentYear = ltm->tm_year + 1900;
	ofstream outputFile;
	
	while (1){ // main loop of program
	
	// do everything after this	

	int command; // store user input
	

	int dueDate;
	string dueDateStr;
	string taskDescr;
	string fullTaskDescr;
	string element;
	string dinput;
	string uinput;

	//int dd;
	//int mm;
	//int yyyy;
	int DD;
	int MM;
	int YYYY;
	string numDays;
	bool leadingzero;

	int a;
	int y;
	int m;

	int jdnstart;
	int jdnend;

	char temp[2000];

	int inserted;
	bool deleted;
	bool charnum;
	int curJulianDay;

	int updateDays;
	int itemNum;
	int newLineCtr;

	string restDescr;

	bool firstHeader;

	cout << "\nMain Menu\n1) Display Schedule\n2) Add Item\n3) Remove Item\n4) Update Item\n5) Update Schedule\n6) Add/Change Prefix\n7) Calculate Julian Difference\n8) Add or Remove Linebreak" << endl;
	cin >> command;

	switch ( command ) 
	{
		case 1:
			// printing it out (for now) will be the only time the data is written to file
			// need to update the number of days each time its printed if the day has changed
			now = time(0);                                        // get current time
		    ltm = localtime(&now); // How exactly does this work?
		    dd = ltm->tm_mday;		// lower case is set when any relevant command is called
		    mm = ltm->tm_mon + 1;
		    yyyy = ltm->tm_year + 1900;

		    if(currentDay != dd || currentMonth != mm || currentYear != yyyy)
		    {
		    	updateSched(computejdn(dd, mm, yyyy) - computejdn(currentDay, currentMonth, currentYear));
		    	currentDay = dd;
		    	currentMonth = mm;
		    	currentYear = yyyy;
		    }

		    outputFile.open("schedule.txt");
			cout << "\n\n\n\n" << "*******************SCHEDULE*************************" << "\n" << (curJulianDay = computejdn(currentDay,currentMonth, currentYear)) << "\n\n" << endl; // for the legacy
			outputFile /*<< "\n\n\n\n" */<< "*******************SCHEDULE*************************" << "\n" << curJulianDay << "\n" << endl;
			//if(!importedFile)  // We want to control how the schedule is formatted, so we don't need this right now 
			//	outputFile << "\n\n" << endl; // these two are extra newlines if file was imported

			newLineCtr = 0;
			firstHeader = false;

		    for(int i = 0; i < arr.size(); i++)
		    {
		    	if(arr[i].isNewLine())
		    	{
		    		cout << "\n";
		    		outputFile << endl;
		    		newLineCtr++;
		    		//if(!firstHeader)
		    			//cout << "Printed " << newLineCtr << " newlines before first header" << endl;
		    	} // if

		    	else if(arr[i].isHeaderFun())
		    	{
		    		cout << arr[i].taskDescr << endl;
		    		outputFile << "\n" << arr[i].taskDescr; //<< endl; // try printing newlines first to avoid having an extra line at bottom
		    		firstHeader = true;
		    	}
		    	
		    	else if(arr[i].hasStringNumPriority && arr[i].hasNumPriority) // ^#^ case
		    	{
		    		cout << arr[i].prefix << "(" << (arr[i].priority).substr(0,arr[i].breakPos) << arr[i].numDays << (arr[i].priority).substr(arr[i].breakPos) << ") " << arr[i].taskDescr << endl; // print priority, number of days, then task description in proper format
		    		outputFile << "\n" << arr[i].prefix << "(" << (arr[i].priority).substr(0,arr[i].breakPos) << arr[i].numDays << (arr[i].priority).substr(arr[i].breakPos) << ") " << arr[i].taskDescr; // << endl;
		    	} // else if

		    	else if(arr[i].hasCharNumPriority && arr[i].hasNumPriority) // ^# case  **can make this come after the current else if and only check if it has CharNumPriority
		    	{
		    		cout << arr[i].prefix << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print priority, number of days, then task description in proper format
		    		outputFile << "\n" << arr[i].prefix << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr; // << endl;
		    	} // else if

		    	else if(!arr[i].hasNumPriority)
		    	{
		    		cout << arr[i].prefix << "(" << arr[i].priority << ") " << arr[i].taskDescr << endl; //  pure string case. print priority then task description in proper format
		    		outputFile << "\n" << arr[i].prefix << "(" << arr[i].priority << ") " << arr[i].taskDescr; // << endl;
		    	} // else if

		    	else
		    	{	
		    		cout << arr[i].prefix << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl; // pure number case. print number of days, then task description in proper format
		    		outputFile << "\n" << arr[i].prefix << "(" << arr[i].numDays << ") " << arr[i].taskDescr; // << endl;
		    	} // else
		    } // for
		    cout << "\n\n\n\n\n\n\n\n" << endl;
		    //outputFile << "\n\n\n\n\n\n\n\n" << endl; dont think this is needed
		    outputFile.close();
		    //cout << "Exported " << newLineCtr << " new lines" << endl;
		    //cout << "size after the print: " << arr.size() << endl;
			break;


		case 2:

			cout << "Enter Description of Task" << endl;
			//cin >> taskDescr;
			cin >> taskDescr; // get the line up to a whitespace
			getline(cin, restDescr); // get the rest of the line
			taskDescr = taskDescr + restDescr; // concatenate together
			//std::cin.getline(cin,sizeof(taskDescr));

			cout << "Input due date of task in format DDMMYYYY or a desription of the priority" << endl;
			cin >> dueDateStr;
			getline(cin, restDescr);
			dueDateStr = dueDateStr + restDescr;

			now = time(0);                                        // get current time
		    ltm = localtime(&now); // How exactly does this work?
		    dd = ltm->tm_mday;
		    mm = ltm->tm_mon + 1;
		    yyyy = ltm->tm_year + 1900;

		    // RACE CONDITION POTENTIAL if this runs before item was inserted -> actually only the date when calculations happen is important
		    // this makes everything in terms of the date we just calculated
		    // Dates are determined by moment of user action
		    if(currentDay != dd || currentMonth != mm || currentYear != yyyy)
		    {
		    	updateSched(computejdn(dd, mm, yyyy) - computejdn(currentDay, currentMonth, currentYear));
		    	currentDay = dd;
		    	currentMonth = mm;
		    	currentYear = yyyy;
		    } // ehhh just set current times when starts, and update as you go through? if nothing entered, update doesnt do anything -> fine

		    charnum = false;
		    // dueDate is a string, check if it is a ^# or pure string case (that is, if it is not a pure number)
		    if(!isNumber(dueDateStr))
		    { 
		    	charnum = true;
		        // dueDate is not a string, so check if it is ^# case. If not, it is a pure string
		    	if(isNumber(dueDateStr.substr(1))) // ^# case
		    	{
		    		// convert dueDateStr.substr(1) into an int, assign this to a new element's numdays, assign dueDateStr[0] to e's priority, and rest to descr
		    		stringstream geek(dueDateStr.substr(1)); // convert string to int						
					geek >> dueDate;					 // ""
					//addCharNumElement(dueDateStr.substr(0,1), dueDate, taskDescr);
					//break; // breaks out of this insertion request
		    	}
		    	else // pure string case
		    	{
		    		addCharElement(dueDateStr, taskDescr);
		    		cout << "Element added\n" << endl;
		    		break;
		    	}
		    } // if

		    // convert dueDateStr to int if we didnt partially already above in the ^# case
		    if(!charnum){ 
		    stringstream geek(dueDateStr); // convert string to int
			geek >> dueDate;		
			}

		    // convert current time to julian day 
		  	jdnstart = computejdn(dd, mm, yyyy);

		  	DD = dueDate / 1000000; // uppercase is due date, calculated when provided
		    MM = (dueDate % 1000000) / 10000;
		  	YYYY = dueDate % 10000;

		  	jdnend = computejdn(DD, MM, YYYY);
		  	
		  	sprintf(temp, "%d", jdnend - jdnstart); // convert int to string
		    numDays = (string)temp;                        // temp is char*, needs to be casted to string

			element = "(" + numDays + ") " + taskDescr;

			ep = new Element();
			e = *ep;
			e.taskDescr = taskDescr;
			e.numDays = jdnend - jdnstart;
			if(charnum) // ^# case
			{
				e.priority = dueDateStr.substr(0,1);
				e.hasCharNumPriority = true;
			}

			//arr.push_back(element);
			inserted = 0;
			if(!arr.empty())
			{
				for(int i = 0; i < arr.size(); i++)
				{
					// for each string in arr, extract the nuDays
				
					/*string tempdays = "";
					int j = 1;
					while(arr[i].substr(j,1).compare(")") != 0)
					{
						tempdays += arr[i].substr(j,1);
						j++;
					}
					stringstream geek(tempdays); // convert string to int
					int x;						 // ""
					geek >> x;					 // ""
					cout << x << endl;*/

					if(arr[i].hasNumPriority && arr[i].numDays > jdnend - jdnstart) // if tempdays casted to an int is bigger than numDays, place element here (before)
					{	
						arr.insert(arr.begin() + i, e);// place here and move everything else back one
						inserted = 1;
						cout << "Element added\n" << endl;
						break;
					}
				}
			}
			else
			{
				//cout << "pushback4" << endl;
				arr.push_back(e);
				inserted = 1;
				cout << "Element added\n" << endl;
			}
			if(inserted == 0)
				//cout << "pushback5" << endl;
				arr.push_back(e);
			cout << "Element added\n" << endl;
			//sort(arr.begin(), arr.end()); 
			break;


		case 3:

			// Ask user what item he/she wants removed, and remove it

			deleted = false;
			while(deleted == false)
			{
				cout << "Which item number would you like deleted? (Enter 'd' to display item numbers)" << endl;
				cin >> dinput;

				while(dinput == "d")
				{
					displayItemNumbers();
					cout << "Which item number would you like deleted? (Enter 'd' to display item numbers)" << endl;
					cin >> dinput;
				} // while

				if(1)
				{ // cast dinput into an int and delete arr[dinput]
					stringstream geek(dinput); // convert string to int
					int x;						 // ""
					geek >> x;					 // ""
					if(x >= arr.size() || x < 0)
					{
						cout << "Item number does not exist. Redirecting to main menu" << endl; // instead, we can loop and get proper output, or ask if user wants to return to main menu
							break;
					} // if
					arr.erase(arr.begin() + x);
					cout << "Item number " << x << " deleted" << endl;
					deleted = true;
				} // if

			} // while

			break;


		case 4:

			// Ask user what item he/she wants updated, and update it
			cout << "Which item number would you like updated? (Enter 'd' to display item numbers)" << endl;
			cin >> uinput;

			while(uinput == "d")
			{
				displayItemNumbers();
				cout << "Which item number would you like updated? (Enter 'd' to display item numbers)" << endl;
				cin >> uinput;
			} // while
			
			if(isNumber(uinput)) // if the number was valid
			{
				int pos;
				stringstream geek(uinput); // convert string to int			
				geek >> pos; 
				if(pos >= arr.size() || pos < 0)
				{ 
					cout << "Item number does not exist. Redirecting to main menu" << endl; // instead, we can loop and get proper output, or ask if user wants to return to main menu
					break;
				}
				cout << "What would you like to update for item number " << pos << "?:" << endl;
				arr[pos].print();
				cout << "1) Priority" << endl; // can add option to enter new duedate
				cout << "2) Task Description" << endl;
				cout << "3) Location" << endl;

				cin >> uinput;
				int num;
				bool sortRequest = false;
				bool numberWasFound = false;
				if(uinput == "1")
				{
					cout << "Enter the new desired priority for item number " << pos << endl;
					cin >> uinput;
					getline(cin, restDescr);
					uinput += restDescr;
					arr[pos].priority = uinput;
					// if there is a number in priority, ask if user wants to re-sort
					if((num = numberFound(arr[pos].priority)) + 1 )
					{
						numberWasFound = true;
						cout << "Would you like to re-sort this item?" << endl;
						cout << "1) Yes\n2) No" << endl;
						cin >> uinput;
						if(uinput == "1" || uinput == "Yes" || uinput == "YES" || uinput == "yes" || uinput == "YeS" || uinput == "YEs" || uinput == "yES")
							sortRequest = true;
					}
					// then if input was yes, do the resorting
					// if there is a number in priority, remove it and put it as numdays. If afterwards priority is "", then this is a purenumber priority.
					// if no number, pure string priority. If it has a number, potentially need to sort (or rather just place this appropriately)
					if(numberWasFound) // need to convert its type accordingly
					{
						// update everything now, then continue using our current time stamp
						refreshSchedule(); // all current values are of when this function was invoked

						//cout << "I found this number: " << num << endl;
						sprintf(temp, "%d", num); // convert int to string
		    			string intPriorityStr = (string)temp; 
		  				int intPriorityStrpos = (arr[pos].priority).find(intPriorityStr); // find the position of the integer in the priority string
				  		int digits = floor(log10(num)) + 1;
				  		arr[pos].priority = (arr[pos].priority).erase(intPriorityStrpos,digits); // delete integer in the string
				  		arr[pos].breakPos = intPriorityStrpos;					// position needs to be saved, so we can print it correctly later
				  		arr[pos].numDays = num;

				  		if(arr[pos].priority == "") // pure int case
				  		{
				  			arr[pos].convertPureInt();
				  		}
				  		else if(intPriorityStrpos == 1 && (arr[pos].priority).length() == 1) // ^# case if intPriorityStrPos is 1 and length of priority is now 1
				  		{
				  			arr[pos].convertCharNum();
				  			//cout << "Priority changed to ^#" << endl;
				  		}
				  		else // ^#^ case
				  		{
				  			arr[pos].convertStringNum();
				  			//cout << "converted to ^#^" << endl;
				  		}
				  		//arr.push_back(e); // instead of this, need to place in correct spot and delete current
				  		
				  		if(sortRequest) // only  update its position if re-sort was requested
				  		{
				  			ep = new Element();
					  		e = *ep;
					  		e = arr[pos];
					  		arr.erase(arr.begin()+pos);
					  		insertNumPriority(e);
				  		} // if re-sort was requested
				  		

					} // if a number was found in priority
					else // no number was found => pure string case
						arr[pos].convertPureString();
					cout << "Sucessfully updated item" << endl;

				}// if
				
				else if(uinput == "2")
				{
					cout << "Enter the new desired task description for item number " << pos << endl;
					//arr[pos].print();
					cin >> uinput;
					getline(cin, restDescr);
					uinput += restDescr;
					arr[pos].taskDescr = uinput;
					cout << "Successfully updated item number " << pos << endl;
				} // else

				else if(uinput == "3")
				{
					cout << "Which item number would you like item number " << pos << " to appear after? (Enter 'd' to display item numbers)"  << endl;
					cin >> uinput;

					while(uinput == "d")
					{
						displayItemNumbers();
						cout << "Which item number would you like item number " << pos << " to appear after? (Enter 'd' to display item numbers)" << endl;
						cin >> uinput;
					} // while

					if(isNumber(uinput)) // if the input was a number
					{
						stringstream geek(uinput); // convert string to int
						geek >> itemNum;
					} // if

					if(itemNum >= arr.size() || itemNum < 0)
					{
						cout << "There is no such item number. Returning to the main menu" << endl; // can change to go back and try again or option for main menu
						break;
					}
					
					else // valid number input
					{
						// save, then delete current element, and insert a copy after the specified number
						Element temporary = arr[pos];
						arr.insert(arr.begin()+itemNum+1, temporary);
						arr.erase(arr.begin()+pos);
						cout << "Successfully moved item" << endl;
					} // else	

				} // else if input was 3 (Update Location)
				
				else
				{
					cout << "That was not a valid input. Returning to the main menu" << endl;
				}	
			} // if

			else
			{
				cout << "That was not a valid input. Returning to the main menu" << endl;
			}

			break;


		// hidden updateSched()
		case 5:

			cout << "How many days old is the schedule?" << endl;
			cin >> updateDays;
			updateSched(updateDays);
			cout << "Schedule updated by " << updateDays << " days" << endl;
			break;


		// hidden add/change a prefix
		case 6: 

			cout << "To which item number would you like to add/change a prefix? (Enter 'd' to display item numbers)" << endl;
			cin >> uinput;

			while(uinput == "d")
			{
				displayItemNumbers();
				cout << "To which item number would you like to add/change a prefix? (Enter 'd' to display item numbers)" << endl; // option to append prefix?
				cin >> uinput;
			} // while

			if(isNumber(uinput)) // if the input was a number
			{
				stringstream geek(uinput); // convert string to int
				geek >> itemNum;
			} // if

			if(itemNum >= arr.size() || itemNum < 0)
			{
				cout << "There is no such item number. Returning to the main menu" << endl; // can change to go back and try again or option for main menu
				break;
			}
			
			else // valid number input
			{
				cout << "What would you like to do to item number " << itemNum << endl;
				arr[itemNum].print();
				cout <<"1) Append Prefix\n2) Change Prefix\n3) Delete Prefix" << endl;
				cin >> uinput;

				string prefixStr;

				if(uinput == "1") // this actually seems like a strange option
				{
					cout << "Enter the desired prefix to append to item number " << itemNum << endl;
					//arr[itemNum].print();
					cin >> prefixStr;
					arr[itemNum].prefix += prefixStr;
					cout << "Prefix appended for item number " << itemNum << endl;
				} // if Append Prefix

				else if(uinput == "2")
				{
					cout << "Enter the desired prefix for item number " << itemNum << endl;
					//arr[itemNum].print();
					cin >> prefixStr;
					arr[itemNum].prefix = prefixStr;
					cout << "Prefix changed for item number " << itemNum << endl;
				} // if Change Prefix

				else
				{
					arr[itemNum].prefix.erase();
					cout << "Successfully deleted prefix of item number " << itemNum << endl;
				} // else Delete Prefix
				
			} // if
			

			break;

		
		// hidden calculate a julian number from today
		case 7: 

			cout << "Input due date of task in format DDMMYYYY" << endl;
			cin >> dueDateStr;

			now = time(0);                                        // get current time
		    ltm = localtime(&now);
		    dd = ltm->tm_mday;
		    mm = ltm->tm_mon + 1;
		    yyyy = ltm->tm_year + 1900;

		    if(1) // hacks
		    {
		    	stringstream geek(dueDateStr); // convert string to int
				geek >> dueDate;
			} // if		

		    // convert current time to julian day 
		  	jdnstart = computejdn(dd, mm, yyyy);

		  	DD = dueDate / 1000000; // uppercase is due date, calculated when provided
		    MM = (dueDate % 1000000) / 10000;
		  	YYYY = dueDate % 10000;

		  	jdnend = computejdn(DD, MM, YYYY);

		  	cout << "The Julian number is " << jdnend - jdnstart << endl; 
		  	break;


		// hidden create a line break (unfinished)
		case 8: 

			cout << "Would you like to append or remove a linebreak?" << endl;
			cout << "1) Append\n2) Remove" << endl;
			cin >> uinput;

			if(uinput == "1")
			{
				cout << "After which item number would you like to append a linebreak? (Enter 'd' to display item numbers)" << endl;
				cin >> uinput;

				while(uinput == "d")
				{
					displayItemNumbers();
					cout << "To which item number would you like to append a linebreak? (Enter 'd' to display item numbers)" << endl;
					cin >> uinput;
				} // while

				if(isNumber(uinput)) // if the input was a number
				{
					stringstream geek(uinput); // convert string to int
					geek >> itemNum;
				} // if

				if(itemNum >= arr.size() || itemNum < 0)
				{
					cout << "There is no such item number. Returning to the main menu" << endl; // can change to go back and try again or option for main menu
					break;
				}
				
				else // valid number input
				{
					// create a new newline element and insert it into itemNum
					ep = new Element(2.0);
		    		e = *ep;
		    		arr.insert(arr.begin() + itemNum+1, e);
		    		cout << "Appended a newline to item number " << itemNum << endl;
				} // else
			} // if append linebreak

			else
			{
				cout << "Which number linebreak would you like to remove? (Enter 'd' to display itemnumbers)" << endl;
				cin >> uinput;

				while(uinput == "d")
				{
					displayItemNumbers();
					cout << "Which number linebreak would you like to remove? (Enter 'd' to display item numbers)" << endl;
					cin >> uinput;
				} // while

				if(isNumber(uinput)) // if the input was a number
				{
					stringstream geek(uinput); // convert string to int
					geek >> itemNum;
				} // if

				if(itemNum >= arr.size() || itemNum < 0)
				{
					cout << "There is no such item number. Returning to the main menu" << endl; // can change to go back and try again or option for main menu
					break;
				} // if
				
				else // valid number input
				{
					if(arr[itemNum].isNewLine())
					{
						arr.erase(arr.begin()+itemNum);
						cout << "Successfully deleted linebreak" << endl;
					}
					else
					{
						cout << "That is not a linebreak. Returning to the main menu" << endl;
					}
				} // else

 			} // else remove linebreak
			

			break;

		default:
		
			cout << "Error: Please supply a correct integer between 1 and 8.\n" << endl;


	} // switch; determine user action

















	} // while main loop of program 
	//cout << computejdn(11, 1, 2018) << endl;
	//cout << computejdn(12, 1, 2018) << endl; This works
	/*int dd;
	int mm;
	int yyyy;
	time_t now;
	tm *ltm;

	now = time(0);                                        // get current time
	ltm = localtime(&now);
	dd = ltm->tm_mday;
	mm = ltm->tm_mon + 1; // this is months since january
	yyyy = ltm->tm_year + 1900; // this is years since 1900
	cout << dd << endl;
	cout << mm << endl;
	cout << yyyy << endl;
	cout << computejdn(dd, mm, yyyy) << endl;
	int jdnstart = computejdn(dd, mm, yyyy);

	int dueDate;
	cout << "Input due date of task in format DDMMYYYY" << endl;
			cin >> dueDate;
	//cout << dueDate << endl;
	int DD;
	int MM;
	int YYYY;
	DD = dueDate / 1000000;
		    MM = (dueDate % 1000000) / 10000;
		  	YYYY = dueDate % 10000;
	//cout << DD << endl;
	//cout << MM << endl;
	//cout << YYYY << endl;

	int jdnend = computejdn(DD, MM, YYYY);
	//cout << jdnend << endl;
	char temp[20];
	string numDays;
		  	
	sprintf(temp, "%d", jdnend - jdnstart);
	cout << temp << endl;
	numDays = (string)temp;
	cout << numDays << endl; */


} // main()
