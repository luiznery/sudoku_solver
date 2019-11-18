#include<iostream>
#include<list>
#include<sstream>
#include<fstream>
#include"math.h"
#include"dlx.h"

using namespace std;

//Le o arquivo com o nome passado por parametro e armzena suas informaçoes em uma unica string
string text_em_string(string nome_arq){

    fstream arq;
    arq.open(nome_arq);
    string text = "";

    while(!arq.eof()){
        string aux;
        getline(arq, aux);
        text = text + aux + ' ';
    }

    arq.close();
    
    return text;
}

//Imprime a matriz dada
void imprime_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++){ 
        for(int j = 0; j < n; j++){
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

//Imprime a matriz dada 
void imprime_matriz(int** matriz, int lin, int col ) {
    for (int i = 0; i < lin; i++){ 
        for(int j = 0; j < col; j++){
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

void imprime_matriz(bool** matriz, int lin, int col, int n) {
    for (int i = 0; i < lin; i++){
        printf("C%2d %d - ", i/n, i%n );
        for(int j = 0; j < col; j++){
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

//Transforma o sudoku em um problema de cobertura exata - exact cover
bool** sudokuToExactCover(int** matriz_sudoku, int* lin, int* col, int n, int n_col, int n_lin) {
    *lin = n*n*n; //Cada linha representará um valor que um campo do sudoku pode assumir, como temos n*n campos e cada campo pode assumir n valores, temos n*n*n linhas
    *col = 4*n*n; //Cada coluna irá repesentar cada uma das restrições (regras do sudoku - 4 regras: não repetiçao de numeros em um mesmo (1)campo, (2)linha, (3)coluna e (4)quadrante )
    //Para cada uma dessas regras temos que representar para cada campo do sudoku. Como temos n*n campos e 4 regras, precisamos de 4*n*n colunas
    int num_quadrantes = n; //Numero de quadrantes

    //Alocando a matriz M que representara nossas restriçoes
    bool **M = (bool**)malloc(*lin * sizeof(bool*));
    for (int i = 0; i < *lin; i++){ 
        M[i] = (bool*) malloc(*col * sizeof(bool));
        for(int j = 0; j < *col; j++){
            M[i][j] = 0;
        }
    }

    //Definindo as restriçoes da matriz M
    for(int cel = 0; cel < n*n; cel++ ){
        for(int num = 0; num < n; num ++){
            M[n*cel+num][cel] = 1; //Definindo as restrições de celula
            M[n*cel+num][n*n + num + n*(cel/n)] = 1; //Definindo as restrições de linha
            M[n*cel+num][2*n*n + num + n*(cel%n)] = 1; //Definindo as restrições de coluna
            M[n*cel+num][ 3*n*n + ((cel/(n*n_lin))%n_col)*n_lin*n  + ((cel/n_col)%n_lin)*n + num] = 1; //Definindo as restrições de qudrante
        }
    }            

    return M;
} 


//Cria a estrutura do DLX
Node* estrutura_DLX(bool** M, int lin, int col){
    Node* header = new Node();

    //Criando cabeçalhos
    Node*** nodes = (Node***)malloc((lin+1)*sizeof(Node**));
    for (int i = 0; i < lin+1; i++){ 
        nodes[i] = (Node**) malloc(col * sizeof(Node*));
        for(int j = 0; j<col; j++){
            nodes[i][j] = (Node*) malloc(sizeof(Node));
        }              
    }
    
    header->_R = nodes[0][0];
    nodes[0][0]->_L = header;
    header->_L = nodes[0][col-1];
    nodes[0][col-1]->_R = header;
    for(int j = 0; j<col-1;j++ ){
        nodes[0][j]->_R = nodes[0][j+1];
        nodes[0][j+1]->_L = nodes[0][j];
    }

    for(int j = 0; j<col; j++) {
        nodes[0][j]->_id = j;
        Node* anterior = nodes[0][j];//O sempre apontara inicialmente para o primeiro node da coluna, ou seja, o header
        int count = 0;
        for (int i = 1; i<lin+1; i++){
            if(M[i-1][j] == 1){
                count++;
                anterior->_D = nodes[i][j];
                nodes[i][j]->_U = anterior;
                nodes[i][j]->_C = nodes[0][j];
                anterior = nodes[i][j];
            }
        }
        anterior->_D = nodes[0][j];
        nodes[0][j]->_U = anterior;
        nodes[0][j]->_size = count;
    }

    for(int i = 1; i<lin+1; i++){
        Node* anterior = nullptr;
        Node* primeiro = nullptr;
        int count = 0;
        for(int j = 0; j <col; j++){
            if(M[i-1][j] ==1) {
                if(anterior != nullptr) {
                    nodes[i][j]->_L = anterior;
                    anterior->_R = nodes[i][j];
                    anterior = nodes[i][j];
                } else {
                    anterior = nodes[i][j];
                    primeiro = anterior;
                }
            }
        
        }
        printf("%d\n", count);
        anterior->_R = primeiro;
        primeiro->_L = anterior;
    }

    // for(Node* inode = header->_R; inode!=header; inode = inode->_R) {
    //     for (Node * jnode = inode->_D; jnode!=inode; jnode = jnode->_D)
            
    //     for (Node * jnode = inode->_U; jnode!=inode; jnode = jnode->_U)
            
        
    // }

    // for(Node* node = header->_R; node!=header; node = node->_R) // PERCORRE TODAS AS COLUNAS
    //     printf("%d ", node->_id);



    return header;
}


int main(int argc, char const *argv[]){
    //Lendo o arquivo
    stringstream ss(text_em_string(argv[1]));
    
    //Criando variveis de entrada e definindo seus valores de acordo com a entrada
    int n, n_col, n_lin; 
    ss >> n;
    ss >> n_lin;
    ss >> n_col;

    printf("n_lin = %d \n", n_lin);
    printf("n_col = %d \n", n_col);
    //Cria a matriz para representar o sudoku e à preenche
    int **matriz_sudoku = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++){ 
        matriz_sudoku[i] = (int*) malloc(n * sizeof(int));
        for(int j = 0; j < n; j++){
            ss >> matriz_sudoku[i][j];
        }
    }

    //Criando e definindo as variaveis que armazenarão as informações de num de linhas e colunas da matriz usada para representar 
    //o problema de cobertura exata, orginado da transformação do sudoku
    int* mlin = new (int);
    int* mcol = new (int);

    //Criando e armazenando o matriz que representara o sudoku como um problema de cobertura exata - exact cover
    bool** M = sudokuToExactCover(matriz_sudoku, mlin, mcol, n, n_col, n_lin); 
    
    imprime_matriz(M, *mlin, *mcol, n); //TESTE

    Node* header = estrutura_DLX(M, *mlin, *mcol);

    printf("\n%d\n\n", header->_R->_D->_L->_R->_R->_C->_id);
    


    return 0;
}

