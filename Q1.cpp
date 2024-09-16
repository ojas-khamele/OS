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
        int priority;
        int currIO_time;

    public:
        process(int id, int arrival_time) {
            process_id = id;
            this->arrival_time = arrival_time;
            priority = 0;
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

        void set_priority(int priority) {
            this->priority = priority;
        }

        int get_priority() {
            return priority;
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

struct compare{         //Custom Compare function for priority queue
    bool operator()(process*& p1, process*& p2) {
        return p1->get_priority()<=p2->get_priority();
    }
};

vector<process*> processList;
vector<process*> ioList;
priority_queue<process*, vector<process*>, compare> readyList;

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
        cout<<"Set burst time : ";
        cin>>bt;

        newProc->set_bt(bt);
    }
    processList.push_back(newProc);
    readyList.push(newProc);
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
    processList[id]->set_priority(0);
}

void changePriorities(int alpha) {

    stack<process*> temp;
    while(!readyList.empty()) {
        temp.push(readyList.top());
        readyList.pop();
    }

    while(!temp.empty()) {
        
        if(temp.top()->get_cpu_time()<temp.top()->get_bt()) {
            if(temp.top()->get_IO()==0) temp.top()->set_priority(temp.top()->get_priority() + alpha);
            readyList.push(temp.top());
        }
        temp.pop();
    }
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
        changePriorities(0);
    }
}

void printDetails(process* p) {
    cout<<"Process id = "<< p->get_process_id()<<endl;
    cout<<"Burst Time = "<<p->get_bt()<<endl;
    cout<<"Cpu Time = "<<p->get_cpu_time()<<endl;
    cout<<"Priority = "<<p->get_priority()<<endl;
    if(p->get_IO()!=0) cout<<"IO time left = "<<p->get_IO()<<endl;
    cout<<endl;
}

void displayProcesses() {

    priority_queue<process*, vector<process*>, compare> pq;
    pq = readyList;

    while(!pq.empty()) {
        printDetails(pq.top());
        pq.pop();
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
    
    int alpha, beta;    // alpha = rate of increase in priority in ready queue, beta = rate of increase in priority in running state

    alpha = 2;
    beta = 1;

    int opt;

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
        
        if(readyList.empty()) {
            runTime++;
            continue;
        }        
         
        process* temp = readyList.top();
        readyList.pop();

        if(temp->get_bt()>temp->get_cpu_time()) {
            
            temp->set_priority(temp->get_priority()+beta);
            temp->set_cpu_time(temp->get_cpu_time()+1);
            if(temp->get_response_time()==-1) temp->set_response_time(runTime-temp->get_at());
            
            if(!readyList.empty()) changePriorities(alpha);            
            readyList.push(temp);
            
            process* newTop = readyList.top();
            if((newTop->get_process_id())!=temp->get_process_id()) contextSwitch++;
        }
        
        else {
            temp->set_completion_time(runTime);
            contextSwitch++;
        }

        runTime++;

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