#ifndef _CErrorLog_HG_
#define _CErrorLog_HG_

// A singleton error logging class.
// Can write to cout, cerr, or to a specified file

#include <string>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>

class CErrorLog
{
public:
	CErrorLog();
	~CErrorLog();

	template <class T>
	void PrintToLog( T theThingToWrite )
	{
		if ( this->m_pLogFile != 0 )
		{
			(*this->m_pLogFile) << theThingToWrite;
			this->m_PrintEndlineCharacter();
		}
		return;
	}


	template <class T>
	void PrintToLog( T theThingToWrite, const int line, const char* file )
	{
		if ( this->m_pLogFile != 0 )
		{
			if ( this->m_bAutoStripPathFromFileNames )
			{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );	}
			else 
			{	(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileString( line, file );	}
			this->m_PrintEndlineCharacter();
		}
		return;
	}

	template <class T>
	void PrintToLogNoPath( T theThingToWrite, const int line, const char* file )
	{
		if ( this->m_pLogFile != 0 )
		{
			(*this->m_pLogFile) << theThingToWrite << this->m_FormatLineAndFileStringNoPath( line, file );
			this->m_PrintEndlineCharacter();
		}
		return;
	}
	// This isn't quite the right overloading signature (can't chain the streams together)
	template <class T>
	void operator<<( T theThingToWrite )
	{
		if ( this->m_pLogFile != 0 )
		{
			(*this->m_pLogFile) << theThingToWrite;
			this->m_PrintEndlineCharacter();
		}// if ( this->m_pLogFile != 0 )
		return;
	}
	//template <class T>
	//friend std::ostream &operator<<( std::ostream &output, const std::string &theThingToWrite )
	//{ 
	//	if ( this->m_pLogFile != 0 )
	//	{
	//		(*this->m_pLogFile) << theThingToWrite;
	//		this->m_PrintEndlineCharacter();
	//	}// if ( this->m_pLogFile != 0 )
	//	return output;          
	//}

	void SetWriteToCout(bool bEnabled);
	void SetWriteToCerr(bool bEnabled);
	void SetWriteToLogFile(bool bEnabled);		// false = Prevents writing, but doesn't close
	void SetAutoCRLF(bool bEnabled);
	void SetForceFlush(bool bEnabled);
	void SetAutoStripPathFromFileNames(bool bEnabled);
	void OpenOrChangeLogFile( std::string logFileName );
	void CloseLogFile(void);
	std::string GetLogFileName(void);
	bool bIsLogFileOpen(void);

	// This class is to aid in determining the "last X log files" to keep
	class CSensibleLogFileData
	{
	public:
		CSensibleLogFileData() : hour(0), minute(0), second(0), date(0), month(0), year(0) {};
		// A couple example file names:
		// "RunLog (Sat Feb 13 13-45-57 2016).log"
		// "RunLog (Tue Nov 24 13-13-07 2015).log"
		std::string sDayRaw;		// Original value: Mon, Tue, Wed, Thu, Fri, Sat, Sun
		std::string sDayLong;
		std::string sMonthRaw;		// Original value: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
		std::string sMonthLong;
		int date;
		int month;
		int year;
		int hour;
		int minute;
		int second;
		std::string logFileNameRaw;	
		// Year, month, date, hour, minute, second, separated by spaces
		std::string dateThatYouCanActuallySortDammitJim;
		// If blank, takes the member value
		void parseLogFileNameAndUpdate( std::string logFileNameToParse = "" );
		int stringToInt( std::string theString );
		// Mainly for the sorting operation. Well, that's the only reason these are here
		bool operator< (const CSensibleLogFileData &rhs)
		{	/* do actual comparison */ 
			return (this->dateThatYouCanActuallySortDammitJim < rhs.dateThatYouCanActuallySortDammitJim);
		}
		// I don't really know if these work... (they weren't needed for the sort, anyway
		//bool operator> (const CSensibleLogFileData &rhs)
		//{	/* do actual comparison */ 
		//	//return (this->dateThatYouCanActuallySortDammitJim > rhs.dateThatYouCanActuallySortDammitJim);
		//	return !(*this < rhs );
		//}
		//bool operator<=(const CSensibleLogFileData &rhs){ return !((*this) > rhs); }
		//bool operator>=(const CSensibleLogFileData &rhs){ return !((*this) < rhs); }
	};
	// Returns anything that ends in "log" 
	void GetListOfExistingLogs( std::vector< CSensibleLogFileData > &vecLogFileNames );
	bool DeleteOldLogFiles( int numberOfLogsToKeep = 5 );

private:
	bool m_bWriteToCout;
	bool m_bWriteToCerr;
	bool m_bWriteToLogFile;
	bool m_bAutoCRLF;
	bool m_bForceFlush;
	bool m_bAutoStripPathFromFileNames;
	std::ofstream* m_pLogFile;
	bool m_bLogFileOK;
	std::string m_logFileName;
	std::string m_logFileTimeWhenOpennedAsText;
	std::string m_cleanTimeToNiceFileName(std::string theText);
	// These take the __LINE__ and __FILE__ things and return a string in this format:
	// ":@Line(xxx), File(yyy) "
	std::string m_FormatLineAndFileString( const int lineNumber, const char* file );
	std::string m_FormatLineAndFileStringNoPath( const int lineNumber, const char* file );

	// Scans from the right side until it gets to any kind of slash
	std::string m_StripPathFromFile( std::string fileNameIn );
	// WARNING: Assumes file is VALID!
	void m_PrintEndlineCharacter(void);
};

#endif
