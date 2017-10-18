#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
void print_leftover(int rows, int cols, char** pizza) {
    ofstream outfile;
    outfile.open("leftover.txt");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outfile << pizza[i][j];
        }
        outfile << endl;
    }
    outfile.close();
}

bool valid_slice(int start[2], int end[2], int no_row, int no_col, int mini_ingredient, int max_size, char** pizza){

    if(start[0]>=no_row){
        return false;
    }else if(start[1]>=no_col){
        return false;
    }else if(end[0]>=no_row){
        return false;
    }else if(end[1]>=no_col){
        return false;
    }

    int slice_size = (end[0]-start[0]+1)*(end[1]-start[1]+1);
    if(slice_size>max_size){
        return false;
    }else if(slice_size<mini_ingredient*2){
        return false;
    }

    int no_M=0;
    int no_T=0;

    for(int row = start[0]; row<=end[0]; row++){
        for(int col = start[1]; col<=end[1]; col++){

            if(pizza[row][col]=='M'){
                no_M++;
            }else if(pizza[row][col]=='T'){
                no_T++;
            }else{
                return false;
            }

            if(no_M>=mini_ingredient&&no_T>=mini_ingredient){
                return true;
            }
        }
    }

    /*
    if(no_M<mini_ingredient){
        return  false;
    }else if(no_T<mini_ingredient){
        return false;
    }
    */
    return false;
}

void make_occupied(int start_i, int start_j, int end_i, int end_j, char** pizza) {

    int j_copy = start_j;

    while (start_i <= end_i) {
        while (j_copy <= end_j) {
            pizza[start_i][j_copy] = ' ';
            j_copy++;
        }
        j_copy= start_j;
        start_i++;
    }

}

bool search_slice(int i, int j, int no_row, int no_col, char** pizza, int min_ingredient, int max_size, int end[2]) {

    int i_copy = i;
    int j_copy = j;

    int start[2] = {i, j};

    while (i_copy - i <= max_size && i_copy < no_row) {
        while ((j_copy - j + 1) * (i_copy - i + 1) <= max_size && j_copy < no_col) {

            end[0] = i_copy;
            end[1] = j_copy;

            if (valid_slice(start, end, no_row, no_col, min_ingredient, max_size, pizza)) {
                make_occupied(i, j, i_copy, j_copy, pizza);

                return true;
            }
            j_copy++;
        }

        // restart column and increment rows
        j_copy = j;
        i_copy++;
    }
    // cant find any
    return false;
}

bool extend_slice(int row, int col, int max_size, char** pizza, int* slices) {
    int vend_point[2], hend_point[2];
    int start_point[2];
    start_point[0] = slices[0];
    start_point[1] = slices[1];
    vend_point[0] = slices[2];
    vend_point[1] = slices[3];
    hend_point[0] = slices[2];
    hend_point[1] = slices[3];

    //extend vertically
    int  flag = 0;
    while (flag == 0)
    {
        vend_point[0]++;
        if (vend_point[0] >= row)
        {
            break;
        }
        for (int i = start_point[1]; i <= vend_point[1]; i++)
        {
            if (pizza[vend_point[0]][i] == ' ')
                flag = 1;
        }
        if ((vend_point[0] - start_point[0] + 1) * (vend_point[1] - start_point[1] + 1) > max_size)
            flag = 1;
    }
    vend_point[0]--;

    //extend horizontally
    flag = 0;
    while (flag == 0)
    {
        hend_point[1]++;
        if (hend_point[1] >= col)
        {
            break;
        }
        for (int i = start_point[0]; i <= hend_point[0]; i++)
        {
            if (pizza[i][hend_point[1]] == ' ')
                flag = 1;
        }
        if ((hend_point[0] - start_point[0] + 1) * (hend_point[1] - start_point[1] + 1) > max_size)
            flag = 1;
    }
    hend_point[1]--;

    if (vend_point[0] == slices[2] && hend_point[1] == slices[3])
        return false;

    //vertical is better than horizontal
    if ((vend_point[0] - start_point[0] + 1) * (vend_point[1] - start_point[1] + 1) > (hend_point[0] - start_point[0] + 1) * (hend_point[1] - start_point[1] + 1))
    {
        slices[2] = vend_point[0];
        make_occupied(slices[0], slices[1], slices[2], slices[3], pizza);
    }
    else
    {
        slices[3] = hend_point[1];
        make_occupied(slices[0], slices[1], slices[2], slices[3], pizza);
    }
    return true;
}

int main()
{
    ifstream input_file;
    //open file
    char filename[] = "big.in";
    input_file.open(filename);
    if (input_file.fail())
    {
        cout<<"Can't open file"<<endl;
        exit(0);
    }

    //read file
    int no_rows;
    int no_cols;
    int min_ingredient;
    int max_size;
    input_file >> no_rows;
    input_file >> no_cols;
    input_file >> min_ingredient;
    input_file >> max_size;

    //Read pizza
    char** pizza;
    pizza = new char*[no_rows];
    for (int i = 0; i < no_rows; i++)
        pizza[i] = new char[no_cols];

    for (int i = 0; i < no_rows; i++)
    {
        for (int j=0; j < no_cols; j++)
            input_file >> pizza[i][j];
    }
    input_file.close();

    int max_slices = (no_rows * no_cols) / (2 * min_ingredient);
    int slices[max_slices][4];
    int index = 0;

    int end[2];

    for (int i = 0; i < no_rows; i++)
    {
        for (int j = 0; j < no_cols; j++)
        {
            if (pizza[i][j] != ' ')
            {
                if (search_slice(i, j, no_rows, no_cols, pizza, min_ingredient, max_size, end))
                {
//                    for (int i = 0; i < no_rows; i++ ) {
//                        for ( int j = 0; j < no_cols; j++) {
//                            cout << pizza[i][j];
//                        }
//                        cout << endl;
//                    }
                    slices[index][0] = i;
                    slices[index][1] = j;
                    slices[index][2] = end[0];
                    slices[index][3] = end[1];
                    ++index;
                }
            }
        }
    }

    int count = 0;
    //index is number of slices
    for (int i = 0; i < index; i++)
    {
        if (extend_slice(no_rows, no_cols, max_size, pizza, slices[i]))
            count++;
    }

    for (int i = 0; i < index; i++)
    {
        if (extend_slice(no_rows, no_cols, max_size, pizza, slices[i]))
            count++;
    }

    cout << count;

    ofstream out;
    if (filename[0] == 'e')
    {}
    out.open("output.txt");
    out << index << endl;
    for (int i = 0; i < index ; i++)
    {
        for (int j = 0; j < 4; j++)
            out << slices[i][j] << " ";
        out << endl;
    }
    out.close();

    print_leftover(no_rows, no_cols, pizza);
    return 0;
}