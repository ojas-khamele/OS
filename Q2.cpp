#include<iostream>
#include<bits/stdc++.h>
#include<algorithm>

using namespace std;

int processID = 0;
int runTime = 0;
int contextSwitch = 0;

class process{
    private:
        int response_time;
        int burst_time;
        int cpu_time;
        int process_id;
        int arrival_time;
        int completion_time;
        int currIO_time;

    public:
        process(int id, int arrival_time) {
            process_id = id;
            this->arrival_time = arrival_time;
            burst_time = INT_MAX;
            completion_time = 0;
            response_time = -1;
            cpu_time = 0;
            currIO_time = 0;
        }

        int get_at() {
            return arrival_time;
        }

        void set_bt(int bt) {
            burst_time = bt;
        }

        int get_bt() {
            return burst_time;
        }

        int get_process_id() {
            return process_id;
        }

        void set_completion_time(int time) {
            completion_time = time;
        }

        int get_completion_time() {
            return completion_time;
        }

        void set_response_time(int time) {
            response_time = time;
        }

        int get_response_time() {
            return response_time;
        }

        int set_cpu_time(int t) {
            cpu_time = t;
        }

        int get_cpu_time() {
            return cpu_time;
        }

        int get_IO() {
            return currIO_time;
        }

        void set_IO(int t) {
            currIO_time = t;
        }
};

vector<process*> processList;
vector<process*> ioList;

queue<process*> q0;
queue<process*> q1;
queue<process*> q2;

int timeSlice0 = 9;
int timeSlice1 = 6;
int timeSlice2 = 3;

void show_menu() {
    cout<<"1 - Add process"<<endl;
    cout<<"2 - Switch Process to IO"<<endl;
    cout<<"3 - Resume process from IO"<<endl;
    cout<<"4 - Terminate process"<<endl;
    cout<<"5 - Display processes"<<endl;
    cout<<"6 - Show Status"<<endl;
    cout<<"Press any other key to exit : ";  
}

void add_process() {
    process* newProc = new process(processID++, runTime);
    
    cout<<"Set burst time?(y/n) : ";
    char c;
    cin>>c;

    if(c=='y') {
        int bt;
        cout<<"Burst time : ";
        cin>>bt;

        newProc->set_bt(bt);
    }

    int qNo;
    cout<<"Enter Queue Number : ";
    cin>>qNo;

    if(qNo==2) q2.push(newProc);
    else if(qNo==1) q1.push(newProc);
    else q0.push(newProc);

    processList.push_back(newProc);
}

void switch_process_for_IO() {
    cout<<"Enter process ID for IO : ";
    int id;
    cin>>id;
    
    cout<<"Enter time units for IO : ";
    int time;
    cin>>time;

    processList[id]->set_IO(time);
    ioList.push_back(processList[id]);
}

void resume_process_from_IO(int id) {
    processList[id]->set_IO(0);
}

void terminate_process() {
    int id;
    cout<<"Enter process ID to terminate : ";
    cin>>id;

    if(id>=processList.size()) cout<<"Invalid ID"<<endl;
    else {
        processList[id]->set_bt(processList[id]->get_cpu_time());
        processList[id]->set_completion_time(runTime);
        if(processList[id]->get_response_time()==-1) processList[id]->set_response_time(0);
    }
}

void printDetails(process p) {
    cout<<"Process id = "<< p.get_process_id()<<endl;
    cout<<"Burst Time = "<<p.get_bt()<<endl;
    cout<<"Cpu Time = "<<p.get_cpu_time()<<endl<<endl;
}

void displayProcesses() {
    queue<process*> q;
    q = q2;

    cout<<"Processes in 2nd Queue :"<<endl;
    while(!q.empty()) {
        printDetails(*q.front());
        q.pop();
    }

    q = q1;
    cout<<"Processes in 1st Queue :"<<endl;
    while(!q.empty()) {
        printDetails(*q.front());
        q.pop();
    }

    q = q0;
    cout<<"Processes in 0th Queue :"<<endl;
    while(!q.empty()) {
        printDetails(*q.front());
        q.pop();
    }
}

void show_summary() {
    cout<<endl<<"*********************************************************************"<<endl;
    cout<<"Context Switches = "<<contextSwitch<<" Run Time = "<<runTime;
    cout<<endl<<"*********************************************************************"<<endl;
    cout<<endl;
    for(int i=0; i<processList.size(); i++) {
        cout<<"Process id = "<<processList[i]->get_process_id()<<"             Arrival = "<<processList[i]->get_at();
        cout<<"         Burst Time = "<<processList[i]->get_bt()<<endl;
        if(processList[i]->get_completion_time()>0) {
            int tat = processList[i]->get_completion_time() - processList[i]->get_at();
            cout<<"Turn Around = "<<tat<<"       "<<"Waiting = "<<tat - processList[i]->get_bt()<<"      "<<"Response Time = "<<processList[i]->get_response_time();
            cout<<endl<<"Completion Time = "<<processList[i]->get_completion_time();
        }
        else cout<<"Process Not Complete";
        
        if(processList[i]->get_IO()!=0) cout<<"     Waiting for IO";
        cout<<endl<<endl;
    }
}

