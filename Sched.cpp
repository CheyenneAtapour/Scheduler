/*

work on line 347

char *ctime(const time_t *time);
This returns a pointer to a string of the form day month year hours:minutes:seconds year\n\0.

* Idea to keep extra features hidden from stdout and instead written in the readme 

* Probably need to store date info in case exported to file, wait a day, then import to program. Probably means we need timestamp of file modification

(#) Needs to be able to import file
(#) Priorities other than numbers (ASAP) etc
Need to be able to have priorities that contain a number somewhere (assume there is only 1)
Edit priorities option
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
	string priority;

	// nonintuitive, but can be updated later
	Element() // by default, assume everything has pure number priority
	{
		hasNumPriority = true;
		hasCharNumPriority = false;
	}

	Element(string s) // ^# case
	{
		hasNumPriority = true;
		hasCharNumPriority = true;
	}

	Element(int a) // pure string case
	{
		hasNumPriority = false;
		hasCharNumPriority = false;
	}

};

int computejdn(int day, int month, int year)
{
	int a = floor((14 - month) / 12);
    int y = year + 4800 - a;
    int m = month + (12*a) - 3;
    return (day + floor( ( (153 * m) + 2) / 5 ) + (365*y) + floor(y/4) -
		  				floor(y/100) + floor(y/400) - 32045);

} // computejdn()

vector<Element> arr;

// subtracts diff from all integer priorities
void updateSched(int diff)
{
	for(int i = 0; i < arr.size(); i++)
	{
		if(arr[i].hasNumPriority)
			arr[i].numDays -= diff;
	} // for
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

void importFile(string fileName)
{
	string inputLine;
	ifstream infile;
	//infile.open ("scheddata.txt");
	infile.open(fileName);
     while(!(infile.eof()))
     {
	    getline(infile,inputLine);
	    cout << inputLine << endl;
	    if(inputLine[0] == '(') //  we are at the first important line of the file
	    {	// need to see if priority is an integer, make a new element object, and assign priority and description. Note that items will be sorted.
	      	// could also take care of case where items are not sorted
	      	// need to take care of ^# case
	      	
	      	string temp;
	      	int j = 1;
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
				arr.push_back(e);
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
		  		arr.push_back(e);
		  	} // else if

		  	else
		  	{ 
		  		// create new element with other priority and the description found in the rest of inputLine
		  		ep = new Element(1);
		  		e = *ep;
		  		e.taskDescr = inputLine.substr(j+2);
		  		e.priority = temp;
		  		arr.push_back(e);
		  	} // else
		  		
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
	currentDay = ltm->tm_mday;
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

	char temp[20];

	int inserted;
	bool deleted;
	bool charnum;

	cout << "\nMain Menu\n1) Display Schedule\n2) Add Item\n3) Remove Item\n4) Update Item\n" << endl;
	cin >> command;

	switch ( command ) 
	{
		case 1:
			// printing it out (for now) will be the only time the data is written to file
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

		    outputFile.open("scheddata.txt");
			cout << "\n\n\n\n" << "*******************SCHEDULE*************************\n\n" << endl;
			outputFile << "\n\n\n\n" << "*******************SCHEDULE*************************\n\n" << endl;
		    for(int i = 0; i < arr.size(); i++)
		    {
		    	if(arr[i].hasCharNumPriority && arr[i].hasNumPriority) // ^# case  **can make this come after the current else if and only check if it has CharNumPriority
		    	{
		    		cout << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print priority, number of days, then task description in proper format
		    		outputFile << "(" << arr[i].priority << arr[i].numDays << ") " << arr[i].taskDescr << endl;
		    	} // if

		    	else if(!arr[i].hasNumPriority)
		    	{
		    		cout << "(" << arr[i].priority << ") " << arr[i].taskDescr << endl; // print priority then task description in proper format
		    		outputFile << "(" << arr[i].priority << ") " << arr[i].taskDescr << endl;
		    	} // else if

		    	else
		    	{	
		    		cout << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print number of days, then task description in proper format
		    		outputFile << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl;
		    	} // else
		    } // for
		    cout << "\n\n\n\n\n\n\n\n" << endl;
		    outputFile << "\n\n\n\n\n\n\n\n" << endl;
		    outputFile.close();
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

		  	DD = dueDate / 1000000;
		    MM = (dueDate % 1000000) / 10000;
		  	YYYY = dueDate % 10000;

		  	jdnend = computejdn(DD, MM, YYYY);
		  	
		  	sprintf(temp, "%d", jdnend - jdnstart); // convert int to string
		    numDays = (string)temp;                        // ummm

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
				arr.push_back(e);
				inserted = 1;
			}
			if(inserted == 0)
				arr.push_back(e);
			cout << "Element added\n" << endl;
			//sort(arr.begin(), arr.end()); 
			break;


		case 3:

			// Ask user what item he/she wants removed, and remove it

			deleted = false;
			while(deleted == false){
			cout << "Which item number would you like deleted? (Enter 'd' to display item numbers)" << endl;
			cin >> dinput;

			if(dinput == "d")
				displayItemNumbers();
			else
			{ // cast dinput into an int and delete arr[dinput]
				stringstream geek(dinput); // convert string to int
				int x;						 // ""
				geek >> x;					 // ""
				arr.erase(arr.begin() + x);
				cout << "Item number " << x << " deleted" << endl;
				deleted = true;
			} // else

			} // while

			break;


		case 4:

			// Ask user what item he/she wants updated, and update it

			break;


		// hidden test updateSched()
		/*case 5:

			updateSched(3);
			cout << "Schedule updated by 3 days" << endl;
			break;*/

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
