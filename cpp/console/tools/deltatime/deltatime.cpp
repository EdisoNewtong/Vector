#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
using namespace std;
using namespace std::chrono;


static const int SC_YEAR_BEGIN = 1900;

// time_t 
std::chrono::system_clock::time_point
makeTimePoint(int year,int mon, int day, int hour,int min, int sec)
{
    struct std::tm tm;
    tm.tm_year = year - SC_YEAR_BEGIN; // year since 1900
    tm.tm_mon = mon - 1;   // 0 ~ 11 => [Jan ~ Dec]
    tm.tm_mday = day;  // 1 .. 31
    tm.tm_hour = hour; // [0~23]
    tm.tm_min = min;  // [0~59]
    tm.tm_sec = sec; // [0~59] or [0,60] 60  for leap seconds
    tm.tm_isdst = 0; // -1, 0, 1 
    std::time_t tt = std::mktime(&tm);

    if ( tt == -1 ) {
        throw logic_error(" mktime(...) failed , invalid date time , date should >=   1970/1/1 08:00:00");
    }

	// the return value's unit is second
    return std::chrono::system_clock::from_time_t(tt);
}

// gmtime(&t)
// std::ctime(&t)

int wrap_stoi(const string& s)
{
	int num = 0;
    try {
        num = stoi(s);
    } catch ( const invalid_argument&  e ) {
        (void)e;
        throw invalid_argument( string("Can't convert string ")  + s +  "\" to int ");
    }

	return num;
}




bool isLeapYear(int year)
{
	return (year % 400 == 0) ||  ( ((year % 4) == 0)   &&  ((year % 100) != 0)  );
}


int checkDayByYearMonth(int day, int year, int month)
{
	if ( !(day >=1   &&   day <= 31) ) {
		return 1;
	}

	bool leapFlag = isLeapYear(year);
	switch( month )
	{
		// the following check is checked in the below part
	// case 1:
	// case 3:
	// case 5:
	// case 7:
	// case 8:
	// case 10:
	// case 12:
	// 	{
	// 		if ( day > 31 ) {
	// 			return 2;
	// 		}
	// 	}
	// 	break;
	case 4:
	case 6:
	case 9:
	case 11:
		{
			if ( day == 31 ) {
				return 2;
			}
		}
		break;
	case 2:
		{
			if ( !leapFlag ) {
				if ( day > 28 ) {
					return 3;
				}
			} else {
				if ( day > 29 ) {
					return 4;
				}
			}
		}
		break;
	default:
		break;
	}

	return 0;
}


