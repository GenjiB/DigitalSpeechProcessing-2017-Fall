#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

#include "hmm.h"

using namespace std;

HMM hmm[5];
int test_data[2500][50]={0};
void Viterbi(int & model,double &like,int file_num,int data_n){

  double likelihood=0.0;
  int model_predict=-1;

  for (int f =0; f<file_num;f++){
      double d[6][50]={0.0};

      //initialize
      for(int i=0;i<6;i++){
              d[i][0]=hmm[f].initial[i]*hmm[f].observation[test_data[data_n][0]][i];
      }


      for (int trans=1;trans<50;trans++){
          for(int j=0;j<6; j++){
              for(int i=0;i<6;i++){
                  double tmp= d[i][trans-1]*hmm[f].transition[i][j];
                      if(tmp>d[j][trans])
                         d[j][trans]=tmp;
                }
              d[j][trans]*=hmm[f].observation[test_data[data_n][trans]][j];

            }
      }
      double model_likelihood=0.0;
      for (int i=0;i<6;i++)
          if(d[i][49]>model_likelihood)
            model_likelihood=d[i][49];

      if(model_likelihood>likelihood){
        likelihood=model_likelihood;
        model_predict=f;
      }
  }
  model=model_predict;
  like=likelihood;

}


int main(int argc, char const *argv[]){
  char modellist[64],test_file[64],result[64];


  strcpy(modellist,argv[1]);
  strcpy(test_file,argv[2]);
  strcpy(result,argv[3]);

  load_models(modellist,hmm,5);

/*  test set: length:50
    number of data:2500
    state:6
    observation:6
*/

  fstream fin ;
  fin.open(test_file,ios::in);
  char temp[51];
  int i=0;
  while (fin.getline(temp,sizeof(temp),'\n')) {
    for (int j=0;j<50;j++)
      test_data[i][j]=temp[j]-'A';
    i++;
  }
  fin.close();


  ofstream fout;
  fout.open(result,ios::out);
  for(int n=0;n<2500;n++){
    int model_predict=-1;
    double likelihood=0.0;
    Viterbi(model_predict,likelihood,5,n);
    fout<<"model_0"<<model_predict+1<<".txt "<<likelihood<<endl;

  }
  fout.close();
  return 0;


}