void perform_task(int opt) {
    switch(opt) {
        case 1: 
            add_process();
            break;

        case 2:
            switch_process_for_IO();
            break;

        case 3:
            cout<<"Enter process ID : ";
            int id;
            cin>>id;
            resume_process_from_IO(id);
            break;

        case 4: 
            terminate_process();
            break;
 
        case 5:
            displayProcesses();
            break;

        case 6:
            show_summary();
            break;
    }
}

int main() {

    int opt;
    int prev = -1;
    int procTime = 0;

    do {
        show_menu();
        cin>>opt;
        if(opt>6 || opt<1) break;

        bool flg = true;
        while(flg) {
            perform_task(opt);
            cout<<"Continue?(y/n) : ";
            
            char c;
            cin>>c;
            if(c=='n') flg = false;
            else {
                cout<<"Next operation : ";
                cin>>opt;
                cout<<endl;
            }
        }        
         
        runTime++;

        bool found = false;

        while(!found) {
            
            int size = q2.size();
            for(int i=0; i<size; i++) {
                if(q2.front()->get_IO()>0) {
                    q2.push(q2.front());
                    q2.pop();
                }

                else if(q2.front()->get_cpu_time()==q2.front()->get_bt()) {
                    q2.pop();
                }

                else {
                    if(q2.front()->get_process_id()!=prev) {
                        prev = q2.front()->get_process_id();
                        contextSwitch++;
                        procTime=0;
                    }
                    procTime++;
                    q2.front()->set_cpu_time(q2.front()->get_cpu_time()+1);
                    
                    if(q2.front()->get_response_time()==-1) q2.front()->set_response_time(runTime - q2.front()->get_at()-1);
                    if(q2.front()->get_cpu_time()==q2.front()->get_bt()) {
                        q2.front()->set_completion_time(runTime);
                        q2.pop();
                    }
                    else if (procTime==timeSlice2) {
                        q2.push(q2.front());
                        q2.pop();
                        procTime = 0;
                    }
                    found = true;
                    break;
                }
            }
            
            if(found) break;

            size = q1.size();
            for(int i=0; i<size; i++) {
                if(q1.front()->get_IO()>0) {
                    q1.push(q1.front());
                    q1.pop();
                }

                else if(q1.front()->get_cpu_time()==q1.front()->get_bt()) {
                    q1.pop();
                }

                else {
                    if(q1.front()->get_process_id()!=prev) {
                        prev = q1.front()->get_process_id();
                        contextSwitch++;
                        procTime=0;
                    }
                    procTime++;
                    q1.front()->set_cpu_time(q2.front()->get_cpu_time()+1);
                    
                    if(q1.front()->get_response_time()==-1) q1.front()->set_response_time(runTime - q1.front()->get_at()-1);
                    if(q1.front()->get_cpu_time()==q1.front()->get_bt()) {
                        q1.front()->set_completion_time(runTime);
                        q1.pop();
                    }
                    else if (procTime==timeSlice1) {
                        q1.push(q1.front());
                        q1.pop();
                        procTime = 0;
                    }
                    found = true;
                    break;
                }
            }

            if(found) break;

            size = q0.size();
            for(int i=0; i<size; i++) {
                if(q0.front()->get_IO()>0) {
                    q0.push(q0.front());
                    q0.pop();
                }

                else if(q0.front()->get_cpu_time()==q0.front()->get_bt()) {
                    q0.pop();
                }

                else {
                    if(q0.front()->get_process_id()!=prev) {
                        prev = q0.front()->get_process_id();
                        contextSwitch++;
                        procTime=0;
                    }
                    procTime++;
                    q0.front()->set_cpu_time(q2.front()->get_cpu_time()+1);
                    
                    if(q0.front()->get_response_time()==-1) q0.front()->set_response_time(runTime - q0.front()->get_at()-1);
                    if(q0.front()->get_cpu_time()==q0.front()->get_bt()) {
                        q0.front()->set_completion_time(runTime);
                        q0.pop();
                    }
                    else if (procTime==timeSlice0) {
                        q0.push(q0.front());
                        q0.pop();
                        procTime = 0;
                    }
                    found = true;
                    break;
                }
            }
            break;
        }
        if(!found) cout<<"No process to schedule"<<endl;

        for(auto it = ioList.begin(); it!=ioList.end(); it++) {
            if((*it)->get_IO()==0) {
                ioList.erase(it);
                it--;
            }
            else (*it)->set_IO((*it)->get_IO()-1);
         
            if((*it)->get_IO()==0) {
                ioList.erase(it);
                it--;
            }
        }
    }
    while(opt>0 && opt<=6);
    show_summary();

    return 0;
}