#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>
using namespace std;

/*読み込んだ行(string型)を数列(integer)に変える*/
vector<double> split_data(const string& data){
    vector<double> ans;
    double num = 0;
    bool minus = false;//符号が-の時true。
    double decimal_digit = 1;
    bool read_data = true;
    for (int i=0; i<(int)data.size(); i++){
        if (data[i]=='-') minus = true;
        else if (data[i]=='.') decimal_digit *= 0.1;
        else if (data[i]>='0' && data[i]<='9'){
            read_data = true;
            if (decimal_digit!=1){
                num += (data[i]-'0')*decimal_digit;
                decimal_digit *= 0.1;
            }else num = num*10+(data[i]-'0');
        }else{
            if (read_data){
                if (minus) num *= -1;
                ans.push_back(num);
                read_data = false;
                num = 0;
                minus = false;
                decimal_digit = 1;
            }
        }
    }
    ans.push_back(num);
    return ans;
}

/*ファイルを読み込む、{[No], [r], [z], [|E|]}*/
vector<vector<double>> read_file(const string& file_name){
    vector<vector<double>> read_data;
    ifstream file(file_name);
    assert(file);
    string data;
    vector<double> ans;
    getline(file, data); //不要な一行目を読みこむ
    while (getline(file, data)){
        ans = split_data(data);
        assert(ans.size()==7);
        read_data.push_back({ans.at(0), ans.at(1), ans.at(2), ans.at(6)});
    }
    return read_data;
}

/*logで均等な配列を作る。pythonのlogspaceと同じ*/
vector<double> log_space(const double min_num, const double max_num, const int arr_size){
    vector<double> array;
    double parameter = (max_num-min_num)/arr_size;
    for (int i=0; i<arr_size; i++){
        array.push_back(pow(10, min_num+i*parameter)); //底は10
    }
    return array;
}

/*実験テキストp213の式(23)(24)(25)の条件分岐*/
double calc_a(double E,double p){
    double x = E/p;
    double a = 0;
    if (x>=31.6 && x < 60) a = p*(1.047*(x-28.5)*(x-28.5)-12.6)/10000;
    else if (x>60 && x < 100) a = (1-0.00674755*(x-60))*(1.047*(x-28.5)*(x-28.5)-12.6)*p/10000;
    else if (x>=100) a = 15*p*exp(-365/x);
    return a;
}

/*配列の中の最小値を返す*/
double min_num_in_array(vector<double>& arr){
    double min = arr.at(0);
    for (int i=1; i<(int)arr.size(); i++){
        if (min>arr.at(i)) min = arr.at(i);
    }
    return min;
}

void write_file(const vector<double>& x, const vector<double>& y, const string& file_name){
    string filename(file_name);
    fstream file_out;
    file_out.open(filename, std::ios_base::out);
    assert(file_out.is_open());
    assert(x.size()==y.size());
    for (int i=0; i<(int)x.size(); i++){
        file_out << x.at(i) << " " << y.at(i) << endl;
    }
    cout << "file write succeeded." << endl;
}

int main(){
    int plot_size = 100;
    vector<vector<double>> data = read_file("data1.csv");
    vector<double> pascal = log_space(-1.15, 4, plot_size);
    double max_no = data[(int)data.size()-1][0]; //ファイルの最大のNo(電気力線の本数)
    vector<double> V;

    /*計算する*/
    for (double p : pascal){ //pに関するfor文
        vector<double> V_array; //各電気力線でもとまったKの値を入れる
        int data_index = 0; //data配列のどこを見ているか
        for (int i=1; i<=max_no; i++){ //各電気力線に関するfor文
            /*にぶたん*/
            double left = 0;
            double right = 1E10;
            double V;
            int index;
            while (right-left > 0.001){
                index = data_index+1;
                V = (right+left)/2;  //今回の候補となるV
                double K = 0;
                while (index<(int)data.size() && data[index][0]==i){
                    double avg_E = (data[index][3]+data[index-1][3])*1000/2;
                    double r = sqrt((data[index][1]-data[index-1][1])*(data[index][1]-data[index-1][1])+
                    (data[index][2]-data[index-1][2])*(data[index][2]-data[index-1][2]));
                    double a = calc_a(V*avg_E, p);
                    K += a*r;
                    index++;
                }
                if (K>4.5) right = V;
                else left = V;
            }
            V_array.push_back(V);
            data_index = index;
        }
        V.push_back(min_num_in_array(V_array));
    }

    write_file(pascal, V, "ans1.txt");
}
