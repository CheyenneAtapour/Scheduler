/*
char *ctime(const time_t *time);
This returns a pointer to a string of the form day month year hours:minutes:seconds year\n\0.

Take timestamp from when user requests an action
Needs file to store tasks, 
Needs to take white spaces in input task descriptions, 
(#) Needs to update days remaining on display 
Make option to display line numbers for editing
Swap option using line numbers
Priorities other than numbers (ASAP) etc
Edit priorities option
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
	bool hasNumPriority;

	Element()
	{
		hasNumPriority = true;
	}

	Element(int a)
	{
		hasNumPriority = false;
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

void displayItemNumbers()
{

} // displayItemNumbers()


int main()
{
	//vector<string> arr; // array of user's to-do items
	// can calculate the day that this started running, then keep track of this day and update as required, save into file 
	//before shutdown. Will be a glitch between days, but when printing out, it should be fine because it will update
	time_t now;
	tm *ltm;
	now = time(0);                                        // get current time
	ltm = localtime(&now);
	int currentDay = ltm->tm_mday;
	int currentMonth = ltm->tm_mon + 1;
	int currentYear = ltm->tm_year + 1900;
	Element *ep;
	Element e;
	
	while (1){ // main loop of program
	
	// do everything after this	

	int command; // store user input
	

	int dueDate;
	string taskDescr;
	string fullTaskDescr;
	string element;
	string dinput;

	int dd;
	int mm;
	int yyyy;
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

	cout << "\nMain Menu\n1) Display Schedule\n2) Add Item\n3) Remove Item\n4) Update Item\n" << endl;
	cin >> command;

	switch ( command ) 
	{
		case 1:
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
		    	cout << "(" << arr[i].numDays << ") " << arr[i].taskDescr << endl; // print first element of string, then rest of string
		    cout << "\n\n\n\n\n\n\n\n" << endl;
			break;


		case 2:

			cout << "Enter Description of Task" << endl;
			cin >> taskDescr;
			//getline(cin, taskDescr);
			//std::cin.getline(cin,sizeof(taskDescr));

			cout << "Input due date of task in format DDMMYYYY" << endl;
			cin >> dueDate;

			now = time(0);                                        // get current time
		    ltm = localtime(&now); // How exactly does this work?
		    dd = ltm->tm_mday;
		    mm = ltm->tm_mon + 1;
		    yyyy = ltm->tm_year + 1900;

		    // RACE CONDITION POTENTIAL if this runs before item was inserted -> actually only the date when calculations happen is important
		    // this makes everything in terms of the date we just calculated
		    if(currentDay != dd || currentMonth != mm || currentYear != yyyy)
		    {
		    	updateSched(computejdn(dd, mm, yyyy) - computejdn(currentDay, currentMonth, currentYear));
		    	currentDay = dd;
		    	currentMonth = mm;
		    	currentYear = yyyy;
		    } // ehhh just set current times when starts, and update as you go through? if nothing entered, update doesnt do anything -> fine

		    // convert current time to julian day 
		  	jdnstart = computejdn(dd, mm, yyyy);

		  	DD = dueDate / 1000000;
		    MM = (dueDate % 1000000) / 10000;
		  	YYYY = dueDate % 10000;

		  	jdnend = computejdn(DD, MM, YYYY);
		  	
		  	sprintf(temp, "%d", jdnend - jdnstart);
		    numDays = (string)temp;                        // ummm

			element = "(" + numDays + ") " + taskDescr;

			ep = new Element();
			e = *ep;
			e.taskDescr = taskDescr;
			e.numDays = jdnend - jdnstart;

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
			}

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
