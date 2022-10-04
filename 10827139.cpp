# include <iostream>
# include <iomanip>
# include <fstream>
# include <vector>
# include <string>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sstream>
using namespace std;

struct processInfo {
    int id;
    int cpuburst;
    int arrival;
    int priority;
};

vector<processInfo> pInfoList;
vector<string> outputList;

class Scheduler{

    public:

    void SortByArrival( vector<processInfo> &info ) {
        bool hasChanged = false ;
		processInfo temp ;
        for ( int pass = 1; pass < info.size() && !hasChanged ; ++pass ) {		
            hasChanged = true;
            for ( int i = 0; i < info.size()-pass; ++i ) {
                if ( ( info[i].arrival > info[i+1].arrival ) ||
                    ( ( info[i].arrival == info[i+1].arrival ) && ( info[i].id > info[i+1].id ) ) ) {
                    temp = info[i];
                    info[i] = info[i+1];
                    info[i+1] = temp;
                    hasChanged = false ;
                } // if
            } // for
        } // for
    } // SortByArrival
    
    void SortByCPUBurst( vector<processInfo> &info ) {
        bool hasChanged = false ;
		processInfo temp ;
        for ( int pass = 1; pass < info.size() && !hasChanged ; ++pass ) {		
            hasChanged = true;
            for ( int i = 0; i < info.size()-pass; ++i ) {
                if ( info[i].cpuburst > info[i+1].cpuburst ) {
                    temp = info[i];
                    info[i] = info[i+1];
                    info[i+1] = temp;
                    hasChanged = false ;
                } // if
            } // for
        } // for
    } // SortByCPUBurst

    void SortByResponseRatio( vector<processInfo> &info, int time ) {
        bool hasChanged = false ;
		processInfo temp ;
        for ( int pass = 1; pass < info.size() && !hasChanged ; ++pass ) {		
            hasChanged = true;
            for ( int i = 0; i < info.size()-pass; ++i ) {
                float r1 = float( ( time - info[i].arrival ) + info[i].cpuburst ) / info[i].cpuburst;
                float r2 = float( ( time - info[i+1].arrival ) + info[i+1].cpuburst ) / info[i+1].cpuburst ;
                if ( r1 < r2 ) {
                    temp = info[i];
                    info[i] = info[i+1];
                    info[i+1] = temp;
                    hasChanged = false ;
                } // if
            } // for
        } // for
    } // SortByResponseRatio

    void SortByPriority( vector<processInfo> &info ) {
        bool hasChanged = false ;
		processInfo temp ;
        for ( int pass = 1; pass < info.size() && !hasChanged ; ++pass ) {		
            hasChanged = true;
            for ( int i = 0; i < info.size()-pass; ++i ) {
                if ( info[i].priority > info[i+1].priority ) {    
                    temp = info[i];
                    info[i] = info[i+1];
                    info[i+1] = temp;
                    hasChanged = false ;
                } // if
            } // for
        } // for
    } // SortByPriority

    void SortByID( vector<processInfo> &info ) {
        bool hasChanged = false ;
		processInfo temp ;
        for ( int pass = 1; pass < info.size() && !hasChanged ; ++pass ) {		
            hasChanged = true;
            for ( int i = 0; i < info.size()-pass; ++i ) {
                if ( info[i].id > info[i+1].id ) {
                    temp = info[i];
                    info[i] = info[i+1];
                    info[i+1] = temp;
                    hasChanged = false ;
                } // if
            } // for
        } // for
    } // SortByID

    void SetGanttName( int id, string &ganttChart ) {
        char ch;
        if ( id >= 0 && id <= 9 )
            ch = '0' + id;
        else if ( id-10 >= 0 && id-10 <= 25 )
            ch = 'A' + (id-10);

        ganttChart += ch;
    } // SetGanttName

    char SetIDName( int id ) {
        char ch;
        if ( id >= 0 && id <= 9 )
            ch = '0' + id;
        else if ( id-10 >= 0 && id-10 <= 25 )
            ch = 'A' + (id-10);

        return ch;
    } // SetIDName

