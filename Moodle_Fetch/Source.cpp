
#include "Moodle_Fetch.h"



int main(void)
{

	string login_link, username, password;

	cout << "Welcome to Moodle Fetch!" << endl;
	cout << "A not very well written assignment grabber. By Justin" << endl;
	cout << "Enter the login address: ";
	cin >> login_link;
	cout << "Enter your username: ";
	cin >> username;
	cout << "Enter your password: ";
	cin >> password;

	Moodle_Fetch(login_link, username, password);


	system("PAUSE");
	return 0;
}

