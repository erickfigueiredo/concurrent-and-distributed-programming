#include <stdio.h>
#include <cuda.h>
#include <stdexcept>
#include <chrono>
#include <iostream>

#define BLOCK_SIZE 32

using namespace std;

/* A função CHECK a seguir pode ser utilizada caso seja necessário identificar
erros em chamadas da biblioteca CUDA. Exemplo: 
CHECK(cudaMalloc((void**)&x,size));
*/
#define CHECK(call)                                                            \
{                                                                              \
    const cudaError_t error = call;                                            \
    if (error != cudaSuccess)                                                  \
    {                                                                          \
        fprintf(stderr, "Error: %s:%d, ", __FILE__, __LINE__);                 \
        fprintf(stderr, "code: %d, reason: %s\n", error,                       \
                cudaGetErrorString(error));                                    \
    }                                                                          \
}

__global__ void calcVizinhos(int8_t *m, int linhas, int colunas){
    int i = blockIdx.y*blockDim.y + threadIdx.y;
    int j = blockIdx.x*blockDim.x + threadIdx.x;

    if(i >= linhas || j >= colunas || m[i*colunas + j] == 9) return;

    int vizinhos[][2] = {{0,1},{1,1}, {-1,1}, {-1,0}, {1,0}, {0,-1},{1,-1}, {-1,-1}};
    int auxX = 0;
    int auxY = 0;

    for(int k = 0; k < 8; k++) {
      auxX = i+vizinhos[k][0];
      auxY = j+vizinhos[k][1];

      if(auxX >=0 && auxX < linhas && auxY >=0 && auxY < colunas && m[auxX*colunas + auxY] == 9) {
          m[i*colunas + j] ++;
      }
    }
}

void calcVizinhosSerial(int8_t *m, int linhas, int colunas){
  int vizinhos[][2] = {{0,1},{1,1}, {-1,1}, {-1,0}, {1,0}, {0,-1},{1,-1}, {-1,-1}};
  int auxX = 0, auxY = 0;

  for(int i = 0; i < linhas; i++){   
    for(int j = 0; j < colunas; j++){
        if(m[i*colunas+j] == 9) continue;

        for(int k = 0; k < 8; k++) {
          auxX = i+vizinhos[k][0];
          auxY = j+vizinhos[k][1];

          if(auxX >=0 && auxX < linhas && auxY >=0 && auxY < colunas && m[auxX*colunas + auxY] == 9)
            m[i*colunas + j] ++;
        }
    }
  }
}

void distribuiBombas(int8_t *a, int linhas, int colunas, int num){
    if (num > linhas*colunas)
        throw std::invalid_argument("Número de bombas extrapolou o máximo possível.");

    for (int i=0; i<linhas; ++i)
        for (int j=0; j<colunas; ++j)
            a[i*colunas+j]=0;

    int lin, col;
    for (int i=0; i<num; ++i) {
        lin=rand()%linhas;
        col=rand()%colunas;
        while (a[lin*colunas+col] == 9) {
            lin=rand()%linhas;
            col=rand()%colunas;
        }
        a[lin*colunas+col]=9;
    }
}

void imprimeMatriz(int8_t *a, int linhas, int colunas ){
    int v;
    for (int i=0; i<linhas; ++i) {
        for (int j=0; j<colunas; ++j) {
            v=a[i*colunas+j];
            if (v==9) printf("* ");
            else printf("%d ",v);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    int linha = 5000, col = 5000;
    bool calcOnGPU = true;

    int8_t *m = (int8_t *) malloc(linha*col*sizeof(int8_t));

    distribuiBombas(m, linha, col, 5);

    //imprimeMatriz(m, linha, col);

    if(calcOnGPU) {
      cout << "ON GPU\n";
      int8_t *mC;
      cudaMalloc((void **) &mC, linha*col*sizeof(int8_t));

      cudaMemcpy(mC, m,  linha*col*sizeof(int8_t), cudaMemcpyHostToDevice);
      dim3 grid (ceil(col*1.0/BLOCK_SIZE),ceil(linha*1.0/BLOCK_SIZE),1);
      dim3 block (BLOCK_SIZE,BLOCK_SIZE,1);

      calcVizinhos <<<grid, block>>>(mC, linha, col);

      cudaMemcpy(m, mC, linha*col*sizeof(int8_t), cudaMemcpyDeviceToHost);
      
      //imprimeMatriz(m, linha, col);  
      
      cudaFree(mC);
      cudaDeviceReset();
    } else {
      cout << "ON CPU\n";
      auto start = chrono::system_clock::now();
      calcVizinhosSerial (m, linha, col);
      auto end = chrono::system_clock::now();
      chrono::duration<double> time = end - start;

      printf("Tempo de Execução: %.10f\n", time.count());
      //imprimeMatriz(m, linha, col);  
    }

    free(m);

    return 0;    
}
