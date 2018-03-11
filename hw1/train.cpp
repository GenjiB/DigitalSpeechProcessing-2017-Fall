#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

#include "hmm.h"

using namespace std;

HMM hmm; //global HMM model
int training_data[10000][50];         //10000 number of data  50 length



void Forward(double (&alpha)[6][50],int data_n){
  //initialize
  for(int i=0;i<6;i++)
    alpha[i][0]=hmm.initial[i] * hmm.observation[training_data[data_n][0]][i];
  //
  for(int trans=1;trans<50;trans++){
    for (int j=0;j<6;j++){
      for(int i=0; i<6 ;i++){
        alpha[j][trans]+=alpha[i][trans-1]*hmm.transition[i][j];
      }
      alpha[j][trans]*=hmm.observation[training_data[data_n][trans]][j];
    }
  }
}

void Backward(double (&beta)[6][50],int data_n){
  for(int i=0;i<6;i++)
    beta[i][49]=1;

  for(int trans=48;trans>-1; trans--)
    for (int i=0;i<6;i++)
      for (int j=0 ; j<6; j++)
          beta[i][trans]+=hmm.transition[i][j]*hmm.observation[training_data[data_n][trans+1]][j]*beta[j][trans+1];
}

void Gamma_cal(double (&g)[6][50],double a[][50], double b[][50]){
    for (int trans=0; trans<50;trans++){
      double sum =0;
      for (int i=0; i<6;i++){
        g[i][trans]=a[i][trans]*b[i][trans];
        sum+=g[i][trans];
      }
      for (int i=0;i<6;i++)
        g[i][trans]/=sum;
    }
}

void Update_obs(double (&o)[6][6],double g[][50],int data_n){
  for (int trans=0;trans<50;trans++)
    for (int i=0; i<6 ; i++)
        o[training_data[data_n][trans]][i]+=g[i][trans];

}
void Update_eps(double (&eps)[49][6][6],double a[][50],double b[][50],int data_n){
    for (int trans=0; trans<49;trans++){
      double sum=0;
        for(int i=0;i<6;i++){
          for (int j=0;j<6;j++){
            eps[trans][i][j]=a[i][trans]*hmm.transition[i][j]*hmm.observation[training_data[data_n][trans+1]][j]*b[j][trans+1];
            sum+=eps[trans][i][j];
          }
        }
        // Update_eps
        for (int i=0;i<6;i++)
            for(int j=0;j<6;j++)
              eps[trans][i][j]/=sum;
    }
}
void Update_all(double (&g_all)[6][50], double (&eps_all)[6][6],double g[][50],double eps[][6][6]){
      for(int i=0; i<6; i++)
          for(int j=0; j<50; j++)
            g_all[i][j]+=g[i][j];

      //eps
      for (int trans=0;trans<49;trans++)
          for(int i=0; i<6;i++)
              for(int j=0;j<6;j++)
                eps_all[i][j]+=eps[trans][i][j];
}


void train(char *train_file, int iteration){


      // reading file
      fstream fin ;
      fin.open(train_file,ios::in);
      char temp[51];
      int i=0;
      while (fin.getline(temp,sizeof(temp),'\n')) {
        for (int j=0;j<50;j++)
          training_data[i][j]=temp[j]-'A';
        i++;
      }
      fin.close();

      // training step
      for(int iter=0;iter<iteration;iter++){
         if(iter%50==0)
            cout<<"traing step"<<iter<<endl;

        double gamma_all[6][50]={0};
        double eps_all[6][6]={0};
        double obv_all[6][6]={0};
        double gamma_sum[6]={0.};
        //process data number
          for (int n=0;n<10000;n++){
            double alpha[6][50]={0.};
            double beta [6][50]={0.};
            double gamma[6][50]={0.};
            double eps[49][6][6]={0.};
            Forward(alpha,n);
            Backward(beta,n);
            Gamma_cal(gamma,alpha,beta);
            Update_obs(obv_all,gamma,n);
            Update_eps(eps,alpha,beta,n);
            Update_all(gamma_all,eps_all,gamma,eps);

        }
        // row sum
        for (int j=0;j<49;j++)
            for (int i=0;i<6;i++)
                    gamma_sum[i]+=gamma_all[i][j];

        //model parm
          for(int i=0; i<6;i++){
            hmm.initial[i]=gamma_all[i][0]/10000;
          }


          for(int i=0;i<6;i++)
              for(int j=0;j<6;j++)
                hmm.transition[i][j]=eps_all[i][j]/gamma_sum[i];

        //add last
          for (int i=0;i<6;i++)
              gamma_sum[i]+=gamma_all[i][49];

          for(int j=0;j<6;j++)
              for(int i=0;i<6;i++)
                  hmm.observation[i][j]=obv_all[i][j]/gamma_sum[j];
      }


}


int main(int argc, char const *argv[]) {

  //iter, model_init.txt, seq_model.txt, model.txt
  int iter=atoi(argv[1]);
  char initial_file[64],train_file[64],save_model[64];
  strcpy(initial_file,argv[2]);
  strcpy(train_file,argv[3]);
  strcpy(save_model,argv[4]);

  loadHMM(&hmm, initial_file);
  //iter=1; // for graph
  train(train_file,iter);


  FILE *f=fopen(save_model,"wb");
  dumpHMM(f,&hmm);
  fclose(f);

  return 0;
}
