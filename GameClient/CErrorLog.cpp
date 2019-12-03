#include "CErrorLog.h"
#include <ctime>
#include <sstream>
#include <algorithm>

CErrorLog::CErrorLog()
{
	this->m_pLogFile = 0;
	this->m_bWriteToCerr = false;
	this->m_bWriteToCout = false;
	this->m_bWriteToLogFile = false;
	this->m_bAutoCRLF = true;
	this->m_bForceFlush = true;
	this->m_bLogFileOK = false;
	this->m_bAutoStripPathFromFileNames = false;
	return;
}
	
CErrorLog::~CErrorLog()
{
	this->m_bLogFileOK = false;
	if ( this->m_pLogFile != 0 )
	{
		this->m_pLogFile->close();
	}
	delete this->m_pLogFile;
	return;
}

void CErrorLog::SetWriteToCout(bool bEnabled)
{
	this->m_bWriteToCout = bEnabled;
	return;
}

void CErrorLog::SetWriteToCerr(bool bEnabled)
{
	this->m_bWriteToCerr = bEnabled;
	return;
}

// false = Prevents writing, but doesn't close
void CErrorLog::SetWriteToLogFile(bool bEnabled)	
{
	this->m_bWriteToLogFile = bEnabled;
	return;
}


void CErrorLog::SetAutoCRLF(bool bEnabled)
{
	this->m_bAutoCRLF = bEnabled;
	return;
}

void CErrorLog::SetForceFlush(bool bEnabled)
{
	this->m_bForceFlush = bEnabled;
	return;
}

void CErrorLog::SetAutoStripPathFromFileNames(bool bEnabled)
{
	this->m_bAutoStripPathFromFileNames = bEnabled;
	return;
}

// Need to get the date and time of the file, maybe?
void CErrorLog::OpenOrChangeLogFile( std::string logFileName )
{
	// File already open?
	if ( this->m_pLogFile != 0 )
	{	// Yes, so close it
		this->m_pLogFile->close();
	}

	//// Get the time
	//time_t curTime = time(0);
	//std::stringstream ssTime;
	//ssTime << logFileName << " (" << asctime( std::localtime(&curTime) ) << ").log";

	// Using the secure functions (mainly to stop the compiler complaining at me)
	// From: https://msdn.microsoft.com/en-us/library/b6htak9c(v=vs.110).aspx
	struct tm newtime;
	__time32_t aclock;

	// "The string result produced by asctime_s contains exactly 26 characters and 
	//		has the form Wed Jan 02 02:03:55 1980\n\0.	
	static const unsigned int BUFFERSIZE = 26;	
	char timeCharBuffer[BUFFERSIZE] = {0};
	_time32( &aclock );						// Get time in seconds.
	_localtime32_s( &newtime, &aclock );	// Convert time to struct tm form.
	// Reutrns zero (0) if everything is OK
	if ( asctime_s(timeCharBuffer, BUFFERSIZE, &newtime) != 0 )
	{	// Didn't get time, so clear the buffer that contains the time string
		memset( timeCharBuffer, 0, BUFFERSIZE );
	}
	std::stringstream ssTime;
	ssTime << logFileName << " (" << timeCharBuffer << ").log";

	this->m_logFileTimeWhenOpennedAsText = ssTime.str();
	this->m_logFileTimeWhenOpennedAsText = this->m_cleanTimeToNiceFileName( this->m_logFileTimeWhenOpennedAsText );
	
	this->m_pLogFile = new std::ofstream( this->m_logFileTimeWhenOpennedAsText );
	this->m_bLogFileOK = this->m_pLogFile->is_open();
	this->m_logFileName = logFileName;
	// They might have set the logger to automatically put a new line, so avoid this by
	//	creating single line here, then printing it (otherwise this header might be broken
	//	up into separate lines. And who wants that, am I right?)
	std::stringstream ssHeader;
	ssHeader << "New log file created: \"" << this->m_logFileTimeWhenOpennedAsText << "\"\n";
	this->PrintToLog( ssHeader.str() );
	return;
}


void CErrorLog::CloseLogFile(void)
{
	if ( this->m_pLogFile != 0 )
	{
		this->m_pLogFile->close();
	}
	return;
}

std::string CErrorLog::GetLogFileName(void)
{
	// This doesn't check if the log file is "OK" because
	//	 it could be you tried to open it, but it didn't open for some reason
	if ( this->m_pLogFile != 0 )
	{
		return this->m_logFileName;
	}
	return "";
}