// YYYY/MM/DD hh:mm:ss
void seq2date(const string& strDate, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMin, int* pSec)
{
    // logic_error( "" )
	const int c_BeginEpoch = 1970;
	const char space = ' ';
    const char sep1 = '/';
    const char sep2 = ':';
    const auto notFound = string::npos;
    auto strPos = string::npos;
    auto beg = strPos;
	beg = 0;

	// year part
    strPos = strDate.find( sep1, beg );
    if ( strPos == notFound ) {
        throw logic_error("Can't found 'Year' Part . ");
    }

	string strYearPart = strDate.substr(beg, strPos - beg);
	 // strip beginning  space
	auto pos1stnumber = strYearPart.find_first_not_of(space);
	if ( pos1stnumber != notFound ) {
		strYearPart = strYearPart.substr(pos1stnumber);
	}

	if ( static_cast<int>(strYearPart.size() ) != 4 ) {
        throw logic_error( string("Year Part \"") +  strYearPart + "\" is not a year(YYYY) format.");
	}
	*pYear = wrap_stoi( strYearPart );
	if ( *pYear <=0 ) { // || *pYear < c_BeginEpoch  ) {
        throw logic_error( string("Year Part \"") +  to_string(*pYear) + "\" is less than 0. ");
	} else if ( *pYear < c_BeginEpoch ) {
        throw logic_error( string("Year Part \"") +  to_string(*pYear) + "\" is less than " + to_string(c_BeginEpoch) + ". " );
	}
	// cout << "\"" << strYearPart << "\" => "  << *pYear << endl;


	// month part
	beg = strPos + 1;
	strPos = strDate.find( sep1, beg);
	if ( strPos == notFound ) {
        throw logic_error("Can't found 'Month' Part . ");
	}

	string strMonthPart = strDate.substr(beg, strPos - beg);
	if ( static_cast<int>(strMonthPart.size() ) > 2 ) {
        throw logic_error( string("Month Part \"") +  strMonthPart + "\" is not a month(MM) format.");
	}
	*pMonth = wrap_stoi( strMonthPart );
	if ( !(*pMonth>=1 && *pMonth<=12) ) {
        throw logic_error( string("Month Part \"") +  strMonthPart + "\" is not between 1~12 .");
	}
	// cout << "\"" << strMonthPart  << "\" => "  << *pMonth << endl;


	// day part
	beg = strPos + 1;
	strPos = strDate.find( space, beg);
	if ( strPos == notFound ) {
        throw logic_error("Can't found 'Day' Part . ");
	}

	string strDayPart = strDate.substr(beg, strPos - beg);
	if ( static_cast<int>(strDayPart.size() ) > 2 ) {
        throw logic_error( string("Day Part \"") +  strDayPart + "\" is not a day(DD) format.");
	}
	*pDay = wrap_stoi( strDayPart );
	int dayCheckFlag = checkDayByYearMonth(*pDay, *pYear, *pMonth);
	if ( dayCheckFlag != 0 ) {
		switch( dayCheckFlag )
		{
		case 1:
			{
				throw logic_error( string("Day Part \"") +  strDayPart + "\" is not between 1 ~ 31." );
			}
			break;
		case 2:
			{
				throw logic_error( string("Day Part \"") +  strDayPart + "\" is not between 1 ~ 30, because of given month : " + to_string(*pMonth) );
			}
			break;
		case 3: // none leap Feb
			{
				throw logic_error( string("Day Part \"") +  strDayPart + "\" is not between 1 ~ 28, because of given month is February in none-leap year. " );
			}
			break;
		case 4: // leap Feb
			{
				throw logic_error( string("Day Part \"") +  strDayPart + "\" is not between 1 ~ 29, because of given month is February in leap year. " );
			}
			break;
		default:
			break;
		}
	}
	// cout << "\"" << strDayPart  << "\" => "  << *pDay << endl;

	// hour part
	beg = strPos + 1;
	strPos = strDate.find_first_not_of(space, beg); // skip space
	if ( strPos == notFound ) {
        throw logic_error("Can't found 'hour' Part . ");
	}

	beg = strPos;
	strPos = strDate.find(sep2, beg); // find    ':'
	if ( strPos == notFound ) {
        throw logic_error("Can't found 'hour' Part . ");
	}

	string strHourPart = strDate.substr(beg, strPos - beg);
	if ( static_cast<int>(strHourPart.size() ) > 2 ) {
        throw logic_error( string("Hour Part \"") +  strHourPart + "\" is not an hour format.");
	}
	*pHour = wrap_stoi( strHourPart );
	if ( !(*pHour >=0 && *pHour < 24) ) {
        throw logic_error( string("Hour Part \"") +  strHourPart + "\" is not between 0~23 .");
	}
    // cout << "hour = " << *pHour << endl;

	// minute part
	beg = strPos + 1;
	strPos = strDate.find(sep2, beg);
	if ( strPos == notFound ) {
        throw logic_error("Can't found 'minute' Part . ");
	}

	string strMinutePart = strDate.substr(beg, strPos - beg);
	if ( static_cast<int>(strMinutePart.size() ) > 2 ) {
        throw logic_error( string("Minute Part \"") +  strMinutePart + "\" is not a minute format.");
	}
	*pMin = wrap_stoi( strMinutePart );
	if ( !(*pMin >=0 && *pMin <=59) ) {
        throw logic_error( string("Minute Part \"") +  strMinutePart + "\" is not between 0~59 .");
	}
    // cout << "minute = " << *pMin << endl;
	


	// sec part
	beg = strPos + 1;
	string strSeconds = strDate.substr(beg);
	if ( static_cast<int>(strSeconds.size() ) > 2 ) {
        throw logic_error( string("Second Part \"") +  strSeconds + "\" is not a second format.");
	}
	*pSec = wrap_stoi( strSeconds );
	if ( !( *pSec >=0  &&   *pSec <=59) ) {
		throw logic_error( string("Second Part \"") +  strSeconds + "\" is not between 0~59 .");
	} 
    // cout << "second = " << *pSec << endl;
}


