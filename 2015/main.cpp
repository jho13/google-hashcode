#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int min_elt(int* list, int size)
{
    int min = list[0];
    for (int i = 0; i < size; i++)
    {
        if (list[i] < min)
            min = list[i];
    }
    return min;
}

int score(const char* filename)
{
    ifstream data;
    data.open("dc2.in");
    int R, S, U, P, M;
    data >> R; data >> S; data >> U; data >> P; data >> M;

    bool unavailable[R][S];
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < S; j++)
            unavailable[i][j] = false;
    }
    int a,b;
    for (int i = 0; i < U; i++)
    {
        data >> a;
        data >> b;
        unavailable[a][b] = true;
    }

    ifstream answer;
    answer.open(filename);
    char ch;
    int first, second, third;
    int table[P][R];
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < P; j++)
            table[j][i] = 0;
    }
    int size, capacity;
    bool occupied[R][S];
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < S; j++)
            occupied[i][j] = false;
    }
    while (answer.get(ch))
    {
        cout << ch;
        if (ch != 'x')
        {
            answer.putback(ch);
            answer >> first; answer >> second; answer >> third;
            data >> size; data >> capacity;
            for (int i = 0; i < size; i++)
            {
                if (second + i >= S)
                {
                    cout << "exceeds slots";
                    return -1;
                }
                if (unavailable[first][second + i])
                {
                    cout << "unavailable";
                    return -1;
                }
                if (occupied[first][second + i])
                {
                    cout << "occupied";
                    return -1;
                }
                occupied[first][second  + i] = true;
            }
            table[third][first] += capacity;
        }
    }
    int table2[P][R];
    for (int i = 0; i < P; i++)
    {
        int sum = 0;
        for (int j = 0; j < R; j++)
            sum += table[i][j];
        for (int j = 0; j < R; j++)
            table2[i][j] = sum - table[i][j];
    }
    int gc[P];
    for (int i = 0; i < P; i++)
        gc[i] = min_elt(table2[i], R);

    return min_elt(gc, P);
}


struct Server{
    int no;
    int size;
    int capacity;
    int pool=-1;
    int row = -1;
    int slot = -1;
    double ratio;
};

int find_first_avail(int** dc, int row, int slot) {
    int start = -1;
    for (int i = 0; i < slot; i++) {
        if (dc[row][i] == -2) {
            start = i;
            break;
        }
    }

    if (start != -1) {
        int size = 1;
        for (int i = start + 1; i < slot; i++) {
            if (dc[row][i] == - 2) {
                size++;
            } else {
                break;
            }
        }
        return size;
    }

    return 0;
}


void sort_servers(const Server* servers, int no_server, int* sorted_server_no) {
    Server* server_copy = new Server[no_server];

    for (int i = 0; i < no_server; i++) {
        server_copy[i] = servers[i];
    }

    for(int i = 0; i < no_server; i++) {
        int max_server_no = i;
        for (int j = i + 1; j < no_server; j++) {
            if (server_copy[j].ratio > server_copy[max_server_no].ratio) {
                max_server_no = j;
            }
        }

        Server temp = server_copy[i];
        server_copy[i] = server_copy[max_server_no];
        server_copy[max_server_no] = temp;
    }

    for (int i = 0; i < no_server; i++) {
        sorted_server_no[i] = server_copy[i].no;
    }
}

