#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

/*
    It must be calc value in compile time   2/3 => std::ratio<2,3>
    int a,b;
    cin >> a >> b;
    std::ratio<a,b>  // Compile-Error , because a and b can't be evaluate at compile time
*/
#include <ratio>

using namespace std;

/*
namespace std {
    namespace chrono {
        typedef duration<signed int-type >= 64 bits,nano>          nanoseconds;  // 1 nanosecond  = 1000 microsecond(s)
        typedef duration<signed int-type >= 55 bits,micro>         microseconds; // 1 microsecond = 1000 millisecond(s)
        typedef duration<signed int-type >= 45 bits,milli>         milliseconds; // 1 millisecond = 1000 second(s)
        typedef duration<signed int-type >= 35 bits      >         seconds;
        typedef duration<signed int-type >= 29 bits,ratio<60> >    minutes;
        typedef duration<signed int-type >= 23 bits,ratio<3600> >  hours;
    }
}

A duration of time is defined as a specific number of ticks over a time unit. 
One example is a duration such as "3 minutes" (3 ticks of a "minute")

A duration is a combination of a value representing the number of ticks and a fraction representing the unit in seconds (default unit is in second )

std::chrono::duration<int>     dur(3);      // 3 ticks of 1 second


*/

void test1()
{
    // Core :
    // 1. cast
    //       chrono::durationTypeA  val_a(tickCount);
    //       chrono::durationTypeB  val_b = std::chrono::duration_cast<chrono::durationTypeB>(val_a);


    std::chrono::duration<int>     dur(120); // 120 secs =>  2 Min ( default unit is second )
    cout << "dur.count() = " << dur.count() << endl;
    std::chrono::minutes  equal_N_min = std::chrono::duration_cast<std::chrono::minutes>(dur);
    cout << "equal_N_Min.count() = " << equal_N_min.count() << endl;


    std::chrono::hours   aDay(24);  // 1 Day
    std::chrono::minutes ms = std::chrono::duration_cast<std::chrono::minutes>(aDay);
    std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(aDay);
    cout << "minutes.count = " << ms.count() << " == 1 day" << endl;
    cout << "seconds.count = " << ss.count() << " == 1 day" << endl;

    // reset it as 1 second
    ss = static_cast<std::chrono::seconds>(1);
    std::chrono::milliseconds millisec = std::chrono::duration_cast<std::chrono::milliseconds>(ss);
    std::chrono::microseconds microsec = std::chrono::duration_cast<std::chrono::microseconds>(ss);
    std::chrono::nanoseconds nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(ss);
    cout << millisec.count() << " milliseconds ==  1 second " << endl;
    cout << microsec.count() << " microseconds ==  1 second " << endl;
    cout << nanosec.count()  << " nanoseconds ==  1 second " << endl;
}


