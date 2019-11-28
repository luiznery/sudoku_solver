#include<iostream>
#include<list>
#include<sstream>
#include<fstream>
#include"math.h"


using namespace std;

class Node {
public:
    Node* _L;
    Node* _R;
    Node* _U;
    Node* _D;
    Node* _C;
    int _size, _id, _lin;
    bool h = 0;

    Node() {
        _L = nullptr;
        _R = nullptr;
        _U = nullptr;
        _D = nullptr;
        _C = nullptr;
    }

    

};


//Le o arquivo com o nome passado por parametro e armzena suas informaçoes em uma unica string
string text_to_string(string nome_arq){

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


//Retorna o valor de uma celula
int get_cel(int** matriz_sudoku, int n, int target) {
    if(target>=n*n){
        printf("Targer maior que o numero de celulas\n");
        return -1;
        }
    int lin = target/n;
    int col = target%n;
    return matriz_sudoku[lin][col];
}


//Transforma o sudoku em um problema de cobertura exata - exact cover
bool** sudokuToExactCover(int** matriz_sudoku, int* lin, int* col, int n, int n_col, int n_lin) {
    *lin = n*n*n; //Cada linha representará um valor que um campo do sudoku pode assumir, como temos n*n campos e cada campo pode assumir n valores, temos n*n*n linhas
    *col = 4*n*n; //Cada coluna irá repesentar cada uma das restrições (regras do sudoku - 4 regras: não repetiçao de numeros em um mesmo (1)campo, (2)linha, (3)coluna e (4)quadrante )
    //Para cada uma dessas regras temos que representar para cada campo do sudoku. Como temos n*n campos e 4 regras, precisamos de 4*n*n colunas

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

    for(int i = 0; i< *lin; i++){
        int val_cel = get_cel(matriz_sudoku, n, i/n); //encontra o valor de uma celula especifica do sudoku
        if(val_cel != 0){   //se esse valor nao for zero, ou seja, se ele estiver definido, isso é, se ele é uma pista 
            if(val_cel-1 == i%n){ //mantemos a linha se o valor que a linha representa for o mesmo que o valor que esta naquele celula
                // printf("nao zera: cel:%d - %d %d\n",i/n, i%n, val_cel-1);
                continue;
            } else {  // se nao, zeramos as outras linhas. Dessa forma garante-se que os valores dados estejam na solução, pois so teremos obrigatoriamente que escolhelo
                // printf("zera: cel: %d - %d %d\n",i/n, i%n, val_cel-1);
                for(int j = 0; j<*col; j++){
                    M[i][j] = 0;
                }
            }              
        }
    }            

    return M;
} 


//Cria a estrutura do DLX
Node* structure_DLX(int** matriz_sudoku, int n, bool** M, int lin, int col){
    Node* header = new Node();

    //Criando a matriz de nodes
    Node*** nodes = (Node***)malloc((lin+1)*sizeof(Node**));
    for (int i = 0; i < lin+1; i++){ 
        nodes[i] = (Node**) malloc(col * sizeof(Node*));
        for(int j = 0; j<col; j++){
            nodes[i][j] = (Node*) malloc(sizeof(Node));
        }              
    }

    //Conecntando os cabeçalhos
    header->_R = nodes[0][0];
    nodes[0][0]->_L = header;
    header->_L = nodes[0][col-1];
    nodes[0][col-1]->_R = header;
    for(int j = 0; j<col-1;j++ ){
        nodes[0][j]->_R = nodes[0][j+1];
        nodes[0][j+1]->_L = nodes[0][j];
    }

    //Conecta os nedes verticalmente
    for(int j = 0; j<col; j++) {
        nodes[0][j]->_id = j;
        nodes[0][j]->_C = nodes[0][j];
        nodes[0][j]->h = 1;
        Node* anterior = nodes[0][j]; //O sempre apontara inicialmente para o primeiro node da coluna, ou seja, o header
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

    // Conecta os nodes horizontalmente
    for(int i = 1; i<lin+1; i++){       
        Node* anterior = nullptr;
        Node* primeiro = nullptr;
        for(int j = 0; j <col; j++){
            if(M[i-1][j] == 1) {
                nodes[i][j]->_lin = i-1;
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
        if(anterior != nullptr)
            anterior->_R = primeiro;
        if(primeiro!=nullptr)
            primeiro->_L = anterior;
    }
    return header;
}


// Escolhe a coluna com menor numero de 1's
Node* chose_column(Node* header) {
    Node* menor = header->_R;
    for(Node* node = header->_R; node!=header; node=node->_R)
        if(node->_size<menor->_size)
            menor = node;
    return menor;
}


//Cobre a coluna c de acordo com a tecnica de DLX
void cover(Node* c) {
    c->_R->_L = c->_L;
    c->_L->_R = c->_R;
    for(Node* i = c->_D; i!=c; i = i->_D){
        for(Node* j = i->_R; j != i; j=j->_R){
            j->_U->_D = j->_D;
            j->_D->_U = j->_U;
            j->_C->_size--;
        }
    }
}


//Descobre a coluna c de acordo com a tecnica de DLX
void uncover(Node* c){
    for(Node* i = c->_U; i !=c ; i=i->_U){
        for(Node* j = i->_L; j!=i; j = j->_L ){
            j->_C->_size++;
            j->_D->_U = j;
            j->_U->_D = j;
        }
    }
    c->_R->_L = c;
    c->_L->_R = c;
}

void Solution(list<Node*> solution, int n, int** matriz_sudoku) {
    for(list<Node*>::iterator it = solution.begin(); it!=solution.end(); it++) {
        int cel = ((*it)->_lin)/n;
        int val = ((*it)->_lin)%n;
        matriz_sudoku[cel/n][cel%n] = val+1;
    }
}

void search(Node* header, list<Node*> solution, int n, int** matriz_sudoku) {
    if(header->_R == header ) {
        Solution(solution, n, matriz_sudoku);
        return;
    }
    Node* c = chose_column(header)->_C;
    // Node* c = header->_R;
    cover(c->_C);
    for(Node* r = c->_D; r!=c; r=r->_D){
        solution.push_back(r);
        for(Node* j = r->_R; j!=r; j=j->_R){
            cover(j->_C);
        }
        search(header, solution, n, matriz_sudoku);
        for(Node* j = r->_L; j!=r; j=j->_R)
            uncover(j->_C);
       
    }   
    uncover(c->_C);
    return;
}


// void test(Node* header, int n) {
//     //header
//     header->_id = -1;
//     header->_C = header;
//     Node* node = header;
//     for(int i = 0; i< 3*(4*n*n+1); i++){
//         printf("%d ", node->_C->_id);
//         node = node->_R;
//     }
    
//     node = header->_R->_D;
//     for(int i = 0; i< 3*(4*n*n+1); i++){
//     }
//     printf("\n");
// }

bool check(int** inicial, int** resultado, int n, int n_lin, int n_col){
    for(int i = 0; i<n; i++){
        for(int j = 0; j< n; j++) {
            if(inicial[i][j]!=0 and resultado[i][j]!=inicial[i][j]){
                printf("i: %d - j: %d\n", i, j);
                return 0;
            }
        }
    }

    int vet[10];
    for(int k = 0; k<10; k++)
        vet[k]=0;
    
    for(int i = 0; i<n; i++){
        for(int k = 0; k<10; k++)
            vet[k]=0;
        for(int j = 0; j< n; j++) {
           vet[resultado[i][j]] ++;
        }
        for(int k = 1; k<n+1; k++)
            if(vet[k]<1 or vet[k]>1){
                printf("i: %d \n", i);
                return 0;
            }
    }

    for(int i = 0; i<n; i++){
        for(int k = 0; k<10; k++)
            vet[k]=0;

        for(int j = 0; j< n; j++) {
           vet[resultado[j][i]] ++;
        }

        for(int k = 1; k<n+1; k++)
            if(vet[k]<1 or vet[k]>1){
                printf("j: %d\n", i);
                return 0;
            }
    }

    int ri;
    int rj;
    for(int k = 0; k < n_lin*n_col; k++){    
        ri = k/n_lin * n_lin;
        rj = k%n_lin * n_col;
        for(int i = ri; i < ri+n_lin; i++ ){
            for(int j = rj; j<rj+n_col;j++){             
                vet[resultado[i][j]] ++;
            }
        }
      
    }
    return 1;    
}


int main(int argc, char const *argv[]){
    //Lendo o arquivo
    stringstream ss(text_to_string(argv[1]));
    
    //Criando variveis de entrada e definindo seus valores de acordo com a entrada
    int n, n_col, n_lin; 
    ss >> n;
    ss >> n_col;
    ss >> n_lin;
    
    //Cria a matriz para representar o sudoku e à preenche
    int **matriz_sudoku = (int**)malloc(n * sizeof(int*));
    int **matriz_sudoku_teste = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++){ 
        matriz_sudoku[i] = (int*) malloc(n * sizeof(int));
        matriz_sudoku_teste[i] = (int*) malloc(n * sizeof(int));
        for(int j = 0; j < n; j++){
            ss >> matriz_sudoku[i][j];
            matriz_sudoku_teste[i][j] = matriz_sudoku[i][j];
        }
    }

    //Criando e definindo as variaveis que armazenarão as informações de num de linhas e colunas da matriz usada para representar 
    //o problema de cobertura exata, orginado da transformação do sudoku
    int* mlin = new (int);
    int* mcol = new (int);

    //Criando e armazenando o matriz que representara o sudoku como um problema de cobertura exata - exact cover
    bool** M = sudokuToExactCover(matriz_sudoku, mlin, mcol, n, n_col, n_lin);

    Node* header = structure_DLX(matriz_sudoku, n, M, *mlin, *mcol);
    
    list<Node*> solution;
    search(header, solution, n, matriz_sudoku);
    
    printf("Original:\n");
    imprime_matriz(matriz_sudoku_teste,n);
    printf("\nSolução:\n");
    imprime_matriz(matriz_sudoku,n);
    printf("\n%d\n\n", check(matriz_sudoku_teste, matriz_sudoku, n, n_lin,n_col));
    
    return 0;
}