int main() {
    ifstream in;
    in.open("dc.in");
    if(in.fail()){
        cout<<"Can't open file"<<endl;
        exit(0);
    }

    int no_rows=0;
    int no_slots=0;
    int no_unavail_slots=0;
    int no_pool=0;
    int no_server=0;

    in>>no_rows;
    in>>no_slots;
    in>>no_unavail_slots;
    in>>no_pool;
    in>>no_server;

    int** unavail_slot = new int*[no_unavail_slots];
    for(int i = 0; i < no_unavail_slots; i++){
        unavail_slot[i] = new int[2];
        in>>unavail_slot[i][0];
        in>>unavail_slot[i][1];
    }

    //int** server = new int*[no_server];

    Server* servers = new Server[no_server];
    for(int i=0; i<no_server; i++){
        in>>servers[i].size;
        in>>servers[i].capacity;
        servers[i].ratio = static_cast<double>(servers[i].capacity)/servers[i].size;
        servers[i].no = i;
    }

    int total_capacity=0;
    int total_size=0;
    for(int i=0; i<no_server; i++){
        total_capacity += servers[i].capacity;
        total_size += servers[i].size;
    }


    int** data_center = new int*[no_rows];
    for(int i=0; i<no_rows; i++){
        data_center[i] = new int[no_slots];
        for(int j=0; j<no_slots; j++){
            data_center[i][j] = -2;
        }
    }

    for(int i=0; i<no_unavail_slots; i++){
        int row_no = unavail_slot[i][0];
        int slot_no = unavail_slot[i][1];
        data_center[row_no][slot_no] = -1;
    }

    /*
    for(int i=0; i<no_rows; i++){
        for(int j=0; j<no_slots; j++){
            cout<<data_center[i][j]<<" ";
        }
        cout<<endl;
    }
     */


    int* sorted_servers = new int[no_server];

    for (int i = 0; i < no_server; i++) {
        sorted_servers[i] = i;
    }

    sort_servers(servers, no_server, sorted_servers);

    //for (int i = 0; i < no_server; i++) {
    //cout << sorted_servers[i] << " ";
    //    cout << servers[sorted_servers[i]].ratio << " ";
    //}

    int no_avail_slot = no_rows*no_slots - no_unavail_slots;
    int current_row=0;
    while(no_avail_slot){
        int first_avail_size = find_first_avail( data_center,current_row, no_slots);
        if(first_avail_size){
            int best_server_no;
            for(int i=0; i<no_server; i++){
                if(sorted_servers[i] != -1 && servers[sorted_servers[i]].size<=first_avail_size){
                    best_server_no = sorted_servers[i];
                    sorted_servers[i] = -1;
                    break;
                }

                if(i == no_server-1){
                    for(int j=0; j<no_slots; j++){
                        if(data_center[current_row][j]==-2){
                            for(int k=0; k<first_avail_size; k++){
                                data_center[current_row][j++]=-3;
                                no_avail_slot--;
                            }
                            break;
                        }
                    }
                }
            }

            for(int j=0; j<no_slots; j++){
                if(data_center[current_row][j]==-2){
                    servers[best_server_no].row = current_row;
                    servers[best_server_no].slot = j;
                    for(int k=0; k<servers[best_server_no].size; k++){
                        data_center[current_row][j++]=best_server_no;
                        no_avail_slot--;
                    }
                    break;
                }
            }
        }

        current_row = (current_row+1)%no_rows;
    }

    int pool_no =0;
    for(int i=0; i<no_rows; i++){
        for(int j=0; j<no_slots; j++){
            if(data_center[i][j]>=0) {
                if(servers[data_center[i][j]].pool == -1){
                    servers[data_center[i][j]].pool = pool_no;
                    pool_no = (pool_no + 1) % no_pool;
                }
            }
        }
    }


    ofstream out;

    out.open("output.txt");
    if (out.fail()) {
        cerr << "fail to open out" << endl;
        return 1;
    }

    for(int i=0; i<no_server; i++){
        if(servers[i].row!= -1){
            out<<servers[i].row<<" "<<servers[i].slot<<" "<<servers[i].pool<<endl;
        }else{
            out<<"x"<<endl;
        }
    }


    int** scores = new int*[no_pool];

    for(int i=0; i<no_pool; i++){
        scores[i] = new int[no_rows];
        for(int j=0; j<no_rows; j++){
            scores[i][j] = 0;
        }
    }

    for(int i=0; i<no_server; i++){
        int j = servers[i].row;
        if(j!=-1){
            scores[servers[i].pool][j] += servers[i].capacity;
        }
    }

    for(int i=0; i<no_pool; i++){
        int max_index = 0;
        int total_score = 0;
        for(int j=0; j<no_rows; j++){
            if (scores[i][max_index] < scores[i][j]) {
                max_index = j;
            }
            total_score += scores[i][j];
            cout<<scores[i][j]<<"\t";
        }
        cout<<"\t" << total_score - scores[i][max_index] <<endl;
    }

    in.close();
    out.close();
    return 0;
}