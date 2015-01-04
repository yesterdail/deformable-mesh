#include <math.h>
#include "taucs.h"

#define TAUCS_FACTORTYPE_NONE			0
#define TAUCS_FACTORTYPE_LLT_SUPERNODAL		1
#define TAUCS_FACTORTYPE_LLT_CCS		2

typedef struct {
  int  n;
  int  flags;
  int  type;
  int* rowperm;
  int* colperm;
  void* L;
} taucs_factorization;

void chol_update(void *vF, int index, double val,void**vetree){
   taucs_ccs_matrix *L;
   double *full_vec;
   int begin_of_col, end_of_col;
   int k,i,min;
   int *parent;
   double s,c;
   double up_val, down_val, norm;
   double tmp, *tmp_up_ptr, *tmp_down_ptr;  
   taucs_factorization *F;


   F=(taucs_factorization*)vF;
   //parent=(int *)(*vetree);
   
   if(F->type==TAUCS_FACTORTYPE_LLT_SUPERNODAL){
      L=taucs_supernodal_factor_to_ccs(F->L);
      F->type=TAUCS_FACTORTYPE_LLT_CCS;
      taucs_supernodal_factor_free(F->L);
      F->L=L;
   }else
      L=F->L;

   if(!(*vetree)){
      *vetree=malloc(L->n*sizeof(int));
      parent=(int *)(*vetree);
      for(i=0; i<L->n; i++){
         begin_of_col=L->colptr[i];
   	     end_of_col=L->colptr[i+1];      
         min=L->n;
         for(k=begin_of_col+1; k<end_of_col; k++)
            min=((L->rowind[k]<min)?(L->rowind[k]):min);
         parent[i]=min;
      }
   }else{
      parent=(int *)(*vetree);
   }

   index=F->colperm[index];
/*
   for(k=0; k<L->n; k++)
      if(F->rowperm[k]==index){
         index=k;  
         break;
      }
*/   
   full_vec=(double *)calloc(L->n,sizeof(double));
   full_vec[index]=val;

   while(index < L->n){
      begin_of_col=L->colptr[index];
      end_of_col=L->colptr[index+1];      
      up_val=L->values.d[begin_of_col];
      down_val=full_vec[index];
      norm=sqrt(up_val*up_val+down_val*down_val);
      s=down_val/norm;
      c=up_val/norm;
      for(k=begin_of_col; k<end_of_col; k++){
         tmp_up_ptr=L->values.d+k;
         tmp_down_ptr=full_vec+L->rowind[k];
         tmp=*tmp_up_ptr;
         *tmp_up_ptr=c*(*tmp_up_ptr)+s*(*tmp_down_ptr);
         *tmp_down_ptr=c*(*tmp_down_ptr)-s*tmp;
      }
      index=parent[index];
   }
   free(full_vec);

}