bool CErrorLog::bIsLogFileOpen(void)
{
	if ( this->m_pLogFile != 0 )
	{
		return this->m_pLogFile->is_open();
	}
	return false;
}

//void CErrorLog::operator<<( std::string textToWrite )
//{
//	if ( this->m_pLogFile != 0 )
//	{
//		(*this->m_pLogFile) << textToWrite;
//	}
//	return;
//}

std::string CErrorLog::m_cleanTimeToNiceFileName(std::string theText)
{
	std::string returnString;
	for ( std::string::iterator itCurChar = theText.begin(); itCurChar != theText.end(); itCurChar++ )
	{
		char curChar = *itCurChar;
		switch ( curChar )
		{
		case ':':
			returnString.push_back('-');
			break;
		case '\n':
			// Do nothing. Don't append
			break;
		default:
			// Regular character so add it
			returnString.push_back( curChar );
			break;
		}
	}
	return returnString;
}

// These take the __LINE__ and __FILE__ things and return a string in this format:
// ":@Line(xxx), File(yyy) "
std::string CErrorLog::m_FormatLineAndFileString( const int lineNumber, const char* file )
{
	std::stringstream ss;
	ss << ":@Line(" << lineNumber << "), File(" << file << ") ";
	return ss.str();
}

std::string CErrorLog::m_FormatLineAndFileStringNoPath( const int lineNumber, const char* file )
{
	std::string justFile( file );
	justFile = this->m_StripPathFromFile( justFile );
	return this->m_FormatLineAndFileString( lineNumber, justFile.c_str() ); 
}

// Scans from the right side until it gets to any kind of slash
std::string CErrorLog::m_StripPathFromFile( std::string fileNameIn )
{
	std::stringstream ssJustFile;
	for ( std::string::reverse_iterator itCurChar = fileNameIn.rbegin();
		  itCurChar != fileNameIn.rend(); itCurChar++ )
	{
		char curChar = *itCurChar;
		if ( ( curChar == '\\' ) || ( curChar == '/' )) 
		{
			break;
		}
		ssJustFile << curChar;
	}
	std::string justFile = ssJustFile.str();
	std::reverse( justFile.begin(), justFile.end() );
	return justFile;
}

// WARNING: Assumes file is VALID!
void CErrorLog::m_PrintEndlineCharacter(void)
{
	if ( this->m_bAutoCRLF )
	{
		if ( this->m_bForceFlush )	
			{ (*this->m_pLogFile) << std::endl;	}
		else 
			{ (*this->m_pLogFile) << "\n";	}
	}
	return;
}

#ifdef WIN32
#include <Windows.h>
#include "../Utilities/CStringHelper.h"
#endif

void CErrorLog::GetListOfExistingLogs( std::vector< CSensibleLogFileData > &vecLogFileNames )
{
#ifdef WIN32
	std::wstring dir = L"*.log";
	WIN32_FIND_DATA ffd;
	HANDLE h = FindFirstFile( dir.c_str(), &ffd);
	while(FindNextFile(h, &ffd))
	{
		CSensibleLogFileData logFile;
		logFile.logFileNameRaw = CStringHelper::UnicodeToASCII_QnD( ffd.cFileName );
		logFile.parseLogFileNameAndUpdate();
		vecLogFileNames.push_back( logFile );
	}
	//DWORD dw = GetLastError();// returns ERROR_NO_MORE_FILES
#endif 
	return;
}

////#include <iostream>
bool CErrorLog::DeleteOldLogFiles( int numberOfLogsToKeep /*=3*/ )
{
	std::vector< CSensibleLogFileData > vecOldLogs;
	this->GetListOfExistingLogs( vecOldLogs );

	// Newest log files are at the start of the vector (oldest at end)
	std::sort( vecOldLogs.rbegin(), vecOldLogs.rend() );

	int fileToKeepCount = numberOfLogsToKeep;
	bool bAllGood = true;
	for ( std::vector< CSensibleLogFileData >::iterator itLF = vecOldLogs.begin(); itLF != vecOldLogs.end(); itLF++, fileToKeepCount-- )
	{	// Saved the right number of files?
		if ( fileToKeepCount <= 0 )
		{	// Yup, so delete this one
#ifdef WIN32
			if ( ! DeleteFile( CStringHelper::ASCIIToUnicodeQnD( itLF->logFileNameRaw ).c_str() ) )
			{
				bAllGood = false;
			}
#endif
		}//if ( fileToKeepCount < 0 )
	}//for ( std::vector< CSensibleLogFileData >::iterator itLF

	return bAllGood;
}