void printUsage()
{

	string text = R"(--------------------------------------------------
Usage :
--------------------------------------------------
    $ deltatime  --date  [--sinceNow]
or 
    $ deltatime  --timestamp  [--sinceNow]
or 
    $ deltatime  --deltatime-1  [--sinceNow] // unit in second
or 
    $ deltatime  --deltatime-2  [--sinceNow] // unit in  hours:minutes:seconds
)";
	cout << text << endl;
}



int main(int argc, char* argv[], char* env[])
{
	if ( argc < 2 ) {
		cerr << "[ERROR] Missing arguments." << endl;
		printUsage();
		return -1;
	}

	int  inputType = 0; // 1:date , 2:unix timestamp since epoch
	bool sinceNow = false;
	bool isDt1Type = false;
	string arg1( argv[1] );
	if ( arg1 == "--date" ) {
		inputType = 1;
	} else if ( arg1 == "--timestamp" ) {
		inputType = 2;
	} else if ( (isDt1Type = (arg1 == "--deltatime-1"))  || arg1 == "--deltatime-2" ) {
		// 3 : unit in second(s)
		// 4 : unit in hh:mm:ss
		inputType = isDt1Type ? 3 : 4;
	} else {
		cout << "[ERROR] Unknow option : \"" << arg1 << "\"" << endl;
		printUsage();
		return -1;
	}

	if ( argc > 2 ) { 
		string arg2( argv[2] );
		if ( arg2 == "--sinceNow" ) {
			sinceNow = true;
		} else {
			cout << "[ERROR] Unknow option : \"" << arg2 << "\"" << endl;
			printUsage();
			return -1;
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// implementation of  calculating  difftime
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////
	try {

		string strStartTime;
		string strEndTime;

		int year1 = 0;
		int mon1 = 0;
		int day1 = 0;
		int h1 = 0;
		int min1 = 0;
		int sec1 = 0;

		int year2 = 0;
		int mon2 = 0;
		int day2 = 0;
		int h2 = 0;
		int min2 = 0;
		int sec2 = 0;

		std::chrono::system_clock::time_point t1;
		std::chrono::system_clock::time_point t2;

		//
		// prepare <Begin> time
		//
		if ( !sinceNow ) {
			if ( inputType == 1 || inputType == 3 || inputType == 4  ) {
				cout << "Please input the <Start> time (YYYY/MM/DD hh:mm:ss) : ";
				getline(cin, strStartTime);
				seq2date(strStartTime, &year1, &mon1, &day1, &h1, &min1, &sec1);

				t1 = makeTimePoint(year1, mon1, day1, h1, min1, sec1);
			} else if ( inputType == 2 ) {
				cout << "Please input the <Start> time (Unix TimeStamp since epoch) : ";
				getline(cin, strStartTime);
				int timetick = wrap_stoi(strStartTime);
				if ( timetick < 0 ) {
					throw logic_error( string("TimeStamp 1 : \"") +  strStartTime + "\" can't be a negative number.");
				}
				time_t startTime = static_cast<time_t>(timetick);
				t1 = std::chrono::system_clock::from_time_t(startTime);

				struct tm* yearMonDayObj = localtime( &startTime );
				year1 = yearMonDayObj->tm_year + SC_YEAR_BEGIN;
				mon1 = yearMonDayObj->tm_mon + 1;
				day1 = yearMonDayObj->tm_mday;
				h1   = yearMonDayObj->tm_hour;
				min1 = yearMonDayObj->tm_min;
				sec1 = yearMonDayObj->tm_sec;
			} 
		} else {
			t1 = system_clock::now();

			time_t nowTick = std::chrono::system_clock::to_time_t(t1);
			struct tm* yearMonDayObj = localtime( &nowTick );
			year1 = yearMonDayObj->tm_year + SC_YEAR_BEGIN;
			mon1 = yearMonDayObj->tm_mon + 1;
			day1 = yearMonDayObj->tm_mday;
			h1   = yearMonDayObj->tm_hour;
			min1   = yearMonDayObj->tm_min;
			sec1   = yearMonDayObj->tm_sec;
		}
		

		//
		// prepare <End> time
		//
		if ( inputType == 1 ) {
			cout << "Please input the < End > time (YYYY/MM/DD hh:mm:ss) : ";
			getline(cin, strEndTime);
			seq2date(strEndTime, &year2, &mon2, &day2, &h2, &min2, &sec2);
			t2 = makeTimePoint(year2, mon2, day2, h2, min2, sec2);
		} else if ( inputType == 2 ) {
			cout << "Please input the < End > time (Unix TimeStamp since epoch) : ";
			getline(cin, strEndTime);
			int timetick = wrap_stoi(strEndTime);
			if ( timetick < 0 ) {
				throw logic_error( string("TimeStamp 2 : \"") +  strEndTime + "\" can't be a negative number.");
			}
			time_t endTime = static_cast<time_t>(timetick);
			t2 = std::chrono::system_clock::from_time_t( endTime );

			struct tm* yearMonDayObj2 = localtime( &endTime );
			year2 = yearMonDayObj2->tm_year + SC_YEAR_BEGIN;
			mon2 = yearMonDayObj2->tm_mon + 1;
			day2 = yearMonDayObj2->tm_mday;
			h2   = yearMonDayObj2->tm_hour;
			min2   = yearMonDayObj2->tm_min;
			sec2   = yearMonDayObj2->tm_sec;
		} else {
			// inputType == 3 || inputType == 4
			if ( inputType == 3 ) {
				int dsecs = 0;
				cout << "Please input the <Duration> time ( unit in second ) : ";
				string strDurationInSecs;
				getline(cin, strDurationInSecs);
				dsecs = wrap_stoi( strDurationInSecs  );

				chrono::seconds dtSetSecs(dsecs);
				t2 = t1 + dtSetSecs;

				time_t t2Tick = std::chrono::system_clock::to_time_t(t2);
				struct tm* yearMonDayObj2 = localtime( &t2Tick );

				year2 = yearMonDayObj2->tm_year + SC_YEAR_BEGIN;
				mon2 = yearMonDayObj2->tm_mon + 1;
				day2 = yearMonDayObj2->tm_mday;
				h2   = yearMonDayObj2->tm_hour;
				min2 = yearMonDayObj2->tm_min;
				sec2 = yearMonDayObj2->tm_sec;
			} else {
				// inputType == 4
				int dhours = 0;
				int dmins = 0;
				int dsecs = 0;

				cout << "Please input the <Duration> time (     hh:mm:ss   ) : ";
				string strDurationInHHMMSS;
				getline(cin, strDurationInHHMMSS); 

				const char space = ' ';
				const char sep2 = ':';

				// hour part
				auto notFound = string::npos;
				decltype(notFound) beg = 0;
				auto strPos = strDurationInHHMMSS.find_first_not_of(space, beg); // skip space
				if ( strPos == notFound ) {
					throw logic_error("Can't found 'hour' Part . ");
				}

				beg = strPos;
				strPos = strDurationInHHMMSS.find(sep2, beg); // find    ':'
				if ( strPos == notFound ) {
					throw logic_error("Can't found 'hour' Part . ");
				}

				string strHourPart = strDurationInHHMMSS.substr(beg, strPos - beg);
				// if ( static_cast<int>(strHourPart.size() ) > 2 ) {
				// 	throw logic_error( string("Hour Part \"") +  strHourPart + "\" is not an hour format.");
				// }
				dhours = wrap_stoi( strHourPart );
				// if ( dhours < 0 ) {
				// 	throw logic_error( string("Hour Part \"") +  strHourPart + "\" can't be a negative number .");
				// }

				// minute part
				beg = strPos + 1;
				strPos = strDurationInHHMMSS.find(sep2, beg);
				if ( strPos == notFound ) {
					throw logic_error("Can't found 'minute' Part . ");
				}

				string strMinutePart = strDurationInHHMMSS.substr(beg, strPos - beg);
				if ( static_cast<int>(strMinutePart.size() ) > 2 ) {
					throw logic_error( string("Minute Part \"") +  strMinutePart + "\" is not a minute format.");
				}
				dmins = wrap_stoi( strMinutePart );
				if ( !( dmins >=0 && dmins <=59 ) ) {
					throw logic_error( string("Minute Part \"") +  strMinutePart + "\" is not between 0~59 .");
				}
				
				// sec part
				beg = strPos + 1;
				string strSeconds = strDurationInHHMMSS.substr(beg);
				if ( static_cast<int>(strSeconds.size() ) > 2 ) {
					throw logic_error( string("Second Part \"") +  strSeconds + "\" is not a second format.");
				}
				dsecs = wrap_stoi( strSeconds );
				if ( !( dsecs >=0  &&   dsecs <=59 ) ) {
					throw logic_error( string("Second Part \"") +  strSeconds + "\" is not between 0~59 .");
				} 

				const std::chrono::hours     oneHour(1);
				const std::chrono::minutes   oneMinute(1);
				const std::chrono::seconds   oneSecond(1);

				t2 = t1 + ( dhours * oneHour + dmins * oneMinute + dsecs * oneSecond);

				time_t t2Tick = std::chrono::system_clock::to_time_t(t2);
				struct tm* yearMonDayObj2 = localtime( &t2Tick );

				year2 = yearMonDayObj2->tm_year + SC_YEAR_BEGIN;
				mon2 = yearMonDayObj2->tm_mon + 1;
				day2 = yearMonDayObj2->tm_mday;
				h2   = yearMonDayObj2->tm_hour;
				min2 = yearMonDayObj2->tm_min;
				sec2 = yearMonDayObj2->tm_sec;
			}

		}



		cout << "====================================================================================================" << endl << endl;
		cout << "Since 1970-1-1 08:00:00 (GMT+8) , date-1: " << year1 << "/" 
													<< setw(2) << setfill('0') << mon1 << "/" 
													<< setw(2) << setfill('0') << day1 << " " 
													<< setw(2) << setfill('0') <<   h1 << ":" 
													<< setw(2) << setfill('0') << min1 << ":" 
													<< setw(2) << setfill('0') << sec1 << ", ";
		auto dtTickInSecond1 = t1 - std::chrono::system_clock::time_point(); // - epoch
		cout << "take " << chrono::duration_cast<chrono::seconds>(dtTickInSecond1).count() << " second(s) ticks. " << endl;


		cout << "Since 1970-1-1 08:00:00 (GMT+8) , date-2: " << year2 << "/" 
													<< setw(2) << setfill('0') << mon2 << "/" 
													<< setw(2) << setfill('0') << day2 << " " 
													<< setw(2) << setfill('0') <<   h2 << ":" 
													<< setw(2) << setfill('0') << min2 << ":" 
													<< setw(2) << setfill('0') << sec2 << ", ";
		auto dtTickInSecond2 = t2 - std::chrono::system_clock::time_point();
		cout << "take " << chrono::duration_cast<chrono::seconds>(dtTickInSecond2).count() << " second(s) ticks. " << endl;

		cout << endl;
		auto deltaTime = t2 - t1;
		auto dtInSec = chrono::duration_cast<chrono::seconds>(deltaTime).count();
		if ( dtInSec < 0 ) {
			cout << "[WARNING] t1 is beyond t2.   Delta = " << dtInSec << endl << endl;
			deltaTime = -deltaTime;
		}

		//
		// calc Days , hours, minutes , sec
		//
		
		// Day(s)
		typedef chrono::duration<int, ratio<3600*24> > Days;
		const Days oneDay(1);
		auto nDays = chrono::duration_cast<Days>(deltaTime).count();

		// Hour(s)
		auto restTime = deltaTime - (nDays * oneDay);
		// cout << "rest time : " << chrono::duration_cast<chrono::seconds>(restTime).count() << " second(s) ticks. " << endl;
		const std::chrono::hours oneHour(1);
		auto nHours = chrono::duration_cast<std::chrono::hours>(restTime).count();

		// Minute(s)
		restTime -= (nHours * oneHour);
		const std::chrono::minutes oneMinute(1);
		auto nMinutes = chrono::duration_cast<std::chrono::minutes>(restTime).count();

		// Second(s)
		restTime -= (nMinutes * oneMinute);
		const std::chrono::seconds oneSecond(1);
		auto nSeconds = chrono::duration_cast<std::chrono::seconds>(restTime).count();

		cout << "It takes ";
		if ( nDays > 0 ) {
			cout << nDays << " day" << (nDays == 1 ? "" : "s") << " ";
		}

		if ( nDays > 0 || nHours > 0 ) {
			cout << nHours << " hour" << (nHours == 1 ? "" : "s") << " ";
		}

		if ( nDays > 0 || nHours > 0 || nMinutes > 0 ) {
			cout << nMinutes << " minute" << (nMinutes == 1 ? "" : "s") << " ";
		}
		cout << setw(2) << setfill('0') << nSeconds << " second" << (nSeconds == 0 ? "" : "s") << " ";

		cout << endl << "====================================================================================================" << endl;

	} catch ( const exception& e ) {
		cerr << "[ERROR] : " << e.what() << endl;
	}

	return 0;
}