    void FCFS() {
        vector<processInfo> waitingQueue;
        vector<processInfo> infoList = pInfoList;
        string ganttChart = "";
        int cpuEndTime = 0;
        for (int time = 0; infoList.size() != 0 || waitingQueue.size() != 0; time++) {
            while ( infoList.size() > 0 && infoList.front().arrival == time ) {
                waitingQueue.push_back( infoList.front() );
                infoList.erase( infoList.begin() );
            } // while

            if ( time >= cpuEndTime ) {        // no process in CPU
                if ( waitingQueue.size() > 0 ) {       // have process in waiting queue
                    cpuEndTime = time + waitingQueue.front().cpuburst;
                    for ( int i = 0; i < waitingQueue.front().cpuburst; i++ )
                        SetGanttName( waitingQueue.front().id, ganttChart );
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
                else
                    ganttChart += '-';
            } // if
        } // for
        outputList.push_back( ganttChart );
    } // FCFS

    void RR( int time_slice ) {
        vector<processInfo> waitingQueue;
        vector<processInfo> infoList = pInfoList;
        string ganttChart = "";
        processInfo pInCPU;
        pInCPU.id = -1;
        int cpuEndTime = 0;
        for (int time = 0; infoList.size() != 0 || waitingQueue.size() != 0 || pInCPU.cpuburst > 0; time++) {
            while ( infoList.size() > 0 && infoList.front().arrival == time ) {
                waitingQueue.push_back( infoList.front() );
                infoList.erase( infoList.begin() );
            } // while

            if ( time >= cpuEndTime ) {        // no process in CPU
                if ( waitingQueue.size() > 0 ) {       // have process in waiting queue
                    if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 )
                        waitingQueue.push_back( pInCPU );

                    if ( waitingQueue.front().cpuburst >= time_slice )
                        cpuEndTime = time + time_slice;
                    else
                        cpuEndTime = time + waitingQueue.front().cpuburst;

                    pInCPU = waitingQueue.front();
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
            } // if

            if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 )
                SetGanttName( pInCPU.id, ganttChart );
            else
                ganttChart += '-';
            pInCPU.cpuburst--;
        } // for
        outputList.push_back( ganttChart );
    } // RR

