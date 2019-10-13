#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Blob{
    public:
        Blob(int numOfFeatures){
            NumOfFeatures = numOfFeatures;
        }

        int GetNumOfSamples(){
            return int(data.size()/GetNumOfFeatures());
        }

        void GetSample(int index, double* sample_data){
            if(index>=GetNumOfSamples()){
                // out of range
                return;
            }
            int start =index*NumOfFeatures;
            int end = start+NumOfFeatures;
            for(int i=start; i<end;i++){
                sample_data[i-start] = data[i];
            }
        }

        void AddSample(double* sample_data){
            for(int i=0;i<NumOfFeatures;i++){
                data.push_back(sample_data[i]);
            }
        }

        void ToString(){
        }

        int GetNumOfFeatures(){
            return NumOfFeatures;
        }

        void Print(){
        }

    private:
        enum {};
        int NumOfFeatures;
        vector<double> data;

};