void test2()
{
    // A duration is a combination of a value representing the number of ticks and a fraction representing the unit in seconds (default unit is in second )
    std::chrono::duration<int>    twentySeconds(20); // default unit is in second

    // The following code means that 1 tick stands for std::ratio<60> or a float number
    // the tick count can be either an interger or a float such as 1.5 or 0.5 2.0f/3 , 0.5 * 60(seconds) = 30(secs) = halfAminute;
    //
    // 1st type indicate tick's type is interger type or float
    // 2nd number indicate the accurate value for per unit to calc (in sec)
    std::chrono::duration<double, std::ratio<60> > halfAminute(0.5);
    std::chrono::duration<long, std::ratio<1,1000> > oneMilliseconds(1); // 1/1000 sec = 1 milliseconds
    (void)twentySeconds;
    (void)halfAminute;
    (void)oneMilliseconds;

    // 2 durations operation
    std::chrono::seconds d1(42);
    std::chrono::milliseconds d2(10);

    // it will cast d1 from  seconds into  milliseconds , because  1 seconds > 1 milliseconds 
    auto diffDuration =  d1 - d2; // so the result will be a type of milliseconds
    cout << "42(secs) - 10(milliseconds) = diffDuration = " << diffDuration.count() << endl; // 41990

    // d1 + d2
    // d1 - d2
    // times * duration
    // d * times(a number)
    // d / times(a number)
    // d1 / d2 : how many times for duration2 are there in duration1 ? 
    // d % val
    // d1 % d2
    // d1 == d2
    // d1 != d2
    // d1 < d2
    // d1 <= d2
    // d1 > d2
    // d1 >= d2
    // ++d1;     : increase d1 by 1 tick
    // --d1;     : decrease d1 by 1 tick
    // d += d1
    // d -= d1
    // d *= times;
    // d /= times;
    // d %= times;
    // d1 %= d2;
    std::chrono::milliseconds d_ms1(6000);
    auto seprate1 = d_ms1 / 2.0f;
    auto seprate2 = d_ms1 / 0.5f;
    cout << "seprate1.count = " << seprate1.count() << endl; // 3000  in milliseconds
    cout << "seprate2.count = " << seprate2.count() << endl; // 12000 in milliseconds

    std::chrono::seconds sec222;    // the tick is undefined at run time
    cout << "sec222.count = " << sec222.count() << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class V, class R>
ostream& operator << (std::ostream& s, const std::chrono::duration<V,R> d)
{
    s << "[" << d.count() << " of "
                          << R::num << "/"
                          << R::den << "]";
    return s;
}

void test3()
{
    std::chrono::milliseconds ms(7255042);

    // split it into hours , minutes , seconds and  milliseconds
    std::chrono::hours hh = std::chrono::duration_cast<std::chrono::hours>(ms);
    std::chrono::minutes mm = std::chrono::duration_cast<std::chrono::hours>(ms % chrono::hours(1) );
    std::chrono::seconds ss = std::chrono::duration_cast<std::chrono::seconds>(ms % chrono::minutes(1) );
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds>(ms % chrono::seconds(1) );


    cout << "raw: " << ms.count() << " milliseconds " << endl;
    cout << "=    " << hh << "::" << mm << "::"                 // use  =>     ostream& operator << (std::ostream& s, const std::chrono::duration<V,R> d)
                    << ss << "::" << msec << endl;

    cout << "=    " << setfill('0') << setw(2) << hh.count() << " hours && "
                                    << setw(2) << mm.count() << " minuts && "
                                    << setw(2) << ss.count() << " seconds && "
                                    << setw(2) << msec.count() << " milliseconds" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


template <typename C>
void printClockData()
{
    cout << "- precision: ";
    // if time unit is less than or equal to one millisecond
    typedef typename C::period P;   // type of time unit
    if ( ratio_less_equal<P,milli>::value ) {
        // convert to and print as milliseconds
        typedef typename ratio_multiply<P,kilo>::type TT;
        cout << fixed  << double(TT::num) / TT::den
             << " milliseconds" 
             << endl;
    } else {
        // print as seconds
        cout << fixed << double(P::num) / P::den 
             << " seconds"
             << endl;
    }

    cout << "- is_steady: " << boolalpha << C::is_steady << endl;
}


void studyChronoClock()
{
    // A clock :
    //    defines an epoch and a tick peroid.
    //    e.g. 
    //        1. a clock might tick in milliseconds since the Unix epoch( January 1, 1970) 
    //        2. tick in nanoseconds since the start of the program

    cout << "system_clock: " << endl;
    // system_clock is the most useful type among all of the clock type
    printClockData<std::chrono::system_clock>();
    cout << endl;

    cout << "high_resolution_clock: " << endl;
    printClockData<std::chrono::high_resolution_clock>();
    cout << endl;

    cout << "steady_clock: " << endl;
    printClockData<std::chrono::steady_clock>();
    cout << endl;

}




void tryToImplementATimer(char* arg)
{
    // the following command will simulator a timer for 10 ticks every seconds
    // $ ./main 10  
    //
    int tick = 0;
    int maxCnt = atoi(arg);
    // std::chrono::milliseconds delta_sec(500);
    std::chrono::seconds delta_sec(1); // tick per second
    std::chrono::seconds delta_sec2(2); // tick every 2 second
    auto dt = delta_sec;
    (void)delta_sec2;

    // std::chrono::system_clock::now()  ==>    Get the current system time
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    cout << "Start a timer : " << endl;
    cout << "====================================================================================================" << endl;

    while ( tick < maxCnt ) {
        auto nowTime = std::chrono::system_clock::now();
        auto diff =  nowTime - startTime; 
        if ( diff >=  dt ) {
            startTime = nowTime;
            ++tick;
            cout << "\t" << (tick * dt.count()) << "  seconds passed ... " << endl;
        }
    }
    cout << "====================================================================================================" << endl;
    cout << "Timer Finished for max sec tick " <<  endl;
    // std::chrono::system_clock::now()
}



string timept2string(const std::chrono::system_clock::time_point& tp, int flag)  
{
    // convert to system time
    // std::time_t type  ==>   must in include  <ctime>
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string timestr; 
    if ( flag == 1 )  {
        // convert to human readable calendar time  string
        // std::ctime(tm)  <=>  std::asctime( localtime(tm) );
        timestr = std::ctime(&t);   // it will take time-zone into account
    } else {
        timestr = std::asctime(gmtime(&t));    // it will <NOT> take time-zone into account
    }
    timestr.resize(timestr.size() - 1);  // skip trailing newline
    return timestr;
};

void study_TimePoint(int tag)
{
    /*
     * A timepoint represents a specific point in time by associating a positive or negative duration to a give clock
       There are 2 key factor
        ----------------------------------------------------------------------------------------------------
           1. a given clock
           2. a +/-  duration by the give clock (which is metioned in 1)
        ----------------------------------------------------------------------------------------------------


        namespace std {
           namespace chrono {

              template<typename Clock, typename Duration = typename Clock::duration>
              class time_point;   
           }
        }
    */




    // print the epoch of this system clock
    /*
     *          chrono::system_clock::time_point
     *     <=>  chrono::time_point<chrono::system_clock>
    */
    std::chrono::system_clock::time_point tp;
    cout << "epoch: " << timept2string(tp,tag) << endl;

    // print current time
    tp = std::chrono::system_clock::now();
    cout << "now: " << timept2string(tp,tag) << endl;

    // print minimum time of this system_clock
    tp = std::chrono::system_clock::time_point::min();
    cout << "min: " << timept2string(tp,tag) << endl;

    // print maximum time of this system_clock
    tp = std::chrono::system_clock::time_point::max();
    cout << "max: " << timept2string(tp,tag) << endl;


}


void study_TimePointCalc(int tag)
{
    /*
     
       time_point t;    // epoch
       time_point t(d); // a time point having a duration after epoch

       // Convert tp into a timepoint with the given clock C and duration D
       time_point_cast<C,D>(tp); // a time point having a duration after epoch


       tp += d; // => timepoint
       tp -= d; // => timepoint

       tp + d;  // => a new timepoint
       d + tp;  // => a new timepoint

       tp - d;  // => a new timepoint
       // d - pt;  // !!!![ERROR]!!! undefined behavior

       tp1 - tp2;   // duration
       tp1 == tp2;  // bool
       tp1 != tp2;  // bool
       
       tp1 < tp2;  // bool
       tp1 <= tp2;  // bool

       tp1 > tp2;  // bool
       tp1 >= tp2;  // bool
        
       tp.time_since_epoch();   // duration
    */

    // define One Day Unit
    typedef chrono::duration<int, ratio<3600*24> > Days;

    std::chrono::system_clock::time_point tp;
    cout << "epoch: " << timept2string(tp,tag) << endl;

    // add  ond day, 23 hours and 55 minutes
    tp += Days(1) + chrono::hours(23) + chrono::minutes(55);
    cout << "later: " << timept2string(tp,tag) << endl;

    // process difference from epoch in minutes and days
    auto diff = tp - std::chrono::system_clock::time_point();
    cout << "diff:  "
         << chrono::duration_cast<chrono::minutes>(diff).count()
         << "  minute(s)"
         << endl;
    Days days = chrono::duration_cast<Days>(diff);
    cout << "diff:  " << days.count() << "  day(s)" << endl;


    // substract one year ( hoping it is valid and not a leap year)
    tp -= chrono::hours(24*365);
    cout << "-1 year:    " << timept2string(tp,tag) << endl;
    
    // substract 50 year ( hoping it is valid and ignoring leap year)
    tp -= chrono::duration<int, ratio<3600*24*365>>(50);
    cout << "1st.   -50 year:    " << timept2string(tp,tag) << endl;

    // substract 50 year ( hoping it is valid and ignoring leap year)
    tp -= chrono::duration<int, ratio<3600*24*365>>(50);
    cout << "2nd.  -50 year:    " << timept2string(tp,tag) << endl;

}

inline
chrono::system_clock::time_point 
makeTimePoint(int year,int mon, int day, int hour,int min, int sec = 0)
{
    struct std::tm tm;
    tm.tm_year = year-1900; // year since 1900
    tm.tm_mon = mon-1;   // 0 ~ 11 => [Jan ~ Dec]
    tm.tm_mday = day;  // 1 .. 31
    tm.tm_hour = hour; // [0,23]
    tm.tm_min = min;  // [0,59]
    tm.tm_sec = sec; // [0,59] or [0,60] 60  for leap seconds
    tm.tm_isdst = -1;
    std::time_t tt = std::mktime(&tm);

    if ( tt == -1 ) {
        throw "no valid system time";
    }

    return std::chrono::system_clock::from_time_t(tt);
}

void testMakeTime(int tag)
{
    try {
        auto tp1 = makeTimePoint(2010,1,1,0,0);
        cout << timept2string(tp1,tag) << endl;

        auto tp2 = makeTimePoint(2011,5,23,13,44);
        cout << timept2string(tp2,tag) << endl;

        // auto tp3 = makeTimePoint(2265,5,23,13,44);
        // cout << timept2string(tp3,tag) << endl;
    } catch (...)  {
        cout << "[Error]!!! " << endl;
    }
}


int main(int argc, char* argv[], char* env[])
{
    test1();
    cout << "====================================================================================================" << endl;
    test2();
    cout << "====================================================================================================" << endl;
    test3();
    cout << "====================================================================================================" << endl;
    studyChronoClock();
    cout << "====================================================================================================" << endl;
    cout << "1. use ctime(...); take time-zone into account." << endl;
    study_TimePoint(1); // 1 => take  time-zone into account
    cout << "====================================================================================================" << endl;
    cout << "2. use asctime(...);  Do <NOT> take time-zone into account (UTC time). " << endl;
    study_TimePoint(2); // 2 => <NOT> take time-zone into account (UTC time)
    cout << "====================================================================================================" << endl;
    study_TimePointCalc(1);
    cout << "====================================================================================================" << endl;
    testMakeTime(1);

    if ( argc > 1 ) {
        // input a countdown number in seconds
        tryToImplementATimer(argv[1]);
    } 



    return 0;
}

