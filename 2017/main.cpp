#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
using namespace std;

void swap(int*& row1, int*& row2)
{
    int* temp = row1;
    row1 = row2;
    row2 = temp;
}

int partition(int** array, int start, int end)
{
    int pivot = array[end][0];
    int partitionidx = start;

    for (int i = start; i < end; i++)
    {
        if (array[i][0] < pivot) {
            swap(array[i], array[partitionidx]);
            partitionidx++;
        }
    }

    swap(array[partitionidx], array[end]);
    return partitionidx;
}

void quicksort(int** array, int start, int end)
{
    if (start < end)
    {
        int partition_idx = partition(array, start, end);
        quicksort(array, start, partition_idx - 1);
        quicksort(array, partition_idx + 1, end);
    }
}


class Cache{
public:
    int capacity;
    int no;
    bool* videos;
    Cache(int capacity_, int no_videos,int no_){
        capacity = capacity_;
        no = no_;
        videos = new bool[no_videos];
        for(int i=0; i<no_videos; i++){
            videos[i] = false;
        }
    }

    void add(int video_size, int video_no){
        videos[video_no] = true;
        capacity -= video_size;
    }
};


class Endpoint{
public:
    int latency_dc;
    int* latency_cache;
    Cache** caches;
    int connection;

    Endpoint(int latencyDC, int Connection, int no_){
        latency_dc = latencyDC;
        connection = Connection;
        latency_cache = new int[connection];
        caches = new Cache*[connection];
    }
};

class Request{
public:
    int video_no, no_request;

    Endpoint* endpoint;
    Request(int video_no_, Endpoint* endpoint_, int no_request_){
        video_no = video_no_;
        endpoint = endpoint_;
        no_request = no_request_;
    }
};


int main() {
    ifstream input("videos_worth_spreading.in");
    if(input.fail()){
        cerr<<"Fail to open file"<<endl;
        exit(1);
    }

    int no_videos, no_endpoints, no_requests, no_caches, size_cache;

    input>>no_videos>>no_endpoints>>no_requests>>no_caches>>size_cache;

    Cache** caches = new Cache*[no_caches];
    for(int i=0; i<no_caches; i++){
        caches[i] = new Cache(size_cache, no_videos, i);
    }

    int* video_size = new int[no_videos];
    for(int i=0; i<no_videos;i++){
        input>>video_size[i];
    }

    Endpoint** endpoints = new Endpoint*[no_endpoints];
    for(int i=0; i<no_endpoints; i++){
        int latency_dc, connection;
        input>>latency_dc>>connection;
        endpoints[i] = new Endpoint(latency_dc,connection, i);

        for(int j=0; j<connection; j++) {
            int cache_no;
            input>>cache_no;
            endpoints[i]->caches[j] = caches[cache_no];

            int cache_latency;
            input>>cache_latency;
            endpoints[i]->latency_cache[j] = cache_latency;
        }
    }


    Request** requests = new Request*[no_requests];
    for(int i=0; i<no_requests; i++){
        int video_no, endpoint_no, num_request;
        input>>video_no>>endpoint_no>>num_request;
        requests[i] = new Request(video_no, endpoints[endpoint_no], num_request);
    }
//==================================================================================================
    int no_scores=0;
    for(int i=0; i<no_requests; i++){
        no_scores+=requests[i]->endpoint->connection;
    }

    int** score_table = new int*[no_scores];
    int count=0;
    for(int i=0; i<no_requests; i++){
        for(int j=0; j<requests[i]->endpoint->connection; j++){
            score_table[count] = new int[4];
            int score = 0;
            int time_saved = 0;
            time_saved = requests[i]->endpoint->latency_dc - requests[i]->endpoint->latency_cache[j];
            score = requests[i]->no_request * time_saved/pow(video_size[requests[i]->video_no],0.33333333);
            score_table[count][0] = score;
            score_table[count][1] = requests[i]->endpoint->caches[j]->no;
            score_table[count][2] = i;
            score_table[count][3] = requests[i]->video_no;
            count++;
        }
    }

    quicksort(score_table,0,no_scores-1);

    bool *request_already = new bool[no_requests];
    for(int i =0; i<no_requests; i++){
        request_already[i] = false;
    }

    int flag = 0;
    for(int i=no_scores-1; i>=0; i--)
    {
        if(!request_already[score_table[i][2]])
        {
            for (int j = 0; j < requests[score_table[i][2]]->endpoint->connection; j++)
            {
                if(requests[score_table[i][2]]->endpoint->caches[j]->videos[score_table[i][3]])
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                if (caches[score_table[i][1]]->capacity - video_size[score_table[i][3]] >= 0)
                {
                    caches[score_table[i][1]]->add(video_size[score_table[i][3]], score_table[i][3]);
                    request_already[score_table[i][2]] = true;
                }
            }
            else
            {
                flag = 0;
            }
        }
    }

    int count_caches=0;
    for(int i=0; i<no_caches; i++){
        if(caches[i]->capacity < size_cache){
            count_caches++;
        }
    }

    cout<<count_caches<<endl;

    ofstream out("out_zoo0.txt");

    out<<count_caches<<endl;

    for(int i=0; i<no_caches; i++){
        out<<i<<" ";
        for(int j=0; j<no_videos; j++){
            if(caches[i]->videos[j]){
                out<<j<<" ";
            }
        }
        out<<endl;
    }


    out.close();
    input.close();
    return 0;
}