// "RunLog (Sat Feb 13 13-45-57 2016).log"
// "RunLog (Tue Nov 24 13-13-07 2015).log"
void CErrorLog::CSensibleLogFileData::parseLogFileNameAndUpdate( std::string logFileNameToParse /* ="" */ )
{
	// Empty? 
	if ( logFileNameToParse != "" ) {	this->logFileNameRaw = logFileNameToParse;	}

	// Anything to parse
	if ( this->logFileNameRaw == "" )	
	{ /*nope*/ return; }

	// Scan to "("
	
	std::stringstream ssSensibleDate;

	std::string sYear = this->logFileNameRaw.substr( 28, 4 );
	this->sMonthRaw = this->logFileNameRaw.substr( 12, 3 );

	// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
	std::string sMonthTwoDigits;
	if ( this->sMonthRaw == "Jan" )		 { this->month = 1; sMonthTwoDigits = "01"; this->sMonthLong = "January"; }
	else if ( this->sMonthRaw == "Feb" ) { this->month = 2; sMonthTwoDigits = "02";	this->sMonthLong = "February"; }
	else if ( this->sMonthRaw == "Mar" ) { this->month = 3; sMonthTwoDigits = "03"; this->sMonthLong = "March"; }
	else if ( this->sMonthRaw == "Apr" ) { this->month = 4; sMonthTwoDigits = "04"; this->sMonthLong = "April"; }
	else if ( this->sMonthRaw == "May" ) { this->month = 5; sMonthTwoDigits = "05"; this->sMonthLong = "May"; }
	else if ( this->sMonthRaw == "Jun" ) { this->month = 6; sMonthTwoDigits = "06"; this->sMonthLong = "June"; }
	else if ( this->sMonthRaw == "Jul" ) { this->month = 7; sMonthTwoDigits = "07"; this->sMonthLong = "July"; }
	else if ( this->sMonthRaw == "Aug" ) { this->month = 8; sMonthTwoDigits = "08"; this->sMonthLong = "August"; }
	else if ( this->sMonthRaw == "Sep" ) { this->month = 9; sMonthTwoDigits = "09"; this->sMonthLong = "September"; }
	else if ( this->sMonthRaw == "Oct" ) { this->month = 10; sMonthTwoDigits = "10"; this->sMonthLong = "October"; }
	else if ( this->sMonthRaw == "Nov" ) { this->month = 11; sMonthTwoDigits = "11"; this->sMonthLong = "November"; }
	else if ( this->sMonthRaw == "Dec" ) { this->month = 12; sMonthTwoDigits = "12"; this->sMonthLong = "December"; }

	this->sDayRaw = this->logFileNameRaw.substr( 8, 3 );

	// Mon, Tue, Wed, Thu, Fri, Sat, Sun
	if ( this->sDayRaw == "Mon" )		{ this->sDayLong = "Monday"; }
	else if ( this->sDayRaw == "Tue" )	{ this->sDayLong = "Tuesday"; }
	else if ( this->sDayRaw == "Wed" )	{ this->sDayLong = "Wednesday"; }
	else if ( this->sDayRaw == "Thu" )	{ this->sDayLong = "Thursday"; }
	else if ( this->sDayRaw == "Fri" )	{ this->sDayLong = "Friday"; }
	else if ( this->sDayRaw == "Sat" )	{ this->sDayLong = "Saturday"; }
	else if ( this->sDayRaw == "Sun" )	{ this->sDayLong = "Sunday"; }

	std::string sDate = this->logFileNameRaw.substr( 16, 2 );
	std::string sHour = this->logFileNameRaw.substr( 19, 2 );
	std::string sMinute = this->logFileNameRaw.substr( 22, 2 );
	std::string sSecond = this->logFileNameRaw.substr( 25, 2 );

	ssSensibleDate 
		<< sYear << "-" << sMonthTwoDigits << "-" << sDate << "@"
		<< sHour << ":" << sMinute << "." << sSecond;
	this->dateThatYouCanActuallySortDammitJim = ssSensibleDate.str();

	this->sDayRaw = this->logFileNameRaw.substr( 8, 3 );

	this->year = this->stringToInt( sYear );
	this->date = this->stringToInt( sDate );
	this->hour = this->stringToInt( sHour );
	this->minute = this->stringToInt( sMinute );
	this->second = this->stringToInt( sSecond );

	return;
}

int CErrorLog::CSensibleLogFileData::stringToInt( std::string theString )
{
	std::stringstream ss;
	ss << theString;
	int returnVal;
	ss >> returnVal;
	return returnVal;
}
