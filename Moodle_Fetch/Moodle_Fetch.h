#pragma warning (disable : 4996)
#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <curl/curl.h>
#include <fstream>

using namespace std;


struct FtpFile {
	const char *filename;
	FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct FtpFile *out = (struct FtpFile *)stream;

	if (out && !out->stream) {
		/* open file for writing */
		out->stream = fopen(out->filename, "wb");
		if (!out->stream)
			return -1; /* failure, can't open file to write */
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

//
//int main(void)
//{
//	CURL *curl;
//	CURLcode res;
//	struct FtpFile ftpfile = {
//		"curl.tar.gz", /* name to store the file as if succesful */
//		NULL
//	};
//
//	curl_global_init(CURL_GLOBAL_DEFAULT);
//
//	curl = curl_easy_init();
//	if (curl) {
//
//		curl_easy_setopt(curl, CURLOPT_URL, "https://moodle.redlands.edu/login/index.php");
//		/* Define our callback to get called when there's data to be written */
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
//		/* Set a pointer to our struct to pass to the callback */
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
//
//		/* Switch on full protocol/debug output */
//		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//
//		res = curl_easy_perform(curl);
//
//		/* always cleanup */
//		curl_easy_cleanup(curl);
//
//		if (CURLE_OK != res) {
//			/* we failed */
//			fprintf(stderr, "curl told us %d\n", res);
//		}
//	}
//
//	if (ftpfile.stream)
//		fclose(ftpfile.stream); /* close the local file */
//
//	curl_global_cleanup();
//
//	system("PAUSE");
//	return 0;
//}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}



void downloadFile(CURL* myHandle, char* file_l, CURLcode res, char*);



//#ifdef SKIP_PEER_VERIFICATION
///*
//* If you want to connect to a site who isn't using a certificate that is
//* signed by one of the certs in the CA bundle you have, you can skip the
//* verification of the server's certificate. This makes the connection
//* A LOT LESS SECURE.
//*
//* If you have a CA cert for the server stored someplace else than in the
//* default bundle, then the CURLOPT_CAPATH option might come handy for
//* you.
//*/
//curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0L);
//#endif
//
//#ifdef SKIP_HOSTNAME_VERIFICATION
///*
//* If the site you're connecting to uses a different host name that what
//* they have mentioned in their server certificate's commonName (or
//* subjectAltName) fields, libcurl will refuse to connect. You can skip
//* this check, but this will make the connection less secure.
//*/
//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//#endif

void Moodle_Fetch(string login_link, string username, string password)
{
	int count = 0;
	int count_file = 0;
	CURL *myHandle;
	CURLcode res;
	string contents;
	string line = login_link;
	char *result = strdup(line.c_str());

	bool finished = false;
	bool state = false;
	string troll, link, reform;
	string files[100];
	size_t found, reform_find;
	string file_link;
	string data_insert;

	

	curl_global_init(CURL_GLOBAL_DEFAULT);

	myHandle = curl_easy_init();
	if (myHandle) {
		curl_easy_setopt(myHandle, CURLOPT_URL, result);
		//#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0L);
		//#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/4.0");
		curl_easy_setopt(myHandle, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(myHandle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(myHandle, CURLOPT_COOKIEFILE, "");

		// Visit the login page once to obtain a PHPSESSID cookie
		curl_easy_setopt(myHandle, CURLOPT_URL, result);
		curl_easy_perform(myHandle);

		// Now, can actually login. First we forge the HTTP referer field, or HTS will deny the login
		curl_easy_setopt(myHandle, CURLOPT_REFERER, result);
		// Next we tell LibCurl what HTTP POST data to submit
		data_insert += "username=";
		data_insert += username;
		data_insert += "&password=";
		data_insert += password;
		
		char *data = strdup(data_insert.c_str());
		//char *data = "username=justin_yang&password=bee1230";
		curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, data);
		curl_easy_perform(myHandle);



		curl_easy_setopt(myHandle, CURLOPT_URL, result);
		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &contents);  // sets user arguments
		res = curl_easy_perform(myHandle);
		//	curl_easy_cleanup(myHandle);

		ofstream lol;
		lol.open("Output.txt");
		lol << contents;
		lol.close();


		ifstream myfile("fileID.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, troll))
			{
				files[count] = troll;
				cout << "Link #" << count << troll << endl;
				count++;
			}
			cout << count << endl;
			myfile.close();
		}




		do{
			found = contents.find("http://moodle.redlands.edu/mod/resource/view.php?id=");
			contents = contents.substr(found, contents.length() - 1);
			////	cout << endl << "CONTENTS1: " << contents << endl;
			found = contents.find("http://moodle.redlands.edu/mod/resource/view.php?id=");
			reform_find = contents.find("\"");
			link = contents.substr(found, reform_find);   //finds from found to next "\";
			contents = contents.substr(reform_find, contents.length() - 1);

			for (int i = 0; i < count; i++)
			{
				if (link.compare(files[i]))
					state = true;
				else
					state = false;
			}
			count_file = count;
			if (state == false)
			{
				count_file++;
				cout << endl << "DOWNLOADING FILE!! DOWNLOADING FILE!!" << endl;
				cout << link << endl;

				char *file_l = strdup(link.c_str());
				string file_name = "Assignment #";
				file_name += to_string(count_file);
				file_name += ".pdf";
				char *filename = strdup(file_name.c_str());
				downloadFile(myHandle, file_l, res, filename);
					std::ofstream myfile("fileID.txt", std::ios_base::app | std::ios_base::out);
					myfile << endl << link;
			}

			
			if (contents.find("http://moodle.redlands.edu/mod/resource/view.php?id=") == string::npos)
				finished = true;
		} while (finished != true);


	}

	curl_global_cleanup();
}

void downloadFile(CURL* myHandle, char* file_l, CURLcode res, char* name)
{
	struct FtpFile ftpfile = {
		name, /* name to store the file as if succesful */
		NULL
	};


	curl_easy_setopt(myHandle, CURLOPT_URL, file_l);
	/* Define our callback to get called when there's data to be written */
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, my_fwrite);
	/* Set a pointer to our struct to pass to the callback */
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &ftpfile);
	/* Switch on full protocol/debug output */
	curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1L);
	res = curl_easy_perform(myHandle);
}