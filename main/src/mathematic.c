#include "mathematic.h"

void insertion_sort(uint16_t arr[], int n){
      for(int i = 1; i < n ; i++){
            uint16_t key = arr[i];
            int j = i - 1;
            while(j >= 0 && arr[j] > key){
                  arr[j+1] = arr[j];
                  j=j-1;
            }
            arr[j+1] = key;
      }
}