    void SRTF() {
        vector<processInfo> waitingQueue;
        vector<processInfo> infoList = pInfoList;
        string ganttChart = "";
        processInfo pInCPU;
        pInCPU.id = -1;
        for (int time = 0; infoList.size() != 0 || waitingQueue.size() != 0 || pInCPU.cpuburst > 0; time++) {
            while ( infoList.size() > 0 && infoList.front().arrival == time ) {
                waitingQueue.push_back( infoList.front() );
                infoList.erase( infoList.begin() );
            } // while

            if ( waitingQueue.size() > 0 )
                SortByCPUBurst( waitingQueue );

            if ( pInCPU.cpuburst <= 0 || pInCPU.id == -1 || pInCPU.cpuburst > waitingQueue.front().cpuburst ) {        // no process in CPU
                if ( waitingQueue.size() > 0 ) {       // have process in waiting queue
                    if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 )
                        waitingQueue.push_back( pInCPU );
                    pInCPU = waitingQueue.front();
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
            } // if
            
            if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 )
                SetGanttName( pInCPU.id, ganttChart );
            else
                ganttChart += '-';
            pInCPU.cpuburst--;
        } // for
        outputList.push_back( ganttChart );
    } // SRTF

    void PPRR( int time_slice ) {
        vector<processInfo> waitingQueue;
        vector<processInfo> infoList = pInfoList;
        string ganttChart = "";
        processInfo pInCPU;
        pInCPU.id = -1;
        int cpuEndTime = 0;
        for (int time = 0; infoList.size() != 0 || waitingQueue.size() != 0 || pInCPU.cpuburst > 0; time++) {
            while ( infoList.size() > 0 && infoList.front().arrival == time ) {
                waitingQueue.push_back( infoList.front() );
                SortByPriority( waitingQueue );
                infoList.erase( infoList.begin() );
            } // while

            if ( time >= cpuEndTime ) {        // no process in CPU
                if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 ) {
                    waitingQueue.push_back( pInCPU );
                    SortByPriority( waitingQueue );
                } // if
                if ( waitingQueue.size() > 0 ) {
                    if ( waitingQueue.front().cpuburst >= time_slice )
                        cpuEndTime = time + time_slice;
                    else
                        cpuEndTime = time + waitingQueue.front().cpuburst;
                    pInCPU = waitingQueue.front();
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
            } // if
            else if ( waitingQueue.size() > 0 && waitingQueue.front().priority < pInCPU.priority ) {
                if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 ) {
                    waitingQueue.push_back( pInCPU );
                    SortByPriority( waitingQueue );
                    if ( waitingQueue.front().cpuburst >= time_slice )
                        cpuEndTime = time + time_slice;
                    else
                        cpuEndTime = time + waitingQueue.front().cpuburst;
                    pInCPU = waitingQueue.front();
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
            } // else if

            if ( pInCPU.cpuburst > 0 && pInCPU.id != -1 )
                SetGanttName( pInCPU.id, ganttChart );
            else
                ganttChart += '-';
            pInCPU.cpuburst--;
        } // for
        outputList.push_back( ganttChart );
    } // PPRR

    void HRRN() {
        vector<processInfo> waitingQueue;
        vector<processInfo> infoList = pInfoList;
        string ganttChart = "";
        int cpuEndTime = 0;
        for (int time = 0; infoList.size() != 0 || waitingQueue.size() != 0; time++) {
            while ( infoList.size() > 0 && infoList.front().arrival == time ) {
                waitingQueue.push_back( infoList.front() );
                infoList.erase( infoList.begin() );
            } // while

            if ( time >= cpuEndTime ) {        // no process in CPU
                if ( waitingQueue.size() > 0 ) {       // have process in waiting queue
                    SortByResponseRatio( waitingQueue, time );
                    cpuEndTime = time + waitingQueue.front().cpuburst;
                    for ( int i = 0; i < waitingQueue.front().cpuburst; i++ )
                        SetGanttName( waitingQueue.front().id, ganttChart );
                    waitingQueue.erase( waitingQueue.begin() );
                } // if
                else
                    ganttChart += '-';
            } // if
        } // for
        outputList.push_back( ganttChart );
    } // HRRN

    void ALL( int time_slice ) {
        FCFS();
        RR(time_slice);
        SRTF();
        PPRR(time_slice);
        HRRN();
    } // ALL

    void SetOutput( int method, string inputFname ) {
        ofstream fout;
        vector<string> methodName;
        string outputFname = "out_" + inputFname + ".txt";
        int waitingtime = 0, maxttime = 0;
        fout.open( outputFname );
        if ( fout.is_open() ) {
            if ( method == 1 || method == 6 )
                methodName.push_back( "FCFS" );
            if ( method == 2 || method == 6  )
                methodName.push_back( "RR" );
            if ( method == 3 || method == 6  )
                methodName.push_back( "SRTF" );
            if ( method == 4 || method == 6  )
                methodName.push_back( "PPRR" );
            if ( method == 5 || method == 6  )
                methodName.push_back( "HRRN" );

            SortByID( pInfoList );
            if ( method >= 1 && method <= 5 ) {
                if ( method == 4 )
                    fout << "Priority RR" << endl;
                else 
                    fout << methodName.front() << endl;
                fout << "==" << setw(12) << methodName.front() << "==" << endl;
                fout << outputList.front() << endl;
                fout << "===========================================================" << endl << endl << "Waiting Time" << endl;
                fout << "ID" << "\t" << methodName.front() << endl;
                fout << "===========================================================" << endl;
                for ( int i = 0; i < pInfoList.size(); i++ ) {
                    fout << pInfoList[i].id << "\t" ;
                    int finish = outputList.front().size();
                    while ( SetIDName( pInfoList[i].id ) != outputList.front()[finish] ) {
                        finish--;
                    } // while
                    fout << finish - pInfoList[i].arrival - pInfoList[i].cpuburst + 1 << endl;
                    waitingtime +=  finish - pInfoList[i].arrival - pInfoList[i].cpuburst + 1;
                } // for
                cout << "Average Waiting Time: " << float(waitingtime) / pInfoList.size() << endl;
                fout << "===========================================================" << endl << endl << "Turnaround Time" << endl;
                fout << "ID" << "\t" << methodName.front() << endl;
                fout << "===========================================================" << endl;
                for ( int i = 0; i < pInfoList.size(); i++ ) {
                    fout << pInfoList[i].id << "\t" ;
                    int finish = outputList.front().size();
                    while ( SetIDName( pInfoList[i].id ) != outputList.front()[finish] ) {
                        finish--;
                    } // while
                    fout << finish - pInfoList[i].arrival + 1 << endl;
                    maxttime +=  finish - pInfoList[i].arrival + 1;
                } // for
                cout << "Average Turnaround Time: " << float(maxttime) / pInfoList.size() << endl;
                fout << "===========================================================" << endl << endl;
            } // if
            else if ( method == 6 ) {
                fout << "All" << endl;
                for ( int i = 0; i < 5; i++ ) {
                    fout << "==" << setw(12) << methodName[i] << "==" << endl;
                    fout << outputList[i] << endl;
                } // for
                fout << "===========================================================" << endl << endl << "Waiting Time" << endl;
                fout << "ID" << "\t" << "FCFS" << "\t" << "RR" << "\t" << "SRTF" << "\t" << "PPRR" << "\t" << "HRRN" << endl;
                fout << "===========================================================" << endl;
                for ( int i = 0; i < pInfoList.size(); i++ ) {
                    fout << pInfoList[i].id;
                    for ( int j = 0; j < outputList.size(); j++ ) {
                        int finish = outputList[j].size();
                        while ( SetIDName( pInfoList[i].id ) != outputList[j][finish] ) {
                            finish--;
                        } // while
                        fout << "\t" << finish - pInfoList[i].arrival - pInfoList[i].cpuburst +1;
                    } // for
                    fout << endl;
                } // for
                fout << "===========================================================" << endl << endl << "Turnaround Time" << endl;
                fout << "ID" << "\t" << "FCFS" << "\t" << "RR" << "\t" << "SRTF" << "\t" << "PPRR" << "\t" << "HRRN" << endl;
                fout << "===========================================================" << endl;
                for ( int i = 0; i < pInfoList.size(); i++ ) {
                    fout << pInfoList[i].id;
                    for ( int j = 0; j < outputList.size(); j++ ) {
                        int finish = outputList[j].size();
                        while ( SetIDName( pInfoList[i].id ) != outputList[j][finish] ) {
                            finish--;
                        } // while
                        fout << "\t" << finish - pInfoList[i].arrival + 1;
                    } // for
                    fout << endl;
                } // for
                fout << "===========================================================" << endl << endl;
            } // else if
            fout.close();
        } // if
        
    } // void

} ; // Scheduler

