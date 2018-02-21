/*

char *ctime(const time_t *time);
This returns a pointer to a string of the form day month year hours:minutes:seconds year\n\0.

* Idea to keep extra features hidden from stdout and instead written in the readme 

* Probably need to store date info in case exported to file, wait a day, then import to program. Probably means we need timestamp of file modification

* Maybe export to file occasionally -> seems like a bad idea, just interact when the user is requesting, but could be explored

(#) Needs to deal with ?() case -> Just add a prefix to everything
Needs to have option to add a prefix to something
Needs to correctly print dates when showing item numbers
(#) Needs to be able to import file
(#) Priorities other than numbers (ASAP) etc
(#) Need to be able to have priorities that contain a number somewhere (assume there is only 1)
(#) Edit priorities option
Take timestamp from when user makes a relevant request (to add, display or update)
(#) Needs file to store tasks, 
Needs to take white spaces in input task descriptions, 
(#) Needs to update days remaining on display 
(#) Make option to display line numbers for editing
Swap option using line numbers?
Need to check against bad inputs - letters other than 'd' for deleting delete 0th item? numbers greater than size of arr give segfault 
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
	}

	Element(string s) // ^# case. Will print out the priority description first, then the number part of the priority
	{
		hasNumPriority = true;
		hasCharNumPriority = true;
		hasStringNumPriority = false;
		prefix = "";
	}

	Element(int a) // pure string case
	{
		hasNumPriority = false;
		hasCharNumPriority = false;
		hasStringNumPriority = false;
		prefix = "";
	}

	Element(bool b, int breakPos) // ^#^ case
	{
		hasNumPriority = true;
		hasCharNumPriority = false;
		hasStringNumPriority = true;
		this->breakPos = breakPos;
		prefix = "";
	}

	void convertPureInt()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = false;
	} // convertPureInt()

	void convertPureString()
	{
		this->hasNumPriority = false;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = false;	
	} // convertPureString()

	void convertCharNum()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = true;
		this->hasStringNumPriority = false;
	} // convertCharNum()

	void convertStringNum()
	{
		this->hasNumPriority = true;
		this->hasCharNumPriority = false;
		this->hasStringNumPriority = true;
	} // convertStringNum()
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
	for(i = 0; i < s.length(); i++)
	{
		if(isNumber(s.substr(i,1)))
		{
			numberFound = true;
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
		if(temp.find("00") != std::string::npos) // if 00 is found, do not treat this as a valid int
			return -1;
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
     while(!(infile.eof()))
     {
	    getline(infile,inputLine);
	    cout << inputLine << endl;
	    if((pos = inputLine.find("(")) != std::string::npos) //  we are at the first important line of the file
	    {	// need to see if priority is an integer, make a new element object, and assign priority and description. Note that items will be sorted.
	      	// could also take care of case where items are not sorted
	      	// need to take care of ^# case
	      	
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
		  	
		  	else if(temp != "" && isNumber(temp.substr(1,temp.length() - 1))) // ^# case
		  	{
		  		// create new element with ^# priority and the description found in the rest of inputline
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
	for(int i = 0; i < arr.size(); i++)
		cout << i << ": " << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print first element of string, then rest of string
	cout << "\n\n\n\n\n\n\n\n" << endl;
} // displayItemNumbers()

void addCharNumElement(string priority, int dueDate, string descr)
{

} // addCharNumElement()

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
			if(arr[i].numDays > e.numDays) // if tempdays casted to an int is bigger than numDays, place element here (before)
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

	if(argc == 2)
		importFile((string)argv[1]);

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

	cout << "\nMain Menu\n1) Display Schedule\n2) Add Item\n3) Remove Item\n4) Update Item\n" << endl;
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

		    outputFile.open("scheddata.txt");
			cout << "\n\n\n\n" << "*******************SCHEDULE*************************" << "\n" << (curJulianDay = computejdn(currentDay,currentMonth, currentYear)) << "\n\n" << endl; // for the legacy
			outputFile << "\n\n\n\n" << "*******************SCHEDULE*************************" << "\n" << curJulianDay << "\n\n" << endl;

		    for(int i = 0; i < arr.size(); i++)
		    {
		    	if(arr[i].hasStringNumPriority && arr[i].hasNumPriority) // ^#^ case
		    	{
		    		cout << arr[i].prefix << "(" << (arr[i].priority).substr(0,arr[i].breakPos) << arr[i].numDays << (arr[i].priority).substr(arr[i].breakPos) << ") " << arr[i].taskDescr << endl; // print priority, number of days, then task description in proper format
		    		outputFile << arr[i].prefix << "(" << (arr[i].priority).substr(0,arr[i].breakPos) << arr[i].numDays << (arr[i].priority).substr(arr[i].breakPos) << ") " << arr[i].taskDescr << endl;
		    	} // if

		    	else if(arr[i].hasCharNumPriority && arr[i].hasNumPriority) // ^# case  **can make this come after the current else if and only check if it has CharNumPriority
		    	{
		    		cout << arr[i].prefix << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print priority, number of days, then task description in proper format
		    		outputFile << arr[i].prefix << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr << endl;
		    	} // if

		    	else if(!arr[i].hasNumPriority)
		    	{
		    		cout << arr[i].prefix << "(" << arr[i].priority << ") " << arr[i].taskDescr << endl; // print priority then task description in proper format
		    		outputFile << arr[i].prefix << "(" << arr[i].priority << ") " << arr[i].taskDescr << endl;
		    	} // else if

		    	else
		    	{	
		    		cout << arr[i].prefix << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print number of days, then task description in proper format
		    		outputFile << arr[i].prefix << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl;
		    	} // else
		    } // for
		    cout << "\n\n\n\n\n\n\n\n" << endl;
		    outputFile << "\n\n\n\n\n\n\n\n" << endl;
		    outputFile.close();
		    //cout << "size after the print: " << arr.size() << endl;
			break;


		case 2:

			cout << "Enter Description of Task" << endl;
			cin >> taskDescr;
			//getline(cin, taskDescr);
			//std::cin.getline(cin,sizeof(taskDescr));

			cout << "Input due date of task in format DDMMYYYY" << endl;
			cin >> dueDateStr;

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

					if(arr[i].numDays > jdnend - jdnstart) // if tempdays casted to an int is bigger than numDays, place element here (before)
					{	
						arr.insert(arr.begin() + i, e);// place here and move everything else back one
						inserted = 1;
						break;
					}
				}
			}
			else
			{
				//cout << "pushback4" << endl;
				arr.push_back(e);
				inserted = 1;
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
					cout << "Which item number would you like updated? (Enter 'd' to display item numbers)" << endl;
					cin >> dinput;
				} // while

				if(1)
				{ // cast dinput into an int and delete arr[dinput]
					stringstream geek(dinput); // convert string to int
					int x;						 // ""
					geek >> x;					 // ""
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
			if(1) // hack
			{
				int pos;
				stringstream geek(uinput); // convert string to int			
				geek >> pos; 
				if(pos >= arr.size() || pos < 0)
				{ 
					cout << "Item number does not exist. Redirecting to main menu" << endl; // instead, we can loop and get proper output, or ask if user wants to return to main menu
					break;
				}
				cout << "What would you like to update?" << endl;
				cout << "1) Priority" << endl; // can add option to enter new duedate
				cout << "2) Task Description" << endl;

				cin >> uinput;
				int num;
				if(uinput == "1")
				{
					cout << "Enter the new desired priority for item number " << pos << endl;
					cin >> uinput;
					arr[pos].priority = uinput;
					// if there is a number in priority, remove it and put it as numdays. If afterwards priority is "", then this is a purenumber priority.
					// if no number, pure string priority. If it has a number, potentially need to sort (or rather just place this appropriately)
					if( (num = numberFound(arr[pos].priority)) + 1 )
					{
						// update everything now, then continue using our current time stamp
						refreshSchedule(); // all current values are of when this function was invoked

						cout << "I found this number: " << num << endl;
						sprintf(temp, "%d", num); // convert int to string
		    			string intPriorityStr = (string)temp; 
		  				int intPriorityStrpos = (arr[pos].priority).find(intPriorityStr); // find the position of the integer in the priority string
				  		int digits = floor(log10(num)) + 1;
				  		arr[pos].priority = (arr[pos].priority).erase(intPriorityStrpos,digits); // delete integer in the string
				  		arr[pos].breakPos = pos;					// position needs to be saved, so we can print it correctly later
				  		arr[pos].numDays = num;
				  		if(arr[pos].priority == "") // pure int case
				  		{
				  			arr[pos].convertPureInt();
				  		}
				  		else if(intPriorityStrpos == 1 && (arr[pos].priority).length() == 1) // ^# case if intPriorityStrPos is 1 and length of priority is now 1
				  		{
				  			arr[pos].convertCharNum();
				  		}
				  		else // ^#^ case
				  		{
				  			arr[pos].convertStringNum();
				  		}
				  		//arr.push_back(e); // instead of this, need to place in correct spot and delete current
				  		ep = new Element();
				  		e = *ep;
				  		e = arr[pos];
				  		arr.erase(arr.begin()+pos);
				  		insertNumPriority(e);

					} // if a number was found in priority
					else // no number was found => pure string case
						arr[pos].convertPureString();
					cout << "Sucessfully updated item" << endl;

				}// if
				else
				{
					cout << "Enter the new desired task description for item number " << pos << endl;
					cin >> uinput;
					arr[pos].taskDescr = uinput;
					cout << "Successfully updated item number " << pos << endl;
				} // else
			} // if

			break;


		// hidden updateSched()
		case 5:

			cout << "How many days old is the schedule?" << endl;
			cin >> updateDays;
			updateSched(updateDays);
			cout << "Schedule updated by " << updateDays << " days" << endl;
			break;

		default:
		
			cout << "Error: Please supply a correct integer between 1 and 4.\n" << endl;


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