int main( int argc, char* argv[] ) {
    ifstream fin;
    processInfo tempInfo;
    string inputFname, s, temps;
    Scheduler sched;
    int method = 0, time_slice = 0;
    cout << "\nInput a file name (0: end of program): " << endl;
    cin >> inputFname;
    while ( inputFname != "0" ) {
        fin.open( inputFname + ".txt" );
        if ( fin.is_open() ) {
            fin >> method >> time_slice;
            getline(fin, s);        // read '\n'
            getline(fin, s);        // read title line
            
            while ( getline(fin, s) && s != "" ) {
                for( int i = 0, j = 1; i <= s.length(); i++) {
                    while ( i < s.length() && ( s[i] == ' ' || s[i] == '\t' ) ) {
                        i++;
                    } // while

                    while ( s[i] != '\t' && s[i] != ' ' && i < s.length() ) {
                        temps += s[i];
                        i++;
                    } // while
                    
                    if ( j == 1 ) tempInfo.id = atoi(temps.c_str());
                    else if ( j == 2 ) tempInfo.cpuburst = atoi(temps.c_str());
                    else if ( j == 3 ) tempInfo.arrival = atoi(temps.c_str());
                    else if ( j == 4 ) tempInfo.priority = atoi(temps.c_str());

                    j++;
                    temps.clear();
                } // for
                pInfoList.push_back( tempInfo );
            } // while
            fin.close();

            sched.SortByArrival( pInfoList );

            if ( method == 1 )
                sched.FCFS();
            else if ( method == 2 )
                sched.RR( time_slice );
            else if ( method == 3 )
                sched.SRTF();
            else if ( method == 4 )
                sched.PPRR( time_slice );
            else if ( method == 5 )
                sched.HRRN();
            else if ( method == 6 )
                sched.ALL( time_slice );

            sched.SetOutput( method, inputFname );
        } // if
        else
            cout << "File name input error!" << endl << endl;
        cout << "\nInput a file name (0: end of program): " << endl;
        cin >> inputFname;
        pInfoList.clear();
        outputList.clear();
    }// while

} // main